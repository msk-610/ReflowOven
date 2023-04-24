/**
 * @file LCD.ino
 * @brief LCDの描画制御
 * @author msk610
 * @date 2023/3/21
 */

/**
 * @fn
 * LCD_GraphTask()
 * @brief グラフ表示のためのタスク
 * @param なし
 * @return なし
 */

void LCD_GraphTask(void *pvParameters)
{
  dot_t r_data;
  dot_t prev_data;

  while (1)
  {
    xQueueReceive(xQ_Graph, &r_data, portMAX_DELAY);
    //Serial.printf("count = %d, temperature = %lf\n", r_data.count, r_data.temperature);
    ucg.setPrintPos(115, 15);
    ucg.setColor(0, 255, 0);
    ucg.setFont(ucg_font_8x13B_mf);
    if(r_data.temperature < 100)
    {
      ucg.print(" ");
    }
    ucg.print(r_data.temperature);
    ucg.print(" ");
    ucg.print("\xb0");
    ucg.print("C     ");

    r_data.count += LCD_X_OFFSET;
    r_data.temperature = LCD_Y - 10 * LCD_Y_SCALING - round(r_data.temperature * LCD_Y_SCALING);
    if (r_data.count != LCD_X_OFFSET)
    {
      ucg.setColor(0, 255, 0);
      ucg.drawLine(prev_data.count, (int)(prev_data.temperature), r_data.count, (int)(r_data.temperature));
    }
    prev_data = r_data;
  }

  vTaskDelete(NULL);
}

/**
 * @fn
 * LCD_print_message()
 * @brief LCDへの文字列、メッセージ表示
 * @param[in] (msg) 表示メッセージの選択
 * @return なし
 */
void LCD_print_message(print_massage_t msg)
{
  if (msg == MSG_INITIALIZE)
  {
    ucg.setColor(0, 0, 0);
    ucg.drawBox(0, 0, 239, 20);
    ucg.setColor(255, 16, 255);
    ucg.setFont(ucg_font_8x13B_mf);
    ucg.setPrintPos(15, 15);
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.print("Initializing - Please wait");
    Serial.println("Initializing SPIFFS - Please wait");
  }
  else if (msg == MSG_START_UP)
  {
    ucg.setColor(0, 0, 0);
    ucg.drawBox(0, 0, 239, 20);
    ucg.setColor(255, 32, 32);
    ucg.setFont(ucg_font_8x13B_mf);
    ucg.setPrintPos(15, 15);
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.print("Press Button - select O.P. ");
    Serial.println("Press Button - Start reflow");
  }
  else if (msg == MSG_START_REFLOW)
  {
    ucg.setColor(0, 0, 0);
    ucg.drawBox(115, 0, 239, 20);
    ucg.setColor(16, 255, 16);
    ucg.setFont(ucg_font_8x13B_mf);
    ucg.setPrintPos(115, 15);
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.print("- Press Button");
    Serial.println("\nStart Reflow Operation");
  }
  else if (msg == MSG_START_XMODEM)
  {
    ucg.setColor(0, 0, 0);
    ucg.drawBox(115, 0, 239, 20);
    ucg.setColor(16, 255, 255);
    ucg.setFont(ucg_font_8x13B_mf);
    ucg.setPrintPos(115, 15);
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.print("- Press Button");
    Serial.println("Press Button - Start XMODEM");
  }
  else if (msg == MSG_UPDATE_XMODEM)
  {
    ucg.setColor(0, 0, 0);
    ucg.drawBox(115, 0, 239, 20);
    ucg.setColor(16, 255, 255);
    ucg.setFont(ucg_font_8x13B_mf);
    ucg.setPrintPos(115, 15);
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.print("- Updating");
  }
  else if (msg == MSG_CANCEL_XMODEM)
  {
    ucg.setColor(0, 0, 0);
    ucg.drawBox(115, 0, 239, 20);
    ucg.setColor(16, 255, 255);
    ucg.setFont(ucg_font_8x13B_mf);
    ucg.setPrintPos(115, 15);
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.print("- Cancelled");
    Serial.println("Cancelled - XMODEM");
  }
  else if (msg == MSG_FINISH_XMODEM)
  {
    ucg.setColor(0, 0, 0);
    ucg.drawBox(115, 0, 239, 20);
    ucg.setColor(16, 255, 255);
    ucg.setFont(ucg_font_8x13B_mf);
    ucg.setPrintPos(115, 15);
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.print("- Saved");
    Serial.println("\nSaved JSON File - XMODEM");
  }
  else if (msg == MSG_START_MEASUREMENT)
  {
    ucg.setColor(0, 0, 0);
    ucg.drawBox(115, 0, 239, 20);
    ucg.setColor(255, 255, 16);
    ucg.setFont(ucg_font_8x13B_mf);
    ucg.setPrintPos(115, 15);
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.print("- Press Button");
    Serial.println("\nStart Measurement Operation");
  }
  else if (msg == MSG_REFLOW)
  {
    ucg.setColor(0, 0, 0);
    ucg.drawBox(0, 0, 239, 20);
    ucg.setColor(16, 255, 16);
    ucg.setFont(ucg_font_8x13B_mf);
    ucg.setPrintPos(20, 31);
    ucg.setPrintPos(15, 15);
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.print("REFLOW");
    Serial.println("Reflow mode");
  }
  else if (msg == MSG_XMODEM)
  {
    ucg.setColor(0, 0, 0);
    ucg.drawBox(0, 0, 239, 20);
    ucg.setColor(16, 255, 255);
    ucg.setFont(ucg_font_8x13B_mf);
    ucg.setPrintPos(15, 15);
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.print("UPDATE JSON");
    Serial.println("JSON File upload mode");
  }
  else if (msg == MSG_MEASUREMENT)
  {
    ucg.setColor(0, 0, 0);
    ucg.drawBox(0, 0, 239, 20);
    ucg.setColor(255, 255, 16);
    ucg.setFont(ucg_font_8x13B_mf);
    ucg.setPrintPos(15, 15);
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.print("MEASUREMENT");
    Serial.println("Temperature Measurement mode");
  }
  else if (msg == MSG_OVER_TEMP)
  {
    ucg.setColor(255, 189, 0);
    ucg.drawRBox(10, 130, 225, 80, 8);
    ucg.setFont(ucg_font_8x13B_tf);
    ucg.setPrintPos(20, 150);
    ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
    ucg.setColor(0, 0, 0);
    ucg.print("!!! Warning !!!");
    ucg.setPrintPos(20, 170);
    ucg.print(" High Temperature");
    ucg.setPrintPos(20, 190);
    ucg.print(" Stopped Reflow Operation");
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.setColor(0, 255, 0);
    Serial.println("\n!!! Warning !!!\n High Temperature!\n Stopped Reflow Operation\n");
  }
  else if (msg == MSG_EMGCY_STOP)
  {
    ucg.setColor(255, 255, 0);
    ucg.drawRBox(10, 130, 225, 80, 8);
    ucg.setFont(ucg_font_8x13B_tf);
    ucg.setPrintPos(20, 170);
    ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
    ucg.setColor(0, 0, 0);
    ucg.print("Detect Emergency Stop!");
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.setColor(0, 255, 0);
    Serial.println("\nDetect Emergency Stop!");
  }
    else if (msg == MSG_THERMOCOUPLE_ERROR)
  {
    ucg.setColor(255, 255, 0);
    ucg.drawRBox(10, 130, 225, 80, 8);
    ucg.setFont(ucg_font_8x13B_tf);
    ucg.setPrintPos(20, 170);
    ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
    ucg.setColor(0, 0, 0);
    ucg.print("Thermocouple ERROR!");
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.setColor(0, 255, 0);
    Serial.println("\nThermocouple ERROR!");
  }
  else if (msg == MSG_SPIFFS_ERROR)
  {
    ucg.setColor(255, 255, 0);
    ucg.drawRBox(10, 130, 225, 80, 8);
    ucg.setFont(ucg_font_8x13B_tf);
    ucg.setPrintPos(20, 170);
    ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
    ucg.setColor(0, 0, 0);
    ucg.print("SPIFFS Error!");
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.setColor(0, 255, 0);
    Serial.println("\nSPIFFS Error!");
  }
  else if (msg == MSG_JSON_ERROR)
  {
    ucg.setColor(255, 255, 0);
    ucg.drawRBox(10, 130, 225, 80, 8);
    ucg.setFont(ucg_font_8x13B_tf);
    ucg.setPrintPos(20, 150);
    ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
    ucg.setColor(0, 0, 0);
    ucg.print("JSON File Error");
    Serial.println("\nJSON File Error");
    ucg.setPrintPos(20, 170);
    if (result_check_json == JSON_NOT_FOUND)
    {
      ucg.print(" Cannot find JSON");
      Serial.println("\n Cannot find JSON");
    }
    else if (result_check_json == JSON_FORMAT_ERROR)
    {
      ucg.print(" JSON Format Error");
      Serial.println("\n JSON Format Error");
    }
    else if (result_check_json == JSON_KP_ERROR)
    {
      ucg.print(" JSON Kp Error");
      Serial.println("\n JSON Kp Error");
    }
    else if (result_check_json == JSON_KI_ERROR)
    {
      ucg.print(" JSON Ki Error");
      Serial.println("\n JSON Ki Error");
    }
    else if (result_check_json == JSON_KD_ERROR)
    {
      ucg.print(" JSON Kd Error");
      Serial.println("\n JSON Kd Error");
    }
    else if (result_check_json == JSON_SEQUENCE_ERROR)
    {
      ucg.print(" JSON Sequence Error");
      Serial.println("\n JSON Sequence Error");
    }
    else
    {
      ucg.print(" JSON Error");
      Serial.println("\n JSON Error");
    }
    ucg.setPrintPos(20, 190);
    ucg.print(" Upload Correct JSON");
    Serial.println("\n Upload Correct JSON");
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.setColor(0, 255, 0);
  }
}

/**
 * @fn
 * LCD_DrawGraph()
 * @brief LCDの目盛り描画
 * @param なし
 * @return なし
 */
void LCD_DrawGraph()
{
  String number[] = {"0", "50", "100", "150", "200", "250", "300"};
  int x = 0, x_adj = 10 - 4;
  int y = 0, y_adj = 11;
  int y_inv = LCD_Y;
  int max_res = LCD_X > LCD_Y ? LCD_X : LCD_Y;

  ucg.setFont(ucg_font_p01type_hr);
  
  ucg.setColor(0, 0, 0);
  ucg.drawBox(0, 21, LCD_X-1, LCD_Y-22);
  
  for (int i = 0; i <= max_res; i += 10)
  {

    // X scale
    if (i < LCD_X + 10)
    {
      if (i % 50 == 20)
      {
        ucg.setColor(189, 189, 189);
        if (x == 0)
          x_adj = 10 - 4;
        else if (x == 1)
          x_adj = 10 - 2;
        else if (x == 2)
          x_adj = 10;
        else if (x == 3)
          x_adj = 10;
        else if (x == 4)
          x_adj = 10;
        else if (x == 5)
          x_adj = 10;

        ucg.setPrintPos(i - x_adj, LCD_Y - 4);
        ucg.print(number[x]);
        x++ ;
      }
      else
      {
        ucg.setColor(64, 64, 64);
      }
      if (i >= 20)
      {
        ucg.drawLine(i - 5, LCD_Y_MAX, i - 5, LCD_Y_OFFSET);
      }
    }

    // Y scale
    //    if(i < LCD_Y_OFFSET)
    if (i * LCD_Y_SCALING < LCD_Y_OFFSET)
    {
      if (i % 50 == 10)
      {
        ucg.setColor(189, 189, 189);
        if (y == 0)
        {
          y_adj = 11;
        }
        else if (y == 1)
        {
          y_adj = 11 - 4;
        }
        else if (y == 2)
        {
          y_adj = 11 - 6;
        }
        else if (y == 3)
        {
          y_adj = 11 - 6;
        }
        else if (y == 4)
        {
          y_adj = 11 - 8;
        }
        else if (y == 5)
        {
          y_adj = 11 - 8;
        }

        ucg.setPrintPos(y_adj, (int)(y_inv - (i * LCD_Y_SCALING) + 2));

        ucg.print(number[y]);
        y++;
      }
      else
      {
        ucg.setColor(64, 64, 64);
      }
      int y_pos = y_inv - (i * LCD_Y_SCALING);
      if (LCD_Y_MAX <= y_pos)
      {
        ucg.drawLine(15, y_pos, LCD_X - 1, y_pos);
      }
    }
  }
}

/**
 * @fn
 * LCD_init()
 * @brief LCDの初期化
 * @param なし
 * @return なし
 */
void LCD_init()
{
  ucg.begin(UCG_FONT_MODE_SOLID);
  ucg.clearScreen();
#ifdef ROTATE90
    ucg.setRotate90();
#endif
  digitalWrite(LCD_BACKLIGHT_pin, HIGH);

}
