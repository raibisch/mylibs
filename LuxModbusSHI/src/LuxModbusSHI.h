#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <Client.h>

#include "ModubusTCPClient/src/ModbusTCPClient.h"

#define DEBUG_PRINT 1

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

enum SHI_MODBUS_REGISTER
{
  HEAT_MODE     = 10000,
  HEAT_SETPOINT = 10001,
  HEAT_OFFSET   = 10002,

  PC_MODE       = 10040,
  PC_SETPOINT   = 10041
};

enum SHI_MODBUS_INPUT
{
  TEMP_RL       = 10100,
  TEMP_RL_SOLL  = 10101,
  TEMP_VL       = 10105,
  TEMP_OUTDOOR  = 10108
};

class LuxModbusSHI: private ModbusTCPClient
{
  private:
    IPAddress  _ipLux;
    WiFiClient _wificlient;
    bool       _ipInitok = false;
   
  protected:
    int _heat_setpoint  = 350;
    int _heat_offset    = 0;
    int _pc_setpoint    = 300;
    int _temp_outdoor   = 0;


  public:
   LuxModbusSHI(Client& clients) : ModbusTCPClient(clients) { _ipLux.clear();};
   LuxModbusSHI() : LuxModbusSHI(_wificlient){};
   
   bool init(const char* sIP);
   bool poll();

   int readRegister(SHI_MODBUS_REGISTER reg);
   int readInput(SHI_MODBUS_INPUT reg);
   virtual void readValues();
   
   int getHeatSetpointX10() {return _heat_setpoint;};
   int getHeatOffsetX10()   {return _heat_offset;};
   int getTempOutdoorX10()  {return _temp_outdoor;};
   int getPCSetpointX10()   {return _pc_setpoint;};

   //bool     setHeatSetpoint(uint16_t tempx10);
   bool     setHeatOffset(uint16_t tempx10, uint16_t mode);
   bool     setPCSetpoint(uint16_t kwx10, uint16_t mode);
};

