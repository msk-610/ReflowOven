#define TEMPERATURE_CELL 0
#define TIME_CELL 1

#define ABSOLUTE_MAX_TEMPERATURE 260
#define MEASUREMENT_MAX_TEMPERATURE 250
#define ONE_SECOND 1000

#define LCD_CS 5
#define LCD_DC 17
#define LCD_RST 16

#define START_pin 22
#define LCD_BACKLIGHT_pin 4
#define BUZZER_pin 32
#define SSR_pin 26
#define SSR_OPTION_pin 27

#define ROTATE90

#ifdef ROTATE90

#define LCD_X 320
#define LCD_Y 240
#define LCD_Y_SCALING 0.8

#else

#define LCD_X 240
#define LCD_Y 320
#define LCD_Y_SCALING 1

#endif

#define LCD_X_OFFSET 15
#define LCD_Y_OFFSET LCD_Y-10

#define LCD_Y_MAX 20

#define LCD_PRINT_POS_X 20
#define LCD_PRINT_POS_Y 15

typedef struct DOT{
  float temperature;
  int count;
} dot_t;

typedef enum
{
  OPERATION_REFLOW,
  OPERATION_JSON_UPLOAD,
  OPERATION_MEASUREMENT
} operation_t;

typedef enum
{
  MAX31855_OK,
  MAX31855_OPEN_CIRCUIT,
  MAX31855_SHORT_TO_GND,
  MAX31855_SHORT_TO_VCC
} max31855_t;

typedef enum
{
  MSG_INITIALIZE,
  MSG_START_UP,
  MSG_START_REFLOW,
  MSG_START_XMODEM,
  MSG_UPDATE_XMODEM,
  MSG_CANCEL_XMODEM,
  MSG_FINISH_XMODEM,
  MSG_START_MEASUREMENT,
  MSG_REFLOW,
  MSG_XMODEM,
  MSG_MEASUREMENT,
  MSG_OVER_TEMP,
  MSG_EMGCY_STOP,
  MSG_THERMOCOUPLE_ERROR,
  MSG_SPIFFS_ERROR,
  MSG_JSON_ERROR
} print_massage_t;

typedef enum
{
  BZ_START_REFLOW,
  BZ_ERROR,
  BZ_FIN_REFLOW,
  BZ_FINISH
} buzzer_t;

typedef enum
{
  XMODEM_NEXT_DATA,
  XMODEM_FINISH,
  XMODEM_RETRY,
  XMODEM_CANCEL
} xmodem_t;

typedef enum
{
  JSON_OK,
  JSON_NOT_FOUND,
  JSON_FORMAT_ERROR,
  JSON_KP_ERROR,
  JSON_KI_ERROR,
  JSON_KD_ERROR,
  JSON_SEQUENCE_ERROR
} json_check_t;

json_check_t result_check_json;

TaskHandle_t thg,thb;
QueueHandle_t xQ_Graph,xQ_Buzzer;

float PID_error = 0, PID_value = 0, previous_error = 0;
float PID_p = 0, PID_i = 0, PID_d = 0;

const int PWMCH = 0;
const double PWMFRQ = 33;
const int PWMRES = 8;

bool bool_redraw_graph = false;

const String setting_file = "/setting.json";
