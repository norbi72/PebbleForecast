#include <pebble.h>

// #undef APP_LOG
// #define APP_LOG(...) 
#define MAX(a,b) \
  ({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; })
  
#define MIN(a,b) \
  ({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })

#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_ICON 2
#define KEY_SUNRISE_HOUR 3
#define KEY_SUNRISE_MIN 4
#define KEY_SUNSET_HOUR 5
#define KEY_SUNSET_MIN 6
#define KEY_WIND_SPEED 7
#define KEY_WIND_DIRECTION 8
#define KEY_PRESSURE 9
#define KEY_HUMIDITY 10
#define KEY_OZONE 11
#define KEY_FORECAST 12
#define KEY_FORECAST_FIRST_TIME 13
#define KEY_FORECAST_PRECIP_INTENSITY 14
#define KEY_FORECAST_PRECIP_PROBABILITY 15
#define KEY_READY 100

#define CONFIG_WEATHER_NO_FETCH_BEFORE 6  // O'CLOCK
#define CONFIG_WEATHER_NO_FETCH_AFTER 23  // O'CLOCK
#define CONFIG_WEATHER_REFRESH_EVERY 60   // MINUTES (default 30)
#define CONFIG_WEATHER_REFRESH_RETRY 5    // MINUTES (default 5), if normal refresh unsuccesfull
#define CONFIG_WEATHER_REFRESH_RETRY_COUNT 6  // TIMES
#define CONFIG_WEATHER_TOO_OLD 180  // MINUTES
  
#define PERSIST_KEY_WEATHER 1
  
typedef struct WeatherState {
  int temperature;
  char conditions[32];
  char icon[4];
  uint8_t sunrise_hour;
  uint8_t sunrise_min;
  uint8_t sunset_hour;
  uint8_t sunset_min;
  int wind_speed;
  int wind_direction;
  uint16_t pressure;
  uint16_t humidity;
  uint16_t ozone;
  time_t time;
  int8_t forecast[24];
  uint8_t forecast_precip_intensity[24];
  uint8_t forecast_precip_probability[24];
  uint8_t forecast_first_time;
  // However, since we set this struct as packed, the padding bytes are removed,
  // suitable to be used as a serialized form or saved in persistent storage.
} __attribute__((__packed__)) WeatherState;

WeatherState weatherState;

bool is_day[] = {false, false, false, false, false, false, false, false, false, false, false, false,   
                 false, false, false, false, false, false, false, false, false, false, false, false };
  
static Window *s_main_window;
//static Window *s_second_window;
static BitmapLayer *s_image_layer;
static BitmapLayer *s_weather_icon_layer;
static GBitmap *s_image_bitmap;

static GBitmap *s_weather_icon_bitmap;

static GBitmap *s_weather_icon_bitmap_day_rain;  
static GBitmap *s_weather_icon_bitmap_day_sunny;  
static GBitmap *s_weather_icon_bitmap_night_rain;  
static GBitmap *s_weather_icon_bitmap_night_clear; 

static Layer *s_battery_layer;
static Layer *s_draw_layer;
static Layer *s_window_layer;


static TextLayer *s_time_layer;
static TextLayer *s_graph_time_layer;
static TextLayer *s_graph_temp_layer;
static TextLayer *s_dow_layer;
static TextLayer *s_month_day_layer;
static TextLayer *s_weather_layer;
static TextLayer *s_wind_speed_layer;
static TextLayer *s_pressure_layer;
static TextLayer *s_humidity_text_layer;

static BitmapLayer *s_weather_icon_layer;
static BitmapLayer *s_humidity_icon_layer;

static GBitmap *s_humidity_icon_bitmap;

static BatteryChargeState batteryState;
const GSize battery_icon_size = {
  .w = 8,
  .h = 23
};
static GFont s_small_font_8;
static GFont s_showcard_font_30;
int s_screen_padding = 5;

bool bluetooth_connected = true;

GPath *s_my_path_ptr_bt;

//int wall_hour;
int wall_min;
int wall_min_prev;
//int wall_sec;
int wall_dow_num;
int wall_day_num;
static char wall_dow[] = "xxxxxxxx\0";
static char wall_month[] = "xxx\0";
static char wall_day[] = "xx\0";
static char wall_month_day[] = "xxx xx\0";

//static const char *days[] = { "", "hétfő", "kedd", "szerda", "csütörtök", "péntek", "szombat", "vasárnap" };
static const char *days[] = { "", "hétfő", "kedd", "szerda", "csütörtök", "péntek", "szombat", "vasárnap" };
const char *months[] = { "", "JAN", "FEB", "MÁR", "ÁPR", "MÁJ", "JÚN", "JÚL", "AUG", "SZE", "OKT", "NOV", "DEC" };

static const uint8_t precip_log_scale[] = {0,5,16,27,33,40,44,51,57,60,60,63,66,69,69,72,75,75,78,78,78,81,81,84,84,84,87,87,87};

const GPathInfo CHARGE_PATH_INFO = {
  .num_points = 7,
  .points = (GPoint []) {{5, 0}, {0, 5}, {3, 6}, {0, 11}, {7, 5}, {4, 4}, {5, 0}}
};
const int CHARGE_PATH_INFO_WIDTH = 6;

static GPath *s_charge_path_ptr = NULL;

const GPathInfo BT_PATH_INFO = {
  .num_points = 7,
  .points = (GPoint []) {{0, 4}, {8, 12}, {4, 16}, {4, 0}, {8, 4}, {0, 12}, {4, 8}}
};

const GPathInfo BATT_EMPTY_PATH_INFO = {
  .num_points = 12,
  .points = (GPoint []) {{2, 0}, {6, 3}, {9, 0}, {11, 2}, {8, 5}, {11, 9}, {9, 11}, {6, 8}, {2, 11}, {0, 9}, {3, 6}, {0, 2}}
  //.points = (GPoint []) {{3, 0}, {8, 5}, {13, 0}, {15, 3}, {10, 8}, {15, 12}, {12, 15}, {7, 10}, {3, 15}, {0, 12}, {5, 8}, {0, 3}}
};
const int BATT_EMPTY_PATH_INFO_HEIGHT = 11;
static GPath *s_batt_empty_path_ptr = NULL;

const GPathInfo CURR_TIME_MARKER_PATH_INFO = {
  .num_points = 3,
  .points = (GPoint []) {{0, 6}, {3, 0}, {-3, 0}}
};
const int CURR_TIME_MARKER_PATH_INFO_HEIGHT = 6;
static GPath *s_curr_time_marker_path_ptr = NULL;


#define KEY_LAST_ATTEMPT_TIME 9998
int last_weather_request_time = 0;
bool jsReady = false;
bool loadWeatherDelayed = false;

time_t last_tap_time = 0;
bool is_freehand_mode = false;
uint16_t freehand_rot_deg = 20;
AppTimer *freehand_timer;

char hourBuffer[] = "00\0";
char windBuffer[] = "0000\0";
char   HMbuffer[] = "00:00";

uint8_t weather_refresh_retry_count = 0;
bool SecLoaded = false;
bool AllUpdated = false;

#ifdef PBL_PLATFORM_APLITE
  #define COLOR_BG_DARK GColorBlack
  #define COLOR_BG_LIGHT GColorWhite
  #define COLOR_TIME GColorBlack
  #define COLOR_TIME_OBSTRUCTED GColorWhite
  #define COLOR_GRID_BORDER GColorWhite
  #define COLOR_GRID_LINES GColorBlack
  
  #define COLOR_GRID_BG_DAY1 GColorWhite
  #define COLOR_GRID_BG_DAY2 GColorBlack
  #define COLOR_GRID_BG_DAY_DITHER DITHER_70_PERCENT
  
  #define COLOR_GRID_BG_NIGHT1 GColorBlack
  #define COLOR_GRID_BG_NIGHT2 GColorBlack
  #define COLOR_GRID_BG_NIGHT_DITHER DITHER_0_PERCENT
  
  #define COLOR_GRAPH_TEMPERATURE GColorWhite
  #define COLOR_GRAPH_TEMPERATURE_NEG GColorWhite
  #define COLOR_BLUETOOTH GColorWhite
  #define COLOR_CHARGE_FLASH GColorWhite
  
  #define COLOR_GRAPH_PRECIP_HIGH1_NIGHT GColorWhite
  #define COLOR_GRAPH_PRECIP_HIGH2_NIGHT GColorBlack
  #define COLOR_GRAPH_PRECIP_HIGH1_DAY GColorWhite
  #define COLOR_GRAPH_PRECIP_HIGH2_DAY GColorBlack
  #define COLOR_GRAPH_PRECIP_HIGH_DITHER DITHER_70_PERCENT
  
  #define COLOR_GRAPH_PRECIP_LOW1_NIGHT GColorWhite
  #define COLOR_GRAPH_PRECIP_LOW2_NIGHT GColorBlack
  #define COLOR_GRAPH_PRECIP_LOW1_DAY GColorWhite
  #define COLOR_GRAPH_PRECIP_LOW2_DAY GColorBlack
  #define COLOR_GRAPH_PRECIP_LOW_DITHER DITHER_30_PERCENT
  
  #define COLOR_BATT_EMPTY_FILL GColorWhite
  #define COLOR_BATT_EMPTY_STROKE GColorBlack
  #define COLOR_BATT_EMPTY_BORDER GColorWhite
  const uint8_t battery_bar_colors[] = {
    GColorWhiteARGB8, GColorWhiteARGB8, GColorWhiteARGB8, GColorWhiteARGB8, GColorWhiteARGB8, 
    GColorWhiteARGB8, GColorWhiteARGB8, GColorWhiteARGB8, GColorWhiteARGB8, GColorWhiteARGB8, 
  };
#else

  #define COLOR_BG_DARK GColorBlack
  #define COLOR_BG_LIGHT GColorMintGreen
  #define COLOR_TIME GColorDukeBlue
  #define COLOR_TIME_OBSTRUCTED GColorWhite
  #define COLOR_GRID_BORDER GColorLightGray
  #define COLOR_GRID_LINES GColorDarkGray
  
  #define COLOR_GRID_BG_DAY1 GColorYellow
  #define COLOR_GRID_BG_DAY2 GColorRajah
  #define COLOR_GRID_BG_DAY_DITHER DITHER_50_PERCENT
  
  #define COLOR_GRID_BG_NIGHT1 GColorOxfordBlue
  #define COLOR_GRID_BG_NIGHT2 GColorBlack
  #define COLOR_GRID_BG_NIGHT_DITHER DITHER_50_PERCENT
  
  #define COLOR_GRAPH_TEMPERATURE GColorRed
  #define COLOR_GRAPH_TEMPERATURE_NEG GColorBlue
  #define COLOR_GRAPH_TEMPERATURE_CENTER GColorBlack
  #define COLOR_BLUETOOTH GColorVividCerulean
  #define COLOR_CHARGE_FLASH GColorSpringBud
  
  #define COLOR_GRAPH_PRECIP_HIGH1_NIGHT GColorPictonBlue
  #define COLOR_GRAPH_PRECIP_HIGH2_NIGHT GColorOxfordBlue
  #define COLOR_GRAPH_PRECIP_HIGH1_DAY GColorCeleste
  #define COLOR_GRAPH_PRECIP_HIGH2_DAY GColorVividCerulean
  #define COLOR_GRAPH_PRECIP_HIGH_DITHER DITHER_0_PERCENT

  #define COLOR_GRAPH_PRECIP_LOW1_NIGHT GColorCobaltBlue
  #define COLOR_GRAPH_PRECIP_LOW2_NIGHT GColorOxfordBlue
  #define COLOR_GRAPH_PRECIP_LOW1_DAY GColorBabyBlueEyes
  #define COLOR_GRAPH_PRECIP_LOW2_DAY GColorVividCerulean
  #define COLOR_GRAPH_PRECIP_LOW_DITHER DITHER_50_PERCENT
  
  #define COLOR_BATT_EMPTY_FILL GColorRed
  #define COLOR_BATT_EMPTY_STROKE GColorBlack
  #define COLOR_BATT_EMPTY_BORDER GColorWhite
  const uint8_t battery_bar_colors[] = {
      GColorFollyARGB8, GColorOrangeARGB8, 
      GColorChromeYellowARGB8, GColorLimerickARGB8, 
      GColorIslamicGreenARGB8, GColorYellowARGB8, 
      GColorGreenARGB8, GColorBrightGreenARGB8, 
      GColorWhiteARGB8};

#endif
