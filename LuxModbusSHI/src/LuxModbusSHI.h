#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <Client.h>
#include <AsyncWebLog.h>

#include "ModubusTCPClient/src/ModbusTCPClient.h"

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




class LuxModbusSHI: private ModbusTCPClient
{
  
  private:
    IPAddress  _ipLux;
    WiFiClient _wificlient;
    bool       _ipInitok = false;
   
  protected:
    bool     _bSet_heat_mode = false;
    uint16_t _heat_mode =  0;
    uint16_t _heat_setpoint  = 34;
    int16_t _heat_offset     = 0;

    bool     _bSet_pc_mode   = false;
    uint16_t _pc_mode        = 2;
    uint16_t _pc_setpoint    = 14;

    int16_t  _temp_outdoor   = 0;
    uint16_t _power_in       = 0;
    uint16_t _power_out      = 0;

   bool     writeHeatOffset(int16_t  varx10);
   bool     writePCSetpoint(uint16_t imode, int16_t  kwx10);

  public:
  
  enum SHI_MODBUS_REGISTER
  {
  SHI_HEAT_MODE     = 10000,
  SHI_HEAT_SETPOINT = 10001,
  SHI_HEAT_OFFSET   = 10002,

  SHI_PC_MODE       = 10040,
  SHI_PC_SETPOINT   = 10041
  };

  enum SHI_MODBUS_INPUT
  {
  SHI_TEMP_RL       = 10100,
  SHI_TEMP_RL_SOLL  = 10101,
  SHI_TEMP_VL       = 10105,
  SHI_TEMP_OUTDOOR  = 10108,
  SHI_PWR_HEAT_OUT  = 10300,
  SHI_PWR_ELECT_IN  = 10301
  };
  
  enum SHI_PCMODE
  {
    PC_OFF,
    PC_SMART,
    PC_HARD
  };

   LuxModbusSHI(Client& clients) : ModbusTCPClient(clients) { };
   LuxModbusSHI() : LuxModbusSHI(_wificlient){};
   
   bool init(const char* sIP);
   bool poll();

#ifdef DEBUG_PRINT
   int16_t readRegister(SHI_MODBUS_REGISTER reg);
   int16_t readInput(SHI_MODBUS_INPUT reg);
#endif
   virtual void readValues();

  
  void  setHeatOffset(int16_t val)
  {
   _heat_offset=val; _bSet_heat_mode=true;
   debug_printf("[SHI] setHeatOffset val:%d\r\n", val)
  };

  void  setPCSetpoint(int16_t val)
  {
     _pc_setpoint=val; _bSet_pc_mode=true;
    debug_printf("[SHI] setPCSetpoint val:%d\r\n", val)
  };

  void setPCMode(uint16_t val)
  {
    _pc_mode=val;
  }  

  int16_t getHeatMode()       { return _heat_mode;};
  int16_t getHeatOffsetX10()   {return _heat_offset;};

  int16_t getPCMode()       { return _pc_mode;};
  int16_t getPCSetpoint()   {return _pc_setpoint;};

  int16_t getTempOutdoorX10()  {return _temp_outdoor;};  
};

