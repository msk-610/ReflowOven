/**
 * @file PID_Ctrl.ino
 * @brief PID制御
 * @author msk-610
 * @date 2023/3/21
 */

/* reference :
 *  https://electronoobs.com/eng_arduino_tut24_2.php
 */

 /**
 * @fn
 * PID_Ctrl()
 * @brief PID制御
 * @param[in] (kp) PIDの比例パラメータ
 * @param[in] (ki) PIDの微分パラメータ
 * @param[in] (kd) PIDの積分パラメータ
 * @param[in] (target_temp) 設定温度
 * @param[in] (measured_temp) 読み取り温度
 * @param[in] (elapsedTime) 経過時間　[未使用]
 * @return PWMのDuty値
 */
int PID_Ctrl(float kp, float ki, float kd, float target_temp, float measured_temp, float elapsedTime)
{
  PID_error = target_temp - measured_temp;

  PID_p = kp * PID_error;

//  if(-5 < PID_error && PID_error > 5)
//  {
      PID_i = PID_i + (ki * PID_error);
//  }
  if(PID_i > 254)
  {
    PID_i = 254;
  }
  else if(PID_i < 0)
  {
    PID_i = 0;
  }
      
//  PID_d = kd * ((PID_error - previous_error)/elapsedTime);
  PID_d = kd * (PID_error - previous_error);
  previous_error = PID_error;

  PID_value = PID_p + PID_i + PID_d;
    
  if(PID_value < 0)
  {
    PID_value = 0;
  }
  if(PID_value > 254)
  {
    PID_value = 254;
  }
/*
    if(PID_error > 10)
    {
      PID_value = 254;
    }
    else if(PID_error < -5)
    {
      PID_value = 0;
    }
*/
  if(measured_temp >= 250)
  {
    PID_value = 0;
  }

  return (int)PID_value;
}
