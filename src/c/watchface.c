/*
 * Demonstrate how to display a two color, black and white bitmap image with no
 * transparency.
 *
 * The original source image is from:
 *  <http://openclipart.org/detail/26728/aiga-litter-disposal-by-anonymous>
 *
 * The source image was converted from an SVG into a RGB bitmap using Inkscape.
 * It has no transparency and uses only black and white as colors.
 */

#include "pebble.h"
#include "pebble_assist.h"
#include "declarations.h"
#include "xprintf.h"
#include "dithered_rects.h"

void loadWeather() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Load weather start");
  if (!jsReady) 
  {
    loadWeatherDelayed = true;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "JS not ready, waiting");
    return;
  }
  loadWeatherDelayed = false;
  
  time_t currTime = time(NULL); 
  struct tm *local_tick_time = localtime(&currTime);
  strftime(hourBuffer, sizeof("00"), "%H", local_tick_time);
  int hour = atoi(hourBuffer);
  if (hour >= CONFIG_WEATHER_NO_FETCH_AFTER || hour < CONFIG_WEATHER_NO_FETCH_BEFORE) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Fetching weather data disabled now");
    return;
  }
  if (weather_refresh_retry_count > CONFIG_WEATHER_REFRESH_RETRY_COUNT) {
    // ha túl sokszor próbálkozott újra sikertelenül, akkor csak a 30 perces frissítés mehet    
    if ((local_tick_time->tm_min % CONFIG_WEATHER_REFRESH_EVERY)!=0) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Too many retries");
      return;
    }
  }
  if (currTime - weatherState.time > CONFIG_WEATHER_TOO_OLD*60) {
    // túl régi időjárási adat van kint -> inkább ne legyen semmi
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Too old weather - clearing");
    //text_layer_set_text(s_weather_layer, "");
    bitmap_layer_set_bitmap(s_weather_icon_layer, NULL);
  }
  
  // Begin dictionary
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
   
  dict_write_uint32(iter, KEY_LAST_ATTEMPT_TIME, currTime);
   
  // Send the message!
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sending load weather req");
  app_message_outbox_send();
  
  weather_refresh_retry_count++;
}

void showCurrentWeather(WeatherState wS) {
  //time_t currTime = time(NULL); 
  //struct tm *local_tick_time = localtime(&currTime);
  
  //static char weather_layer_buffer[32];
  static char pressure_buffer[9];
  static char humidity_buffer[5];
  
  snprintf(humidity_buffer, sizeof(humidity_buffer), "%d%%", wS.humidity);
  text_layer_set_text(s_humidity_text_layer, humidity_buffer);

  snprintf(pressure_buffer, sizeof(pressure_buffer), "%d hPa", wS.pressure);
  text_layer_set_text(s_pressure_layer, pressure_buffer);
  
  /*
  strftime(hourBuffer, sizeof("00"), "%H", local_tick_time);
  uint8_t currHour = atoi(hourBuffer);
  int8_t precipProbability = wS.forecast_precip_probability[(24 + currHour - wS.forecast_first_time) % 24];
  
  // ikon
  // nappal kirajzolása, 1 v. 2 részletben
  if (wS.forecast_first_time >= wS.sunrise_hour && wS.forecast_first_time < wS.sunset_hour) {
    // nappal van    
    if (precipProbability > 50) {  // >50% az eső valószínűsége
      s_weather_icon_bitmap = s_weather_icon_bitmap_day_rain;  
    } else {
      s_weather_icon_bitmap = s_weather_icon_bitmap_day_sunny;  
    }
  } else {
    if (precipProbability > 50) {  // >50% az eső valószínűsége
      s_weather_icon_bitmap = s_weather_icon_bitmap_night_rain;  
    } else {
      s_weather_icon_bitmap = s_weather_icon_bitmap_night_clear;  
    }
  }
  */
  
  layer_mark_dirty(s_draw_layer);
}

/*
static void update_only_sec(){
  if (!SecLoaded){
    window_stack_push(s_second_window, false);
    SecLoaded=true;  
    //optionally move frequently updated layer to frequently updated window.
  }
}

static void update_all(){
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "update_all");
  AllUpdated = true;
  if (SecLoaded){
    window_stack_pop(false);
    SecLoaded=false;
    //optionally move frequently updated layer to main window
  }
}
*/

/** 
 *  PERCENKÉNT LEFUTÓ CALLBACK
 *
 **/
void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // Get a tm structure
  time_t currTime = time(NULL); 
  struct tm *local_tick_time = localtime(&currTime);

  if ( (units_changed & DAY_UNIT) != 0 || strcmp(wall_dow, "xxxxxxxx")==0) {
    strftime(hourBuffer, sizeof("00"), "%u", local_tick_time);
    wall_dow_num = atoi(hourBuffer);
    strcpy(wall_dow, days[wall_dow_num]);
    text_layer_set_text(s_dow_layer, wall_dow);
    
    strftime(hourBuffer, sizeof("00"), "%d", local_tick_time);
    strncpy(wall_day, hourBuffer, sizeof("00"));
    //text_layer_set_text(s_day_layer, wall_day);
    
    wall_day_num = atoi(hourBuffer);
    
    layer_mark_dirty(s_draw_layer);
  }
  
  if ( (units_changed & MONTH_UNIT) != 0 || (units_changed & DAY_UNIT) != 0 || strcmp(wall_month_day, "xxx xx")==0) {
    strftime(hourBuffer, sizeof("00"), "%m", local_tick_time);
    strncpy(wall_month, months[atoi(hourBuffer)], sizeof(wall_month));
    snprintf(wall_month_day, sizeof(wall_month_day), "%s %s", wall_month, wall_day);
    
    layer_mark_dirty(s_draw_layer);
  }
  
  if ( (units_changed & MINUTE_UNIT) != 0 || wall_min == -1 ) {
    // Minutes changed
    
    // Use 24 hour format
    char* format = "%H:%M";    
    if(!clock_is_24h_style()) {
      // Use 12 hour format
      format = "%I:%M";
    }
    
    // Write the current hours and minutes into the buffer
    strftime(HMbuffer, sizeof("00:00"), format, local_tick_time);
  
    // Display this time on the TextLayer
    text_layer_set_text(s_time_layer, HMbuffer);
    //text_layer_set_text(s_graph_time_layer, HMbuffer);
    
    strftime(hourBuffer, sizeof("00"), "%M", local_tick_time);
    wall_min_prev = wall_min;
    wall_min = atoi(hourBuffer);
	
    // Get weather update every 30 minutes, or 5 minutes if the weather is more then 30 min old
    if(tick_time->tm_min % ((weatherState.time < currTime-CONFIG_WEATHER_REFRESH_EVERY*60) ? CONFIG_WEATHER_REFRESH_RETRY : CONFIG_WEATHER_REFRESH_EVERY) == 0) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Old weather, because cached time is: %li, current time: %li", weatherState.time, currTime);
      
      loadWeather();
    }
    
    // mindent újra kell rajzolni
    //layer_mark_dirty(s_draw_layer);

    //// csak az idő kiírást elég frissíteni
    layer_mark_dirty((Layer *)s_time_layer);
  }
  
  //if ( (units_changed & HOUR_UNIT) != 0 || wall_hour == -1 ) {
  //  strftime(hourBuffer, sizeof("00"), "%I", local_tick_time);
  //  wall_hour = atoi(hourBuffer);
  //}
  
  //strftime(hourBuffer, sizeof("00"), "%S", local_tick_time);
  //wall_sec = atoi(hourBuffer);

  
  if (loadWeatherDelayed) loadWeather();
}

static void battery_state_handler(BatteryChargeState charge_state) {
  if (batteryState.charge_percent != charge_state.charge_percent || batteryState.is_plugged != charge_state.is_plugged) {
    batteryState = charge_state;
  
    layer_mark_dirty(s_battery_layer);
  }
}

/* NOT NEEDED
void bluetooth_state_handler(bool connected) {
  bluetooth_connected = connected;
  //layer_mark_dirty(s_draw_layer);
}

void bluetooth_layer_update_proc(Layer *my_layer, GContext* ctx, GRect bounds) {
  
  //if (bluetooth_connected) {
    //GPoint center = GPoint(bounds.origin.x + 5, bounds.origin.y + 5); 
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "Bluetooth %c", bluetooth_connected);
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_circle(ctx, GPoint(100,20), 4);
  //}
}
*/

/* 
 ***************************************************
 *************** Másodperc mutató ******************
 ***************************************************
 */
/* NOT NEEDED
void seconds_layer_update_proc(Layer *my_layer, GContext* ctx) {
  //app_log(APP_LOG_LEVEL_DEBUG, "wf", 516, "wsec:%d", wall_sec);
  if (wall_sec < 0) return;
  GRect bounds = layer_get_frame(my_layer);
  
}
*/

void draw_battery_charge_bars(Layer *my_layer, GContext* ctx) {
  GRect bounds = layer_get_frame(my_layer);
  uint8_t p = 0;
  const uint8_t battLeft = bounds.origin.x + s_screen_padding + 1;
  const uint8_t battBottom = bounds.origin.y + s_screen_padding + battery_icon_size.h-3;
  
  for (p=20; p<=batteryState.charge_percent; p+=10) {
    const int vert = 2*(p/10-2);
    GPoint start = GPoint(battLeft, battBottom - vert);
    GPoint end = GPoint(battLeft + battery_icon_size.w-3, battBottom - vert);
    graphics_context_set_stroke_color(ctx, (GColor)battery_bar_colors[p/10-2] );
    graphics_draw_line(ctx, start, end);
    start.y++;
    end.y++;
    graphics_draw_line(ctx, start, end);
  }
}

// BATTERY LAYER
void battery_layer_update_proc(Layer *my_layer, GContext* ctx) {
  GRect bounds = layer_get_frame(my_layer);
  
  // draw 2 rectangles to represent the battery---
  GRect rect1;
  rect1.origin = GPoint(
    bounds.origin.x + s_screen_padding, 
    bounds.origin.y + s_screen_padding + 2
  );
  rect1.size = GSize(battery_icon_size.w, battery_icon_size.h-2);
  
  GRect rect2;
  rect2.origin = GPoint(
    bounds.origin.x + s_screen_padding + 2, 
    bounds.origin.y + s_screen_padding
  );
  rect2.size = GSize( battery_icon_size.w-4, 2);

  graphics_context_set_stroke_color(ctx, (GColor)battery_bar_colors[batteryState.charge_percent/10-2]);
  //graphics_context_set_fill_color(ctx, GColorWhite);
  if (batteryState.charge_percent < 20) {
    graphics_context_set_stroke_color(ctx, COLOR_BATT_EMPTY_BORDER);
    graphics_draw_rect(ctx, rect1);
    graphics_draw_rect(ctx, rect2);
    draw_battery_charge_bars(my_layer, ctx);
    
    // piros x
    graphics_context_set_fill_color(ctx, COLOR_BATT_EMPTY_FILL);
    graphics_context_set_stroke_color(ctx, COLOR_BATT_EMPTY_STROKE);
    gpath_draw_outline(ctx, s_batt_empty_path_ptr);
    gpath_draw_filled(ctx, s_batt_empty_path_ptr);
  } else {
    graphics_draw_rect(ctx, rect1);
    graphics_draw_rect(ctx, rect2);
    //graphics_fill_rect(ctx, rect3, 0, GCornerNone);
    draw_battery_charge_bars(my_layer, ctx);
  }
  
  if (batteryState.is_plugged) {
    
    if (!batteryState.is_charging) {
      GPoint center = GPoint(
        bounds.origin.x + s_screen_padding, 
        bounds.origin.y + s_screen_padding + battery_icon_size.h + 2
      );
      graphics_fill_circle(ctx, center, 3);
    } else {
      // Fill the path:
      graphics_context_set_fill_color(ctx, COLOR_CHARGE_FLASH);
      graphics_context_set_stroke_color(ctx, COLOR_CHARGE_FLASH);
      gpath_draw_filled(ctx, s_charge_path_ptr);
      // Stroke the path:
      gpath_draw_outline(ctx, s_charge_path_ptr);
    }
    
  } 
  
  // *** BLUETOOTH ***
  
  //GPoint btO = GPoint(bounds.size.w - s_screen_padding - 5, 20);
  if (bluetooth_connection_service_peek()) {
    //graphics_fill_circle(ctx, btCircleCenter, 4);
    
    graphics_context_set_stroke_color(ctx, COLOR_BLUETOOTH);
    gpath_draw_outline(ctx, s_my_path_ptr_bt);

  } else {
    graphics_context_set_stroke_color(ctx, COLOR_BLUETOOTH);
    GPoint btCircleCenter = GPoint(bounds.origin.x + s_screen_padding + 4, bounds.origin.y + s_screen_padding + battery_icon_size.h + 28);
    graphics_draw_circle(ctx, btCircleCenter, 4);
  }

}


// *********************************************************************
// **************** KÖZÖS LAYER AZ ÖSSZES KIRAJZOLÁSHOZ ****************
// *********************************************************************
void draw_layer_update_proc(Layer *my_layer, GContext* ctx) {
  GRect bounds = layer_get_frame(my_layer);
  
  graphics_context_set_fill_color(ctx, COLOR_BG_DARK);
  graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, 122), 0, GCornerNone);
  
  uint8_t x, y, y2, c=0, i=0;
  uint8_t day_start, day_width, day_start2, day_width2;
  for (uint8_t ii=0; ii<24; ii++) is_day[ii] = false;
  
  // teljes háttérszín - éjszaka
  draw_dithered_rect(ctx, GRect(37, 11, 94, 90), COLOR_GRID_BG_NIGHT1, COLOR_GRID_BG_NIGHT2, COLOR_GRID_BG_NIGHT_DITHER);
 
  // nappal kirajzolása, 1 v. 2 részletben
  if (weatherState.forecast_first_time >= weatherState.sunrise_hour && weatherState.forecast_first_time <= weatherState.sunset_hour) {
    // nappal van
    day_start = 37;
    uint8_t day_length_hours = weatherState.sunset_hour - weatherState.forecast_first_time;
    day_width = 4*day_length_hours + (uint8_t)(weatherState.sunset_min/15);
    
    for (uint8_t ii=0; ii<MIN(24, day_length_hours); ii++) is_day[ii] = true;

    // az eleje napos
    draw_random_dithered_rect(ctx, GRect(day_start, 11, day_width, 90), COLOR_GRID_BG_DAY1, COLOR_GRID_BG_DAY2, COLOR_GRID_BG_DAY_DITHER);
    
    // és a vége is
    uint8_t new_day_start_hour = 24-weatherState.forecast_first_time+weatherState.sunrise_hour;
    day_start2 = 37 + 4*new_day_start_hour + (uint8_t)(weatherState.sunrise_min/15);
    day_width2 = 133 - day_start2;
    
    for (uint8_t ii=new_day_start_hour; ii<24; ii++) is_day[ii] = true;
    
    draw_random_dithered_rect(ctx, GRect(day_start2, 11, day_width2, 90), COLOR_GRID_BG_DAY1, COLOR_GRID_BG_DAY2, COLOR_GRID_BG_DAY_DITHER);
  } else {
    
    uint8_t new_day_start_hour = 24-weatherState.forecast_first_time+weatherState.sunrise_hour;
    
    if ( weatherState.forecast_first_time > 12 ) {
      day_start = 37 + 4*new_day_start_hour + (uint8_t)(weatherState.sunrise_min/15);
    } else {
      new_day_start_hour = weatherState.sunrise_hour-weatherState.forecast_first_time;
      day_start = 37 + 4*new_day_start_hour + (uint8_t)(weatherState.sunrise_min/15);
    }
//app_log(APP_LOG_LEVEL_DEBUG, "is_day", 366, "new_day_start_hour:%d", new_day_start_hour);
    uint8_t day_length_hours = weatherState.sunset_hour - weatherState.sunrise_hour;
    day_width = 4*day_length_hours + (uint8_t)(weatherState.sunset_min/15) - (uint8_t)(weatherState.sunrise_min/15);
    if (day_start + day_width > 133) day_width = 133-day_start;
    
    for (uint8_t ii=new_day_start_hour; ii<MIN(24, new_day_start_hour+day_length_hours); ii++) is_day[ii] = true;

    draw_random_gradient_rect(ctx, GRect(day_start-4, 11, 9, 90), COLOR_GRID_BG_NIGHT1, COLOR_GRID_BG_DAY1, LEFT_TO_RIGHT);
    draw_random_dithered_rect(ctx, GRect(day_start+4, 11, day_width-4, 90), COLOR_GRID_BG_DAY1, COLOR_GRID_BG_DAY2, COLOR_GRID_BG_DAY_DITHER);
    //draw_random_gradient_rect(ctx, GRect(day_start, 11, day_width, 90), COLOR_GRID_BG_DAY1, COLOR_GRID_BG_DAY2, LEFT_TO_RIGHT);
  }
  
  graphics_context_set_stroke_color(ctx, COLOR_GRID_LINES);
  // függőleges rácsvonalak
  for (x = 45; x<=126; x+=8) {    
    graphics_draw_line(ctx, GPoint(x, 11), GPoint(x, c++%2==0 ? 101 : 104));
  } 
  
  time_t currTime = time(NULL);
  uint8_t currIndex = ((currTime - weatherState.time)/3600) % 24;
  if (currIndex > 0) {
    // piros háromszög
    gpath_move_to(s_curr_time_marker_path_ptr, GPoint(
      bounds.origin.x + 37 + 4*(currTime - weatherState.time)/3600, 
      bounds.origin.y + 11 - CURR_TIME_MARKER_PATH_INFO_HEIGHT
    ));
    
    graphics_context_set_fill_color(ctx, COLOR_BATT_EMPTY_FILL);
    graphics_context_set_stroke_color(ctx, COLOR_BATT_EMPTY_STROKE);
    gpath_draw_outline(ctx, s_curr_time_marker_path_ptr);
    gpath_draw_filled(ctx, s_curr_time_marker_path_ptr);
    
    // ikon
    // nappal kirajzolása, 1 v. 2 részletben
    uint8_t curr_precip_p = weatherState.forecast_precip_probability[currIndex];
    if (is_day[currIndex]) {
      // nappal van    
      if (curr_precip_p > 50) {  // >50% az eső valószínűsége
        s_weather_icon_bitmap = s_weather_icon_bitmap_day_rain;  
      } else {
        s_weather_icon_bitmap = s_weather_icon_bitmap_day_sunny;  
      }
    } else {
      if (curr_precip_p > 50) {  // >50% az eső valószínűsége
        s_weather_icon_bitmap = s_weather_icon_bitmap_night_rain;  
      } else {
        s_weather_icon_bitmap = s_weather_icon_bitmap_night_clear;  
      }
    }
  }
  
  // vízszintes rácsvonalak
  for (y = 19; y<94; y+=8) {
    graphics_draw_line(ctx, GPoint(37, y), GPoint(133, y));    
  }

  graphics_context_set_stroke_color(ctx, COLOR_GRID_BORDER);
  graphics_draw_rect(ctx, GRect(37, 11, 96, 90));
  
  int8_t min = weatherState.forecast[0];
  int8_t max = weatherState.forecast[0];
  for(uint8_t i=1; i<24; i++) {
    if (min > weatherState.forecast[i]) min = weatherState.forecast[i];
    if (max < weatherState.forecast[i]) max = weatherState.forecast[i];
  }
  //app_log(APP_LOG_LEVEL_DEBUG, "wf", 366, "min:%d, max:%d", min, max);
  max++;
  min--;
  
  // min - max tartományt skálázni 90 px-re
  const int16_t scale = 90*100 / (max - min);  // *100, a nagyobb pontosság miatt
  uint8_t precip_i, precip_p;
  
  y = 101 - (weatherState.forecast[0]-min)*scale/100;
  for (x = 37, i=0; x<=129; x+=4, i++) {
    // csapadék és annak valószínűsége
    precip_i = weatherState.forecast_precip_intensity[i] > 28 ? 90 : precip_log_scale[weatherState.forecast_precip_intensity[i]];
    precip_p = weatherState.forecast_precip_probability[i];
    
    GColor c1 = COLOR_GRAPH_PRECIP_LOW1_NIGHT;
    GColor c2 = COLOR_GRAPH_PRECIP_LOW2_NIGHT;
    //DitherPercentage d = COLOR_GRAPH_PRECIP_LOW_DITHER;
    
    if (precip_i > 0) {
      if (precip_p > 50) {
        if (is_day[i]) {
          c1 = COLOR_GRAPH_PRECIP_HIGH1_DAY;
          c2 = COLOR_GRAPH_PRECIP_HIGH2_DAY;
        } else {
          c1 = COLOR_GRAPH_PRECIP_HIGH1_NIGHT;
          c2 = COLOR_GRAPH_PRECIP_HIGH2_NIGHT;
        }
        //d = COLOR_GRAPH_PRECIP_HIGH_DITHER;
      } else {
        if (is_day[i]) {
          c1 = COLOR_GRAPH_PRECIP_LOW1_DAY;
          c2 = COLOR_GRAPH_PRECIP_LOW2_DAY;
        }
      }   
      //draw_random_dithered_rect(ctx, GRect(x, 100-precip_i, 4, precip_i), c1, c2, d);
      draw_random_gradient_rect(ctx, GRect(x, 101-precip_i, 4, precip_i), c1, c2, TOP_TO_BOTTOM);
    }
    
#ifdef PBL_PLATFORM_BASALT
    if (weatherState.forecast[i] < 0) {
      graphics_context_set_stroke_color(ctx, COLOR_GRAPH_TEMPERATURE_NEG);
    } else {
      graphics_context_set_stroke_color(ctx, COLOR_GRAPH_TEMPERATURE);
    }
    graphics_context_set_stroke_width(ctx, 3);
#endif
  
    // hőmérséklet görbe (egyenes szakasza)
    y2 = 101 - (weatherState.forecast[i]-min)*scale/100;    
    graphics_draw_line(ctx, GPoint(x, y), GPoint(x+4, y2));  
    
#ifdef PBL_PLATFORM_APLITE
    graphics_draw_line(ctx, GPoint(x, y+1), GPoint(x+4, y2+1));  
    graphics_draw_line(ctx, GPoint(x, y-1), GPoint(x+4, y2-1));  
#elif PBL_PLATFORM_BASALT
    graphics_context_set_stroke_width(ctx, 1);
    graphics_context_set_stroke_color(ctx, COLOR_GRAPH_TEMPERATURE_CENTER);
    graphics_draw_line(ctx, GPoint(x, y), GPoint(x+4, y2));
#endif    
  
    y = y2;
  }
#ifdef PBL_PLATFORM_BASALT
  graphics_context_set_stroke_width(ctx, 1);
#endif
  
  //text_layer_set_text(s_graph_time_layer, "11    15    19    23    03    07    11");
  static char xAxisText[] = "11    15    19    23    03    07    11            ";
  xsprintf(xAxisText, "%02d    %02d    %02d    %02d    %02d    %02d    %02d"
           , weatherState.forecast_first_time
           , (weatherState.forecast_first_time+ 4) % 24
           , (weatherState.forecast_first_time+ 8) % 24
           , (weatherState.forecast_first_time+12) % 24
           , (weatherState.forecast_first_time+16) % 24
           , (weatherState.forecast_first_time+20) % 24
           , weatherState.forecast_first_time
          );
  text_layer_set_text(s_graph_time_layer, xAxisText);
  
  //text_layer_set_text(s_graph_temp_layer, "30°\n\n20°\n\n10°\n\n0°\n\n-10°\n\n-20°"); 
  static char yAxisText[] = "30°\n\n20°\n\n10°\n\n0°\n\n-10°\n\n-20°           ";
  const uint8_t step = 2*max - 2*min;
  max--;
  min++;
  xsprintf(yAxisText, "%d°\n\n%d°\n\n%d°\n\n%d°\n\n%d°\n\n%d°", 
           max, (int8_t)((10*max-step)/10), (int8_t)((10*max-(2*step))/10), 
           (int8_t)((10*min+(2*step))/10), (int8_t)((10*min+step)/10), min);
  text_layer_set_text(s_graph_temp_layer, yAxisText);
  
  bitmap_layer_set_bitmap(s_weather_icon_layer, s_weather_icon_bitmap);
}


// A QuickView el fogja takarni a képernyő egy részét
static void prv_unobstructed_will_change(GRect final_unobstructed_screen_area, void *context) {
  // Get the full size of the screen
  GRect full_bounds = layer_get_bounds(s_window_layer);
  if (!grect_equal(&full_bounds, &final_unobstructed_screen_area)) {
    // Screen is about to become obstructed, hide the date
    
    layer_set_frame(text_layer_get_layer(s_time_layer), GRect(60, 10, 80, 35));
    text_layer_set_text_color(s_time_layer, COLOR_TIME_OBSTRUCTED);
    text_layer_set_text(s_time_layer, HMbuffer);
  }
}

static void prv_unobstructed_did_change(void *context) {
  // Get the full size of the screen
  GRect full_bounds = layer_get_bounds(s_window_layer);
  // Get the total available screen real-estate
  GRect bounds = layer_get_unobstructed_bounds(s_window_layer);
  if (grect_equal(&full_bounds, &bounds)) {
    // Screen is no longer obstructed, show the date

    //layer_set_bounds((Layer *)s_time_layer, GRect(60, 120, 104, 44));
    layer_set_frame(text_layer_get_layer(s_time_layer), GRect(60, 120, 80, 35));
    text_layer_set_text_color(s_time_layer, COLOR_TIME);
    text_layer_set_text(s_time_layer, HMbuffer);
  }
}


static void main_window_load(Window *window) {
  s_window_layer = window_get_root_layer(window); 
  GRect bounds = layer_get_frame(s_window_layer);
  time_t currTime = time(NULL); 
  struct tm *local_tick_time = localtime(&currTime);
  
  
  // Subscribe to the will_change and did_change events
  UnobstructedAreaHandlers handlers = {
    .will_change = prv_unobstructed_will_change,
    .did_change = prv_unobstructed_did_change
  };
  unobstructed_area_service_subscribe(handlers, NULL);
  
  
  s_small_font_8 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SMALL_8));
  s_showcard_font_30 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SHOWCARD_30));
   
  s_weather_icon_bitmap_day_rain = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_DAY_RAIN);  
  s_weather_icon_bitmap_day_sunny = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_SUNNY);  
  s_weather_icon_bitmap_night_rain = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_NIGHT_RAIN);  
  s_weather_icon_bitmap_night_clear = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_NIGHT_CLEAR); 
  
  s_weather_icon_bitmap = s_weather_icon_bitmap_day_sunny;
  //s_weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_SUNNY);  
  
  //wall_hour = local_tick_time->tm_hour;
  wall_min = local_tick_time->tm_min;
  //wall_sec = local_tick_time->tm_sec;
  
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(HMbuffer, sizeof("00:00"), "%H:%M", local_tick_time);
  } else {
    // Use 12 hour format
    strftime(HMbuffer, sizeof("00:00"), "%I:%M", local_tick_time);
  }
  

  batteryState = battery_state_service_peek();
  
  // Create battery layer
  s_charge_path_ptr = gpath_create(&CHARGE_PATH_INFO);
  gpath_move_to(s_charge_path_ptr, GPoint(
    bounds.origin.x + s_screen_padding, 
    bounds.origin.y + s_screen_padding + battery_icon_size.h + 2
  ));
  
  // Battery empty icon
  s_batt_empty_path_ptr = gpath_create(&BATT_EMPTY_PATH_INFO);
  gpath_move_to(s_batt_empty_path_ptr, GPoint(
    bounds.origin.x + s_screen_padding - 2, 
    bounds.origin.y + s_screen_padding + battery_icon_size.h/2 - (BATT_EMPTY_PATH_INFO_HEIGHT/2)
  ));
  
  // Time marker icon - show only if last weather info too old
  s_curr_time_marker_path_ptr = gpath_create(&CURR_TIME_MARKER_PATH_INFO);
  
  // Create temperature Layer
  s_weather_layer = text_layer_create(GRect(2, 0, bounds.size.w/4, 50));
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorBlack);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentLeft);
  text_layer_set_font(s_weather_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  //text_layer_set_text(s_weather_layer, "Loading...");
  layer_add_child(s_window_layer, text_layer_get_layer(s_weather_layer));
  
  // légnyomás  
  s_pressure_layer = text_layer_create(GRect(bounds.size.w - 50, bounds.size.h - 16, 50, 14));
  text_layer_set_text_alignment(s_pressure_layer, GTextAlignmentLeft);
  text_layer_set_background_color(s_pressure_layer, COLOR_BG_LIGHT);
  text_layer_set_font(s_pressure_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(s_pressure_layer, "--");
  layer_add_child(s_window_layer, text_layer_get_layer(s_pressure_layer));
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "pressure layer");
  
  // Hónap, nap
  s_month_day_layer = text_layer_create(GRect(5, bounds.size.h - 16, 60, 16));
  text_layer_set_text_alignment(s_month_day_layer, GTextAlignmentLeft);
  text_layer_set_background_color(s_month_day_layer, COLOR_BG_LIGHT);
  text_layer_set_font(s_month_day_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(s_month_day_layer, wall_month_day);
  layer_add_child(s_window_layer, text_layer_get_layer(s_month_day_layer));
  
  // a hét napja
  s_dow_layer = text_layer_create(GRect(44, bounds.size.h - 16, 50, 16));
  text_layer_set_text_alignment(s_dow_layer, GTextAlignmentLeft);
  text_layer_set_background_color(s_dow_layer, COLOR_BG_LIGHT);
  text_layer_set_font(s_dow_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  //text_layer_set_font(s_dow_layer, s_small_font_8);
  text_layer_set_text(s_dow_layer, wall_dow);
  layer_add_child(s_window_layer, text_layer_get_layer(s_dow_layer));

  // BLUETOOTH
  s_my_path_ptr_bt = gpath_create(&BT_PATH_INFO);
  gpath_move_to(s_my_path_ptr_bt, GPoint(bounds.origin.x + s_screen_padding, bounds.origin.y + s_screen_padding + battery_icon_size.h + 24));
  
  // MINDEN KIRAJZOLÁS EGYETLEN RÉTEGBEN
  s_draw_layer = layer_create(bounds);
  layer_set_update_proc(s_draw_layer, draw_layer_update_proc);
  layer_add_child(s_window_layer, s_draw_layer);
  
  // AKKU ÁLLAPOT
  s_battery_layer = layer_create(bounds);
  layer_set_update_proc(s_battery_layer, battery_layer_update_proc);
  layer_add_child(s_window_layer, s_battery_layer);

  // GRAPH AXIS TEXT
  // Time axis
  s_graph_time_layer = text_layer_create(GRect(33, 106, 104, 10));
  text_layer_set_text_alignment(s_graph_time_layer, GTextAlignmentLeft);
  text_layer_set_text(s_graph_time_layer, "12    16    20    24    04    08    12");
  text_layer_set_colors(s_graph_time_layer, GColorWhite, GColorClear);
  text_layer_set_font(s_graph_time_layer, s_small_font_8);
  layer_add_child(s_draw_layer, text_layer_get_layer(s_graph_time_layer));
  
  // Temperature axis
  s_graph_temp_layer = text_layer_create(GRect(13, 10, 20, 100));
  text_layer_set_text_alignment(s_graph_temp_layer, GTextAlignmentRight);
  text_layer_set_text(s_graph_temp_layer, "30°\n\n20°\n\n10°\n\n0°\n\n-10°\n\n-20°"); 
  text_layer_set_colors(s_graph_temp_layer, GColorWhite, GColorClear);
//  s_custom_font_8 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SMALL_8));
  text_layer_set_font(s_graph_temp_layer, s_small_font_8);
  layer_add_child(s_draw_layer, text_layer_get_layer(s_graph_temp_layer));
  
  // Humidity icon
  s_humidity_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DROP);
  s_humidity_icon_layer = bitmap_layer_create(GRect(bounds.origin.x + s_screen_padding, bounds.size.h/2, 10, 16));
  bitmap_layer_set_alignment(s_humidity_icon_layer, GAlignCenter);
  bitmap_layer_set_bitmap(s_humidity_icon_layer, s_humidity_icon_bitmap);
  layer_add_child(s_draw_layer, bitmap_layer_get_layer(s_humidity_icon_layer));
  
  // Humidity value
  s_humidity_text_layer = text_layer_create(GRect(bounds.origin.x, bounds.size.h/2 + 18, 20+s_screen_padding, 10));
  text_layer_set_text_alignment(s_humidity_text_layer, GTextAlignmentCenter);
  text_layer_set_text(s_humidity_text_layer, "60%"); 
  text_layer_set_colors(s_humidity_text_layer, GColorWhite, GColorClear);
//  s_custom_font_8 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SMALL_8));
  text_layer_set_font(s_humidity_text_layer, s_small_font_8);
  layer_add_child(s_draw_layer, text_layer_get_layer(s_humidity_text_layer));
  
    // időjárás ikonja
  s_weather_icon_layer = bitmap_layer_create(GRect(5, bounds.size.h - 55, 40, 40));
  bitmap_layer_set_alignment(s_weather_icon_layer, GAlignCenter);
#ifdef PBL_PLATFORM_APLITE
  bitmap_layer_set_compositing_mode(s_weather_icon_layer, GCompOpAssign);
#elif PBL_PLATFORM_BASALT
  bitmap_layer_set_compositing_mode(s_weather_icon_layer, GCompOpSet);
#endif
  layer_add_child(s_draw_layer, bitmap_layer_get_layer(s_weather_icon_layer));
  
  // Create digital time TextLayer
  s_time_layer = text_layer_create(GRect(60, 120, 94, 40));
  //layer_set_update_proc(text_layer_get_layer(s_time_layer), time_layer_update_proc);
  
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
  text_layer_set_font(s_time_layer, s_showcard_font_30);
  //text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text(s_time_layer, HMbuffer);
  text_layer_set_colors(s_time_layer, COLOR_TIME, GColorClear);
  
  layer_add_child(s_window_layer, text_layer_get_layer(s_time_layer));  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "The time is %s (%li)", HMbuffer, currTime);

  
  // ADATOK VISSZATÖLTÉSE PERZISZTENS TÁROLÓBÓL
  if (persist_exists(PERSIST_KEY_WEATHER)) {
    persist_read_data(PERSIST_KEY_WEATHER, &weatherState, sizeof(weatherState));
    
    struct tm *pers_local_tick_time = localtime(&weatherState.time);
    char persTimeBuffer[] = "00:00";
    strftime(persTimeBuffer, sizeof("00:00"), "%H:%M", pers_local_tick_time);
    
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Persistent: temp = %d°C, cond = %s, icon = %s AT %s (%li)", 
            weatherState.temperature, weatherState.conditions, weatherState.icon, persTimeBuffer, weatherState.time);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "            sunrise = %i:%i, sunset = %i:%i", 
            weatherState.sunrise_hour, weatherState.sunrise_min, 
            weatherState.sunset_hour, weatherState.sunset_min);
    showCurrentWeather(weatherState);
    if (weatherState.time < currTime-CONFIG_WEATHER_REFRESH_EVERY*60) {
      // túl régi időjárás adat -> reload
      loadWeather();
    }
  } else {
    // nem volt elmentve időjárás adat -> load
    APP_LOG(APP_LOG_LEVEL_DEBUG, "No persistent weater data saved, load weather");
    loadWeather();
  }
  
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Heap Available: %d", heap_bytes_free()); 
  app_log(APP_LOG_LEVEL_DEBUG, "wf", 682, "U:%zd", heap_bytes_used());
  app_log(APP_LOG_LEVEL_DEBUG, "wf", 682, "A:%zd", heap_bytes_free());
  //loadWeather();
}

char *translate_error(AppMessageResult result) {
  switch (result) {
    case APP_MSG_OK: return "APP_MSG_OK";
    case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
    case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
    case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
    case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
    case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
    case APP_MSG_BUSY: return "APP_MSG_BUSY";
    case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
    case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
    case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
    case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
    case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
    case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
    case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
    default: return "UNKNOWN ERROR";
  }
}

void setForecast(uint8_t *data, uint8_t length) {
  for(uint8_t i=0; i<24; i++) {
    weatherState.forecast[i] = data[i] >= 128 ? data[i]-255 : data[i];
  } 
}

void setForecastPrecipIntensity(uint8_t *data, uint8_t length) {
  for(uint8_t i=0; i<24; i++) {
    weatherState.forecast_precip_intensity[i] = data[i];
  } 
}

void setForecastPrecipProbability(uint8_t *data, uint8_t length) {
  for(uint8_t i=0; i<24; i++) {
    weatherState.forecast_precip_probability[i] = data[i];
  } 
}

void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "JS callback");
  
  bool weatherDataReceived = false;
  weather_refresh_retry_count = 0;
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Pressure: %d ", p);
  
  // Read first item
  Tuple *t = dict_read_first(iterator);
 
  // For all items
  while(t != NULL) {
    // Which key was received?
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "Key %d received!", (int)t->key);
    switch(t->key) {
      case KEY_READY:
        jsReady = true;
        break;
      case KEY_TEMPERATURE:
        //snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°C", (int)t->value->int32);
        weatherState.temperature = (int)t->value->int32;
        weatherDataReceived = true;
        break;
      case KEY_CONDITIONS:
        snprintf(weatherState.conditions, sizeof(weatherState.conditions), "%s", t->value->cstring);
        weatherDataReceived = true;
        break;
      case KEY_ICON:
        snprintf(weatherState.icon, sizeof(weatherState.icon), "%s", t->value->cstring);
        weatherDataReceived = true;
        break;
      case KEY_SUNRISE_HOUR:
        weatherState.sunrise_hour = (uint8_t)t->value->int32;
        weatherDataReceived = true;
        break;
      case KEY_SUNRISE_MIN:
        weatherState.sunrise_min = (uint8_t)t->value->int32;
        weatherDataReceived = true;
        break;
      case KEY_SUNSET_HOUR:
        weatherState.sunset_hour = (uint8_t)t->value->int32;
        weatherDataReceived = true;
        break;
      case KEY_SUNSET_MIN:
        weatherState.sunset_min = (uint8_t)t->value->int32;
        weatherDataReceived = true;
        break;
      case KEY_WIND_SPEED:
        weatherState.wind_speed = (int)t->value->int32;
        weatherDataReceived = true;
        break;
      case KEY_WIND_DIRECTION:
        weatherState.wind_direction = (int)t->value->int32;
        weatherDataReceived = true;
        break;
      case KEY_PRESSURE:
        weatherState.pressure = (int)t->value->int32;
        weatherDataReceived = true;
        break;
      case KEY_HUMIDITY:
        weatherState.humidity = (int)t->value->int32;
        weatherDataReceived = true;
        break;
      case KEY_OZONE:
        weatherState.ozone = (int)t->value->int32;
        weatherDataReceived = true;
        break;
      case KEY_FORECAST:
        //weatherState.forecast = (uint8_t[24])*t->value->data; // uint8_t (*fb_matrix)[WINDOW_WIDTH]
        setForecast(t->value->data, t->length);
        weatherDataReceived = true;
        break;
      case KEY_FORECAST_PRECIP_INTENSITY:
        //weatherState.forecast = (uint8_t[24])*t->value->data; // uint8_t (*fb_matrix)[WINDOW_WIDTH]
        setForecastPrecipIntensity(t->value->data, t->length);
        weatherDataReceived = true;
        break;
      case KEY_FORECAST_PRECIP_PROBABILITY:
        //weatherState.forecast = (uint8_t[24])*t->value->data; // uint8_t (*fb_matrix)[WINDOW_WIDTH]
        setForecastPrecipProbability(t->value->data, t->length);
        weatherDataReceived = true;
        break;
      case KEY_FORECAST_FIRST_TIME:
        weatherState.forecast_first_time = (time_t)t->value->int32;
        weatherDataReceived = true;
        break;
      default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
        break;
    }
 
    // Look for next item
    t = dict_read_next(iterator);
  }
 
  if (weatherDataReceived) {    
    weatherState.time = time(NULL);
  }

  showCurrentWeather(weatherState);
}

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox failed! Reason given: %s", translate_error(reason));
}
 
void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Dropped message! Reason given: %s", translate_error(reason));
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void main_window_unload(Window *window) {
  // save weather state & show until connection
  persist_write_data(PERSIST_KEY_WEATHER, &weatherState, sizeof(weatherState));
  
  bitmap_layer_destroy_safe(s_image_layer);
  //gbitmap_destroy(s_image_bitmap);
  
  text_layer_destroy_safe(s_time_layer);
  text_layer_destroy_safe(s_graph_time_layer);
  text_layer_destroy_safe(s_weather_layer);
  text_layer_destroy_safe(s_dow_layer);
  gbitmap_destroy(s_humidity_icon_bitmap);
  
  bitmap_layer_destroy_safe(s_humidity_icon_layer);
  
  bitmap_layer_destroy_safe(s_weather_icon_layer);
  
  // NEVER DESTROY THIS!!!!
  // it contains only the reference of the other weather icons
  //gbitmap_destroy(s_weather_icon_bitmap);
  
  gbitmap_destroy(s_weather_icon_bitmap_day_sunny);
  gbitmap_destroy(s_weather_icon_bitmap_day_rain);
  gbitmap_destroy(s_weather_icon_bitmap_night_clear);
  gbitmap_destroy(s_weather_icon_bitmap_night_rain);
  
  fonts_unload_custom_font(s_small_font_8);
  fonts_unload_custom_font(s_showcard_font_30);
  
  gpath_destroy(s_charge_path_ptr);
  gpath_destroy(s_my_path_ptr_bt);
  
  layer_destroy_safe(s_draw_layer);
  
  app_message_deregister_callbacks();
}

static void init() {
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });

  window_set_background_color(s_main_window, COLOR_BG_LIGHT);
  window_stack_push(s_main_window, true);

  
  
  // creating the smaller window for the frequent updates.
//  s_second_window = window_create();
//  window_set_background_color(s_second_window, GColorWhite);
//  Layer *window_second_layer = window_get_root_layer(s_second_window);
//  layer_set_update_proc(window_second_layer, seconds_layer_update_proc);
  
  // Subscribe to Services
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_state_handler);
  //bluetooth_connection_service_subscribe(bluetooth_state_handler);

}

static void deinit() {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  //bluetooth_connection_service_unsubscribe();
  //accel_tap_service_unsubscribe();
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
