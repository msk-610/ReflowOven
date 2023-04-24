/**
 * @file JSON_FILE.ino
 * @brief JSONファイルの保存、読み込み、変換
 * @author msk-610
 * @date 2023/3/21
 */

/**
 * @fn
 * JSON_Save_SettingFile()
 * @brief SPIFFSへのJSONファイルの保存
 * @param[in] (setting_file) 設定ファイルの保存ファイル名
 * @param[in] (str) JSON形式のString文字列
 * @return true:正常終了, false:SPIFFSエラー
 */
bool JSON_Save_SettingFile(String setting_file, String str)
{ 
    if(!SPIFFS.begin(true)){
      Serial.println("SPIFFS Mount Failed");
      return false;
    }

  File fp = SPIFFS.open(setting_file,"w");

  if(!fp)
  {
    SPIFFS.format();
    fp = SPIFFS.open(setting_file,"w");
  }

  if(fp)
  {
    fp.print(str);
    fp.close();
  }
  
  return true;
}

/**
 * @fn
 * JSON_Read_SettingFile()
 * @brief SPIFFSからJSONファイルの読み出し
 * @param[in] (setting_file) 設定ファイルのファイル名
 * @param[out] (str) JSON形式のString文字列
 * @return true:正常終了, false:ファイル読み込みエラー
 */
bool JSON_Read_SettingFile(String setting_file, String* str)
{
  File fp = SPIFFS.open(setting_file,"r");
  if(!fp)
  {
    Serial.println("Read Error: Setting File not available");
    return false;
  }

  while(fp.available()){
    *str = fp.readString();
  }
  fp.close();
  return true;
}

/**
 * @fn
 * JSON_Convert_SettingFile_Json_To_Array()
 * @brief JSONのDeserialize
 * @param[in] (setting_file) 設定ファイルのファイル名
 * @param[out] (kp) JSONファイルから読み出したKpの値
 * @param[out] (ki) JSONファイルから読み出したKiの値
 * @param[out] (kd) JSONファイルから読み出したKdの値
 * @param[out] (sequence_data) 温度と時間の設定値、2次元配列
 * @param[out] (sequence_max) sequence_dataの配列の数
 * @return JSON_OK:正常終了, それ以外:フォーマットエラー、設定値エラー
 */
json_check_t JSON_Convert_SettingFile_Json_To_Array(String str, float* kp, float* ki, float* kd, int*** sequence_data, uint32_t* sequence_max)
{
  StaticJsonDocument<512> doc;

  DeserializationError error = deserializeJson(doc,str);
  if(error)
  {
    Serial.println(error.f_str());
    return JSON_FORMAT_ERROR;
  }
  else
  {
   size_t JsonSize = 0;

   JsonSize = doc["Sequence"].size();

   *sequence_max = (uint32_t)JsonSize;

   if(doc.containsKey("Kp"))
   {
     *kp = doc["Kp"];
   }
   else
   {
     return JSON_KP_ERROR;
   }
   if(doc.containsKey("Ki"))
   {
     *ki = doc["Ki"];
   }
   else
   {
     return JSON_KI_ERROR;
   }
   if(doc.containsKey("Kd"))
   {
     *kd = doc["Kd"];
   }
   else
   {
     return JSON_KD_ERROR;
   }
   
   if(doc.containsKey("Sequence"))
   {
     *sequence_data = (int**)malloc(sizeof(int)*JsonSize);
     for(int i=0; i<JsonSize; i++)
     {
       (* sequence_data)[i] = (int*)malloc(sizeof(int)*2);
       (* sequence_data)[i][0] = doc["Sequence"][i][TEMPERATURE_CELL];
       (* sequence_data)[i][1] = doc["Sequence"][i][TIME_CELL];
     }
   }
   else
   {
     return JSON_SEQUENCE_ERROR;
   }
  }

  return JSON_OK;
}
