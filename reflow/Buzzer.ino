/**
 * @file Buzzer.ino
 * @brief ブザーの制御
 * @author msk-610
 * @date 2023/3/21
 */

/**
 * @fn
 * BuzzerTask()
 * @brief ブザー制御のタスク
 * @param なし
 * @return なし
 */
void BuzzerTask(void *pvParameters)
{
  buzzer_t bz;

  while(1)
  {
    xQueueReceive(xQ_Buzzer, &bz, portMAX_DELAY);
#if 1
    if(bz == BZ_START_REFLOW)
    {
      digitalWrite(BUZZER_pin, HIGH);
      delay(100);
      digitalWrite(BUZZER_pin, LOW);
    }
    else if(bz == BZ_ERROR)
    {
      for(int i = 0; i < 10; i++)
      {
        digitalWrite(BUZZER_pin, HIGH);
        delay(50);
        digitalWrite(BUZZER_pin, LOW);
        delay(50);
      }
    }
    else if(bz == BZ_FIN_REFLOW)
    {
      digitalWrite(BUZZER_pin, HIGH);
      delay(50);
      digitalWrite(BUZZER_pin, LOW);
      delay(100);
      digitalWrite(BUZZER_pin, HIGH);
      delay(50);
      digitalWrite(BUZZER_pin, LOW);
      delay(100);
    }
    else if(bz == BZ_FINISH)
    {
      digitalWrite(BUZZER_pin, HIGH);
      delay(1000);
      digitalWrite(BUZZER_pin, LOW);
    }
#endif
  }
      vTaskDelete(NULL);
}

/**
 * @fn
 * Buzzer()
 * @brief ブザー制御タスクへのQueue送信
 * @param[in] (bz) enum buzzer_t
 * @return なし
 */
void Buzzer(buzzer_t bz)
{
    xQueueSend(xQ_Buzzer, &bz, 0);
}
