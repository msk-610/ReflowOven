/**
 * @file MAX31855.ino
 * @brief MAX31855の制御
 * @author msk-610
 * @date 2023/3/21
 */

/*
 * HSPI
 * SCLK #14
 * MOSI #13
 * MISO #12
 * CS(SS) #15
 */
SPIClass hspi(HSPI);

/**
 * @fn
 * MAX31855_init()
 * @brief MAX31855のSPI I/Fの初期化
 * @param なし
 * @return なし
 */
void MAX31855_init() {
  //start and configure hardware SPI
  hspi.begin();
  hspi.setBitOrder(MSBFIRST);
  hspi.setDataMode(SPI_MODE0);
  hspi.setClockDivider(SPI_CLOCK_DIV4);
  hspi.setHwCs(true);
  
}


/**
 * @fn
 * MAX31855_readTemp()
 * @brief MAX31855から温度読み出し
 * @param なし
 * @return 温度（摂氏）
 */
max31855_t MAX31855_readTemp(double* temperature) {

  int32_t MAX31855_Value;

  MAX31855_Value = hspi.transfer32(0x00);
  *temperature = NAN;
  if (MAX31855_Value & 0x1 == 0x1)
  {
    //open circuit
    return MAX31855_OPEN_CIRCUIT; 
  }
  else if (MAX31855_Value & 0x2 == 0x2)
  {
    //short to GND
    return MAX31855_SHORT_TO_GND; 
  }
  else if (MAX31855_Value & 0x4 == 0x4)
  {
    //short to VCC
    return MAX31855_SHORT_TO_VCC; 
  }

  if (MAX31855_Value & 0x80000000)
  {
    MAX31855_Value =((MAX31855_Value >> 18) & 0x00003FFFF);
    MAX31855_Value |= 0xFFFFC000;
  }
  else
  {
    MAX31855_Value >>= 18;
  }
  //Serial.println(MAX31855_Value, HEX);
  
  double thermocouple_temp = MAX31855_Value;

  // LSB = 0.25 degrees C
  thermocouple_temp *= 0.25;
  //return thermocouple_temp;
  *temperature = thermocouple_temp;
  return MAX31855_OK;
}
