#include "ESP32ntp.h"

/// @brief 
/// @return 
bool ESP32ntp::update()
{
  getLocalTime(&timeinfo);
  time(&now);
  unixTime = now;
  
  //See http://www.cplusplus.com/reference/ctime/strftime/
  //strftime(time_output, sizeof(time_output), "%H:%M", &timeinfo);  // Creates: '14:05'
  //Time_str = time_output;
  //strftime(time_output, sizeof(time_output), "%w", &timeinfo);     // Creates: '0' for Sun
  //DoW_str  = time_output;
  return true;
}

/// @brief 
/// @return 
bool ESP32ntp::begin(const char* timezone, const char* ntpserver1, const char* ntpserver2)
{
  // replaced with "configTzTime" see: https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/Time/SimpleTime/SimpleTime.ino
  /*
  configTime(0, 0, this->_ntpserver);                               // (gmtOffset_sec, daylightOffset_sec, ntpServer)
  setenv("TZ", this->_timezone, 1);                                 // setenv()adds "TZ" variable to the environment, only used if set to 1, 0 means no change
  tzset();
  */

  /**
   * NTP server address could be acquired via DHCP,
   *
   * NOTE: This call should be made BEFORE esp32 acquires IP address via DHCP,
   * otherwise SNTP option 42 would be rejected by default.
   * NOTE: configTime() function call if made AFTER DHCP-client run
   * will OVERRIDE acquired NTP server address
   */
  debug_printf("*** Start ESP32ntp\r\n");
  delay(500);
  //esp_sntp_servermode_dhcp(true);  // (optional oder Absturz wenn es nicht aufgerufen wird ??!??!)

   /**
   * A more convenient approach to handle TimeZones with daylightOffset
   * would be to specify a environment variable with TimeZone definition including daylight adjustmnet rules.
   * A list of rules for your zone could be obtained from https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
   */
  uint8_t cnt =0;
  configTzTime(timezone,ntpserver1, ntpserver2);
  // max 20sec sync time
  while ((sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED) && (cnt < 20))
  {
            debug_print(':');
            delay(200);
            cnt++;
  }

  if (getLocalTime(&timeinfo) == false)
  {
    debug_println("***ERROR: no NTP Time");
    delay(200);
    return false;
  }
  update();
  debug_printf("\r\n*** ESP32ntp Time:%s\r\n", getTimeString());
  return true;   
}


/// @brief See http://www.cplusplus.com/reference/ctime/strftime/
/// @return 
char* ESP32ntp::getTimeString()
{
  struct tm *now_tm = localtime(&unixTime);
  static char output[6];

  strftime(output, sizeof(output), "%H:%M", now_tm);               // Returns 21:12
  return output;
}

/// @brief See http://www.cplusplus.com/reference/ctime/strftime/
/// @return 

char* ESP32ntp::getDateString()
{
  struct tm *now_tm = localtime(&unixTime);
  static char output[40];
  
  strftime(output, sizeof(output), "%d.%m %G", now_tm);               // Returns 30.01.2024
  return output;  
}



/// @brief  get Weekday 0=sun 1=mon 2=tue 3=wen 4=thu 5=fri 6=sat
/// @return 
int ESP32ntp::getWeekday()
{
  struct tm *now_tm = localtime(&unixTime);
  return now_tm->tm_wday;
}

/// @brief  
/// @return timeinfo
struct tm* ESP32ntp::getTimeInfo()
{
  struct tm *now_tm = localtime(&unixTime);
  return now_tm;
}

/// @brief 
/// @return 
time_t ESP32ntp::getUnixTime()
{
  return unixTime;
}

/* wird das gebraucht ???
/// @brief 
/// @param eventIndex 
/// @param SGrMode 
/// @param start 
/// @param stop 
/// @return 
bool ESP32ntp::setSGrModeEvent(uint8_t eventIndex, uint8_t SGrMode, tm start, tm stop)
{
   if (eventIndex >= NumOfEvents)
   {return false;}
   TimerEvents[start.tm_wday][eventIndex].sgMode     = SGrMode;
   TimerEvents[start.tm_wday][eventIndex].Start_hour = start.tm_hour;
   TimerEvents[start.tm_wday][eventIndex].Start_min  = start.tm_min;
   TimerEvents[start.tm_wday][eventIndex].Stop_hour  = stop.tm_hour;
   TimerEvents[start.tm_wday][eventIndex].Stop_min   = stop.tm_min;

   return true;
}


 /// @brief 
 /// @param eventIndex 
 /// @return 
 uint8_t ESP32ntp::getSGrModeActual(uint8_t eventIndex)
 {
  time_t tm1 = unixTime;

  struct tm *now_tm = localtime(&tm1);

  eventsettings * aktEvent;
  uint8_t akt_day = now_tm->tm_wday;
  
  for (size_t i = 0; i < NumOfEvents; i++)
  {
    aktEvent = &TimerEvents[akt_day][i];
    if (aktEvent->Start_hour >= now_tm->tm_hour && 
        aktEvent->Start_min  >= now_tm->tm_min  &&
        aktEvent->Stop_hour  <= now_tm->tm_hour &&
        aktEvent->Stop_min   <= now_tm->tm_min  &&
        aktEvent->sgMode > 0)
    {
      return aktEvent->sgMode;
    }  
  }
  return 0;
 }
 */

