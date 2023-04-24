/**
 * @file reflow.ino
 * @brief リフローコントローラのmain
 * @author msk-610
 * @date 2023/3/21
 */

#include <Arduino.h>
#include <stdlib.h>
#include <SPI.h>

#include <Ucglib.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "param.h"

Ucglib_ILI9341_18x240x320_HWSPI ucg(LCD_DC, LCD_CS, LCD_RST);

/**
 * @fn
 * setup()
 * @brief 初期化
 * @param なし
 * @return なし
 */
void setup()
{
  Serial.begin(115200);

  GPIO_Setup_Pins();
  LCD_init();
  MAX31855_init();

  delay(100);
  LCD_print_message(MSG_INITIALIZE);
    
  xQ_Graph = xQueueCreate(1, sizeof(dot_t));
  xQ_Buzzer = xQueueCreate(1, sizeof(buzzer_t));
    
  xTaskCreateUniversal(
    LCD_GraphTask,
    "LCD_GraphTask",
    4096,
    NULL,
    1,
    &thg,
    CONFIG_ARDUINO_RUNNING_CORE
  );
    
  xTaskCreateUniversal(
    BuzzerTask,
    "BuzzerTask",
    1024,
    NULL,
    1,
    &thb,
    CONFIG_ARDUINO_RUNNING_CORE
  );
  
  if(!SPIFFS.begin(true))
  {
    delay(1000);
    LCD_print_message(MSG_SPIFFS_ERROR);
  }
  else
  {
    delay(1000);
  }

  LCD_DrawGraph();
}

/**
 * @fn
 * loop()
 * @brief 繰り返し制御
 * @param なし
 * @return なし
 */
void loop()
{
  uint32_t count = 0;
  uint32_t start_count = 0;

  float float_past_time = 0;
  float float_current_time = 0;
  float float_elapsed_time = 0;
  double double_temperature = 0;
  dot_t s_data;
  int int_PWM_duty = 254;
  operation_t selected_operation;

  LCD_print_message(MSG_START_UP);
  selected_operation = GPIO_Select_Operation(START_pin);
  delay(100);

  if(selected_operation == OPERATION_JSON_UPLOAD)
  {
    LCD_print_message(MSG_START_XMODEM);

    GPIO_Wait_Button_Low(START_pin);
    GPIO_Wait_Button_High(START_pin);
    LCD_print_message(MSG_UPDATE_XMODEM);

    String JsonFromUart = "";
    
    xmodem_t xmodem_result = Xmodem(&JsonFromUart);
    
    if(xmodem_result == XMODEM_FINISH)
    {
      JSON_Save_SettingFile(setting_file, JsonFromUart);
      LCD_print_message(MSG_FINISH_XMODEM);
    }
    else
    {
      LCD_print_message(MSG_CANCEL_XMODEM);
    }

    Buzzer(BZ_FINISH);
    delay(4000);
  }
  else if(selected_operation == OPERATION_REFLOW)
  {
    String string_setting = "";
    bool bool_setting_check = JSON_Read_SettingFile(setting_file, &string_setting);

    if(bool_setting_check == true)
    {
      int **sequence_value;
      uint32_t sequence_max;
      float kp, ki, kd;
      result_check_json = JSON_Convert_SettingFile_Json_To_Array(string_setting, &kp, &ki, &kd, &sequence_value, &sequence_max);
        
      if(result_check_json == JSON_OK)
      {
        //uint32_t sequence_max = sizeof(sequence_value[0]);
        uint32_t sequence = 0;
        bool bool_threashold = false;
        bool bool_clear_sequence = false;
        bool bool_finish_buzzer = false;

        LCD_print_message(MSG_START_REFLOW);
        Serial.printf("Kp = %lf\nKi = %lf\nKd = %lf\nSequence =\n[Temp], [Sec]\n",kp, ki, kd);
      
        for(int i = 0; i < sequence_max; i++)
        {
           Serial.printf("%5d , %4d\n",sequence_value[i][TEMPERATURE_CELL], sequence_value[i][TIME_CELL]);
        }
          
        if(bool_redraw_graph == true)
        {
          LCD_DrawGraph();
          bool_redraw_graph = false;
        }
    
        GPIO_Wait_Button_Low(START_pin);
        GPIO_Wait_Button_High(START_pin);
    
        Buzzer(BZ_START_REFLOW);
        Serial.printf("\ncount, temperature, target_temperature, PWM_duty\n");
    
        delay(1000);
    
        ledcWrite(PWMCH, int_PWM_duty);

        digitalWrite(SSR_OPTION_pin, HIGH);
          
        while (1)
        {
          float_current_time = millis();
          float_elapsed_time = float_current_time - float_past_time;

          if(float_elapsed_time >= ONE_SECOND)
          {
            max31855_t max31855_status;
            uint32_t retry_count = 0;
            
            while(1) 
            {
              max31855_status = MAX31855_readTemp(&double_temperature);
              
              if(max31855_status == MAX31855_OK)
              {
                break;
              }
              
              if(retry_count > 4)
              {
                break;
              }
              delay(100);
              retry_count++;
            }
            
            if(max31855_status != MAX31855_OK)
            {
              ledcWrite(PWMCH, 0);
              LCD_print_message(MSG_THERMOCOUPLE_ERROR);
              Buzzer(BZ_ERROR);
              break;
            }

            if(double_temperature >= ABSOLUTE_MAX_TEMPERATURE)
            {
              ledcWrite(PWMCH, 0);
              LCD_print_message(MSG_OVER_TEMP);
              Buzzer(BZ_ERROR);
              break;
            }

            if(sequence < sequence_max)
            {
              int_PWM_duty = PID_Ctrl(kp, ki, kd, sequence_value[sequence][TEMPERATURE_CELL], double_temperature, (float_elapsed_time/ONE_SECOND));
              ledcWrite(PWMCH, int_PWM_duty);
          
              Serial.printf("%3d, %6.2f, %d, %d\n", count, double_temperature, sequence_value[sequence][TEMPERATURE_CELL], int_PWM_duty);

              if(bool_threashold == false)
              {
                if(double_temperature >= sequence_value[sequence][TEMPERATURE_CELL])
                {
                  bool_threashold = true;
                  start_count = count;
                }                  
              }

              if(bool_threashold == true)
              {
                if(count - start_count > sequence_value[sequence][TIME_CELL])
                {
                  bool_threashold = false;
                  sequence++;
                }
              }
            }
            else //if(sequence >= sequence_max)
            {
              ledcWrite(PWMCH, 0);

              Serial.printf("%3d, %6.2f, %d, %d\n", count, double_temperature, 0, 0);
              if(bool_clear_sequence == false)
              {
                for(int i=0; i<sequence_max; i++)
                {
                  free(sequence_value[i]);
                }
                free(sequence_value);
                bool_clear_sequence = true;
              }

              if((double_temperature < 217) && (bool_finish_buzzer == false))
              {
                Buzzer(BZ_FIN_REFLOW);
                bool_finish_buzzer = true;
              }
            
              if(count >= LCD_X-LCD_X_OFFSET)
              {
                Buzzer(BZ_FINISH);
                break; 
              }
            }
              
            s_data.temperature = (float)double_temperature;
            s_data.count = (int)count;
            xQueueSend(xQ_Graph, &s_data, 0);
        
            float_past_time = float_current_time;
        
            count++;
          }
        
          if(digitalRead(START_pin) == LOW)
          {
            ledcWrite(PWMCH, 0);
            LCD_print_message(MSG_EMGCY_STOP);
            Buzzer(BZ_ERROR);
            break;
          }
        }
      }
      else //if(result_check_json != JSON_OK)
      {
        LCD_print_message(MSG_JSON_ERROR);
      }
    }
    else //if(bool_setting_check != true)
    {
      result_check_json = JSON_NOT_FOUND;
      LCD_print_message(MSG_JSON_ERROR);
      Buzzer(BZ_ERROR);
      delay(5000);
    }

    bool_redraw_graph = true;
  }
  else if(selected_operation == OPERATION_MEASUREMENT)
  {
    LCD_print_message(MSG_START_MEASUREMENT);
        
    if(bool_redraw_graph == true)
    {
      LCD_DrawGraph();
      bool_redraw_graph = false;
    }
    
    GPIO_Wait_Button_Low(START_pin);
    GPIO_Wait_Button_High(START_pin);
   
    Buzzer(BZ_START_REFLOW);
    Serial.println("");
     
    delay(1000);
    
    ledcWrite(PWMCH, int_PWM_duty);

    digitalWrite(SSR_OPTION_pin, HIGH);
      
    while (1)
    {
      float_current_time = millis();
      float_elapsed_time = float_current_time - float_past_time;

      if(float_elapsed_time >= ONE_SECOND)
      {
        max31855_t max31855_status = MAX31855_readTemp(&double_temperature);

        if(max31855_status != MAX31855_OK)
        {
          ledcWrite(PWMCH, 0);
          LCD_print_message(MSG_THERMOCOUPLE_ERROR);
          Buzzer(BZ_ERROR);
          break;
        }

        Serial.printf("%3d, %6.2f\n", count, double_temperature);

        if(double_temperature > MEASUREMENT_MAX_TEMPERATURE)
        {
          ledcWrite(PWMCH, 0);
          if(bool_redraw_graph == false)
          {
            Buzzer(BZ_FIN_REFLOW);
            break;
          }
        }              
        
        s_data.temperature = (float)double_temperature;
        s_data.count = (int)count;
        xQueueSend(xQ_Graph, &s_data, 0);
        
        float_past_time = float_current_time;
        
        count++;
      }
        
      if(digitalRead(START_pin) == LOW)
      {
        ledcWrite(PWMCH, 0);
        LCD_print_message(MSG_EMGCY_STOP);
        Buzzer(BZ_ERROR);
        break;
      }
    }

    bool_redraw_graph = true;
  }

  digitalWrite(SSR_OPTION_pin, LOW);
  
  GPIO_Wait_Button_High(START_pin);
}
