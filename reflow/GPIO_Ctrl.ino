/**
 * @file GPIO_Ctrl.ino
 * @brief GPIOの制御
 * @author msk-610
 * @date 2023/3/21
 */

/**
 * @fn
 * GPIO_Setup_Pins()
 * @brief GPIOの初期化
 * @param なし
 * @return なし
 */
void GPIO_Setup_Pins()
{
    pinMode(LCD_BACKLIGHT_pin,OUTPUT);
    digitalWrite(LCD_BACKLIGHT_pin, LOW);
    
    pinMode(SSR_pin, OUTPUT);
    ledcSetup(PWMCH, PWMFRQ, PWMRES);
    ledcAttachPin(SSR_pin, PWMCH);
    ledcWrite(PWMCH, 0);
    delay(100);
    
    pinMode(SSR_OPTION_pin, OUTPUT); 
    digitalWrite(SSR_OPTION_pin, LOW); 
    
    pinMode(BUZZER_pin, OUTPUT); 
    digitalWrite(BUZZER_pin, LOW); 
    
    pinMode (START_pin, INPUT_PULLUP);
}

/**
 * @fn
 * GPIO_Wait_Button_Low()
 * @brief GPIOピンがLowになるのを待つ
 * @param[in] (gpio_pin_num) ピン番号
 * @return なし
 */
void GPIO_Wait_Button_Low(int gpio_pin_num)
{
    while(1)
    {
      if(digitalRead(gpio_pin_num) == LOW)
      {
        delay(100);
        break;
      }
    }
}

/**
 * @fn
 * GPIO_Wait_Button_High()
 * @brief GPIOピンがHighになるのを待つ
 * @param[in] (gpio_pin_num) ピン番号
 * @return なし
 */
void GPIO_Wait_Button_High(int gpio_pin_num)
{
    while(1)
    {
      if(digitalRead(gpio_pin_num) == HIGH)
      {
        delay(100);
        break;
      }
    }
}

/**
 * @fn
 * GPIO_Select_Operation()
 * @brief スイッチ長押しで動作モードを変更する
 * @param[in] (gpio_pin_num) ピン番号
 * @return enum operation_t
 */
operation_t GPIO_Select_Operation(int gpio_pin_num)
{
  int btn_start_millis = 0;
  int btn_current_millis = 0;

  bool bool_reflow = false;
  bool bool_setting_upload = false;
  bool bool_measurement = false;
  operation_t select_operation = OPERATION_REFLOW;
  
  while(1)
  {
    if(digitalRead(gpio_pin_num) == LOW)
    {
      if(btn_start_millis == 0)
      {
        btn_start_millis = millis();
      }
      delay(100);
      btn_current_millis = millis();
      
      if((btn_current_millis - btn_start_millis >= 3000) && (btn_current_millis - btn_start_millis < 6000))
      {
        if(bool_setting_upload == false)
        {
          LCD_print_message(MSG_XMODEM);
        }
        bool_reflow = false;
        bool_setting_upload = true;
        bool_measurement = false;
        select_operation = OPERATION_JSON_UPLOAD;
      }
      else if((btn_current_millis - btn_start_millis >= 6000) && (btn_current_millis - btn_start_millis < 9000))
      {
        if(bool_measurement == false)
        {
          LCD_print_message(MSG_MEASUREMENT);
        }
        bool_reflow = false;
        bool_setting_upload = false;
        bool_measurement = true;
        select_operation = OPERATION_MEASUREMENT;
      }
      else
      {
        if(bool_reflow == false)
        {
          LCD_print_message(MSG_REFLOW);
        }
        bool_reflow = true;
        bool_setting_upload = false;
        bool_measurement = false;
        select_operation = OPERATION_REFLOW;

        if(btn_current_millis - btn_start_millis >= 9000)
        {
          btn_start_millis = millis();
          btn_current_millis = millis();
        }
      }
    }
    else
    {
      if(btn_start_millis != 0)
      {
        break;
      }
    }
  }

  return select_operation;
}