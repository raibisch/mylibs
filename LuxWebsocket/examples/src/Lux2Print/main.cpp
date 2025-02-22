#include <Arduino.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <FS.h>
#include <WiFi.h>
#include "LuxWebsocket.h"
#include "ESP32ntp.h"

#include "cred.h"
/*
#define SSID "xxx"
#define SSID_PASSWORD "xxxxx"
#define LUX_IP "192.168.2.xx"
#define LUX_PASSWORD "999999"
*/

// Wandelt Source-Code Variablen-Nam varVALUE_i_ABL_minAmpere en in String um :-))
#define GETVARNAME(Variable) (#Variable)

    
#ifdef ESP32_RELAY_X2
#pragma message("Info : ESP32_RELAY_X2")
#define LED_GPIO 23
#define RELAY_1  16
#define RELAY_2  17
#endif

#ifdef ESP32_RELAY_X4
#pragma message("Info : ESP32_RELAY_X4")
#define LED_GPIO 23
#define RELAY_1  26
#define RELAY_2  25
#define RELAY_3  33
#define RELAY_4  32     
#endif

#ifdef ESP32_DEVKIT1
#pragma message("Info : ESP32_DEVKIT")
#define LED_GPIO 2
#endif

#ifdef ESP32_S2_MINI
#pragma message("Info : ESP32_S2_MINI")
#define LED_GPIO LED_BUILTIN    
#endif

#ifdef ESP32_S3_ZERO
 #pragma message("Info : ESP32_S3_ZERO")
 #define NEOPIXEL 21
#endif

#ifdef M5_COREINK
 #pragma message("Info : M5Stack CoreInk Module")
 #define LED_GPIO 10
#endif


// Luxtronic Webservice 
LuxWebsocket luxws;

// ntp client
const char* TimeServerLocal = "192.168.2.1";
const char* TimeServer      = "europe.pool.ntp.org";
const char* Timezone        = "CET-1CEST,M3.5.0,M10.5.0/3";       // Central Europe
ESP32ntp ntpclient(TimeServerLocal,Timezone);


////////////////////////////////////////////
/// @brief init builtin LED
////////////////////////////////////////////
inline void initLED()
{
 #ifndef ESP32_S3_ZERO
  pinMode(LED_GPIO, OUTPUT);
  digitalWrite(LED_GPIO, HIGH);
#else
  neopixel_color='w';
#endif
}

/// @brief  set builtin LED
/// @param i = HIGH / LOW
void setLED(uint8_t i)
{
#ifndef ESP32_S3_ZERO
//#ifndef M5_COREINK
 digitalWrite(LED_GPIO, i);
//#endif
#else
  if (i==0)
  {
    neopixelWrite(NEOPIXEL,0,0,0); // off
  }
  else
  {
    switch (neopixel_color)
    {
    case 'r': 
      neopixelWrite(NEOPIXEL,6,0,0); // red
      break;
    case 'g':
      neopixelWrite(NEOPIXEL,0,6,0); // green
      break;
    case 'b':
      neopixelWrite(NEOPIXEL,0,0,6); // blue
      break;
    case 'y':
       neopixelWrite(NEOPIXEL,4,2,0); // yellow
      break;
    case 'w':
      neopixelWrite(NEOPIXEL,2,2,2); // white
      break;
    default:
       break;
    }
  }
#endif
}

uint8_t blnk=0;
static void blinkLED()
{
  blnk = !blnk;
  setLED(blnk);
}


//////////////////////////////////////////
/// @brief Init Wifi
/////////////////////////////////////////
void initWiFi(const char* mode, const char* hostname, const char* ssid, const char* password)
{
  // Test mit AP !!!!!!!!!!!!!!!!!!!!!
  //varStore.varWIFI_s_Mode="AP";
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  // API Info: https://docs.espressif.com/projects/esp-idf/en/v4.4.6/esp32/api-reference/network/esp_wifi.html
#ifdef MINI_32
   WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG,0); // brownout problems
#endif
   if (String(mode) == "AP")
   {
    delay(100);
    Serial.println("INFO-WIFI:AP-Mode");
    WiFi.softAP(hostname);   
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP().toString());
   }
   else
   {
    Serial.println("INFO-WIFI:STA-Mode");
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(hostname);
    WiFi.begin(ssid, password);
    #if defined ESP32_S3_ZERO || defined MINI_32 || defined M5_COREINK
    WiFi.setTxPower(WIFI_POWER_7dBm);// brownout problems with some boards or low battery load for M5_COREINK
    //WiFi.setTxPower(WIFI_POWER_15dBm);// Test 15dB
    #endif
    #if defined DEBUG_PRINT && (defined ESP32_RELAY_X4 || defined ESP32_RELAY_X2)
    WiFi.setTxPower(WIFI_POWER_MINUS_1dBm); // decrease power over serial TTY-Adapter
    #endif
    int i = 0;
    delay(200);
    Serial.printf("SSID:%s connecting..\r\n", ssid);
    ///debug_printf("Passwort:%s\r\n", varStore.varWIFI_s_Password);
    while (!WiFi.isConnected())
    {
        Serial.print(".");
        blinkLED();
        i++;  
        delay(400);
        if (i > 20)
        {
          ESP.restart();
        }
    }
    if (!WiFi.isConnected())
    {
      ESP.restart();
    }
    delay(300);
    debug_println("CONNECTED!");
    debug_printf("WiFi-Power:%d\r\n",WiFi.getTxPower())
    debug_printf("WiFi-RSSI:%d\r\n",WiFi.RSSI());
    
    debug_print("IP Address: ");
    debug_println(WiFi.localIP().toString());
   }
  return;
}     



void setup()
{
    Serial.begin(115200);
    initWiFi("STA","LUXTEST",SSID, SSID_PASSWORD);
    delay(1000);
    initLED();
    setLED(1);
    if (!ntpclient.begin())
    {
      ESP.restart();
    }
    setLED(0);
    luxws.init(LUX_IP, LUX_PASSWORD);
    delay(1000);
}

#define TIMER_SLOW_DURATION 5000
u_long TimerSlow = 0;
void loop()
{
    if (millis() - TIMER_SLOW_DURATION > TimerSlow) 
    {
     TimerSlow = millis();                      // Reset time for next event
     ntpclient.update();
     time_t tt = ntpclient.getUnixTime();

     luxws.poll();
     if (luxws.isConnected())
     {
      Serial.printf("WS-POLL-State:       \t %s\r\n", luxws.getval(LUX_VAL_TYPE::LUX_STATUS_POLL));
      Serial.printf("WP-State:            \t %s\r\n", luxws.getval(LUX_VAL_TYPE::LUX_STATUS_HEATPUMP));
      Serial.printf("Leistung AUSGANG:    \t %0.2f \r\n", luxws.getval(LUX_VAL_TYPE::LUX_ENERGY_OUTSUM));
      Serial.printf("Luxtronik Aussentemp.\t %02.2f\r\n", luxws.getvalf(LUX_TEMP_OUT));
      Serial.printf("COP-SUM:             \t %02.2f\r\n", luxws.getvalf(LUX_VAL_TYPE::LUX_COP_SUM));
      Serial.printf("COP-DAY_HEIZ:        \t %02.2f\r\n", luxws.getvalf(LUX_VAL_TYPE::LUX_COP_DAY_HEAT));
      Serial.printf("RL SOLL - IST:       \t %02.2f\r\n", luxws.getvalf(LUX_VAL_TYPE::LUX_TEMP_RL_SOLL) - luxws.getvalf(LUX_VAL_TYPE::LUX_TEMP_RL_IST));
      Serial.printf("VL - RL:             \t %02.2f\r\n", luxws.getvalf(LUX_VAL_TYPE::LUX_TEMP_VL_IST) - luxws.getvalf(LUX_VAL_TYPE::LUX_TEMP_RL_IST));
     }
     else
     {
       Serial.printf("...wait for Luxtronik connect\r\n");
     }
     Serial.printf("Time: %s\r\n", ntpclient.getTimeString());
     blinkLED();
    }
  luxws.loop();
}
