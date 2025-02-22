#ifndef SML_DECODE_H
#define SML_DECODE_H

#include <Arduino.h>
#include <WiFi.h>                      // Built-in
#include <WiFiClient.h>
#include <HTTPClient.h>                // Build-in  

#ifdef SML_TASMOTA
#include "ArduinoJson.h"
#endif

#ifdef DEBUG_PRINT
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

class SMLdecode
{
  private:
    byte* httpGETRequest(const char* http_url);
    uint32_t decodeSMLval(byte * payload, byte* smlcode, uint smlsize,  uint offset);
    enum { SMLPAYLOADMAXSIZE = 300 }; 
    byte smlpayload[SMLPAYLOADMAXSIZE] {0}; 
   
  protected:
    int16_t _watt = 0;
    double _inputkWh   = 0.0;
    double _outputkWh  = 0.0;

    String _url = "";
    String _passw = "";
    String _usern = "";
    WiFiClient _wificlient;
  
  public:
    SMLdecode() {};
    ~SMLdecode () {};
   bool init(const char* url, const char* user, const char* pass);
   bool read();
   int16_t getWatt() {return _watt;};
   double getInputkWh() {return _inputkWh;};
   double getOutputkWh() {return _outputkWh;};
};
#endif