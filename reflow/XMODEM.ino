/**
 * @file XMODEM.ino
 * @brief XMODEMプロトコルの制御
 * @author msk-610
 * @date 2023/3/21
 */

const uint8_t SOH=0x01;
const uint8_t EOT=0x04;
const uint8_t ACK=0x06;
const uint8_t NAK=0x15;
const uint8_t CAN=0x18;
const uint8_t SUB=0x1a;

uint8_t head;
uint8_t block;
uint8_t inv_block;
uint8_t serial_data[129];
uint8_t checksum;
uint8_t sum_data = 0;

/**
 * @fn
 * xmodem_read()
 * @brief XMODEMプロトコルの解析
 * @param[in] (block_count) 受信するブロックカウント数
 * @return enum値 xmodem_t
 */
xmodem_t xmodem_read(uint8_t block_count)
{
  head = Serial.read();
  block = Serial.read();
  inv_block = Serial.read();
  for(int i = 0; i < 128; i++)
  {
    serial_data[i] = Serial.read();
  }
  checksum = Serial.read();

  if(head == CAN)
  {
    return XMODEM_CANCEL;
  }

  if(head == EOT)
  {
    return XMODEM_FINISH;
  }
    
  if(head != SOH)
  {
    return XMODEM_RETRY;
  }

  if((block ^ inv_block) != 0xFF)
  {
    return XMODEM_RETRY;
  }

  if(block != block_count)
  {
    return XMODEM_RETRY;
  }
  
  sum_data = 0;
  for(int i = 0; i < 128; i++)
  {
    sum_data += serial_data[i];
  }

  if(checksum != sum_data)
  {
    return XMODEM_RETRY;
  }

  for(int i = 0; i < 128; i++)
  {
    if(serial_data[i] == SUB)
    {
      serial_data[i] = 0x0;
    }
  }

  return XMODEM_NEXT_DATA;
}

/**
 * @fn
 * Xmodem()
 * @brief XMODEMプロトコルの制御
 * @param[out] (str) 受信した文字列　String形式
 * @return enum値 xmodem_t
 */
xmodem_t Xmodem(String* str)
{
  char data_char[129];
  data_char[128] = 0x0;
  uint8_t block_count = 1;
  
  String buff;
  xmodem_t result;
  for(int i = 0; i < 129; i++)
  {
    serial_data[i] = 0x0;
  }
  Serial.write(NAK);

  while(1)
  {
    delay(100);
    if(digitalRead(START_pin) == LOW)
    {
      result = XMODEM_CANCEL;
      break;
    }

    if(Serial.available()) 
    {
      result = xmodem_read(block_count);
  
      if(result == XMODEM_FINISH)
      {
        Serial.write(ACK);
        delay(500);
        break;
      }

      if(result == XMODEM_RETRY)
      {
        Serial.write(NAK);
      }
      
      if(result == XMODEM_CANCEL)
      {
        break;
      }
      
      if(result == XMODEM_NEXT_DATA)
      {
        for(int i = 0; i < 129; i++)
        {
          data_char[i] = (char)serial_data[i];
        }

        buff = String(data_char);
        *str += buff;
        block_count++;
      
        Serial.write(ACK);
      } 
    }
  }

  GPIO_Wait_Button_High(START_pin);
  delay(1000);

  Serial.println(*str);
  return result;
}
