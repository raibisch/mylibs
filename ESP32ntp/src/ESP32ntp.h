#pragma once 

#include <Arduino.h>
#include <time.h>
#include <esp_sntp.h>


/*
Die Bibliothek stellt verschiedenste Funktionen zum Abfragen und Manipulieren von Zeit und Datumswerten zur Verfügung.
*/

// now set in platformio.ini
//#define DEBUG_PRINT 1    // SET TO 0 OUT TO REMOVE TRACES
#ifdef DEBUG_PRINT
#pragma message("Info : DEBUG_PRINT=1")
#define debug_begin(...) Serial.begin(__VA_ARGS__);
#define debug_print(...) Serial.print(__VA_ARGS__);
#define debug_write(...) Serial.write(__VA_ARGS__);
#define debug_println(...) Serial.println(__VA_ARGS__);
#define debug_printf(...) Serial.printf(__VA_ARGS__);
#else
#define debug_begin(...)
#define debug_print(...)
#define debug_printf(...)
#define debug_write(...)
#define debug_println(...)
#endif
    

class ESP32ntp
{
  private:
    struct tm timeinfo;
    time_t now;
    time_t unixTime = 0;

 public: 
    ESP32ntp(){};
   ~ESP32ntp() {};
    bool begin(const char* timezone, const char* ntpserver1, const char* ntpserver2 = NULL);
    char* getTimeString();
    char* getDateString();
    int getWeekday();
    bool update(); 
    time_t getUnixTime();
    struct tm* getTimeInfo();
};
