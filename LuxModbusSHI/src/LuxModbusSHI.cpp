#include "LuxModbusSHI.h"

//----------------------------------------------------public Functions----------------------------------------------------

bool LuxModbusSHI::init(const char* sIP)
{
  _ipInitok = _ipLux.fromString(sIP);
  setTimeout(1000);
  debug_printf("[SHI] LuxModbusSHI-IP: %s\r\n",sIP);
  return _ipInitok;
}

#ifdef DEBUG_PRINT
int16_t LuxModbusSHI::readRegister(SHI_MODBUS_REGISTER reg)
{
 int16_t ival =holdingRegisterRead(reg);
 debug_printf("[SHI] REG:%d, value:%d\r\n", reg, ival);  
 return ival;
}

int16_t LuxModbusSHI::readInput(SHI_MODBUS_INPUT reg)
{ 
 int16_t ival =inputRegisterRead(reg);
 debug_printf("[SHI] INP:%d, value:%d\r\n",reg, ival);  
 return ival;
}
#endif

/// @brief  read 
void LuxModbusSHI::readValues()
{
#ifdef DEBUG_PRINT
  _temp_outdoor = readInput(SHI_MODBUS_INPUT::SHI_TEMP_OUTDOOR);
  _power_out    = readInput(SHI_MODBUS_INPUT::SHI_PWR_HEAT_OUT);
  _power_in     = readInput(SHI_MODBUS_INPUT::SHI_PWR_ELECT_IN);
  //debug_println();
  _heat_mode   = readRegister(SHI_MODBUS_REGISTER::SHI_HEAT_MODE);
  _heat_offset = readRegister(SHI_MODBUS_REGISTER::SHI_HEAT_OFFSET);
  //debug_println();
  _pc_mode     = readRegister(SHI_MODBUS_REGISTER::SHI_PC_MODE);
  _pc_setpoint = readRegister(SHI_MODBUS_REGISTER::SHI_PC_SETPOINT);
#else
   _temp_outdoor = inputRegisterRead(SHI_MODBUS_INPUT::SHI_TEMP_OUTDOOR);
   _power_out    = inputRegisterRead(SHI_MODBUS_INPUT::SHI_PWR_HEAT_OUT);
   _power_in     = inputRegisterRead(SHI_MODBUS_INPUT::SHI_PWR_ELECT_IN);

   _heat_mode   = holdingRegisterRead(SHI_MODBUS_REGISTER::SHI_HEAT_MODE);
   _heat_offset = holdingRegisterRead(SHI_MODBUS_REGISTER::SHI_HEAT_OFFSET);

   _pc_mode     = holdingRegisterRead(SHI_MODBUS_REGISTER::SHI_PC_MODE);
   _pc_setpoint = holdingRegisterRead(SHI_MODBUS_REGISTER::SHI_PC_SETPOINT);
 #endif 
  
  // ... Add more function or define in derivied class
  //debug_println("Warning: no LuxModbusSHI.getvalues()  defined, derivate class");
}

/// @brief  integrate in main loop() with some timer (dont call more than one sec. per loop)
/// @return connected() status
bool LuxModbusSHI::poll()
{
  if (!connected() ) 
  {
    if (!_ipInitok)
    {
      debug_println("[SHI] LuxModbusSHI: call init() before loop()");
      return false;
    }
    // client not connected: start the Modbus TCP client
    debug_println("[SHI] try to connect to Modbus TCP server");

    if (!this->begin(_ipLux))
    {
      debug_println("[SHI] Modbus TCP Client failed to connect!");
      delay(2000);
    
    } else {
      debug_println("[SHI] Modbus TCP Client connected");
    }
  }
  else
  {
    // client connected: write and read values
    if (_bSet_heat_mode)
    {
      writeHeatOffset(_heat_offset);
      _bSet_heat_mode = false;
    }
    if (_bSet_pc_mode)
    {
      writePCSetpoint(SHI_PCMODE::PC_HARD, _pc_setpoint);
      _bSet_pc_mode = false;
    }

    readValues();

  }
  return connected();
}

bool LuxModbusSHI::writeHeatOffset(int16_t tempx10)
{
  if (!_bSet_heat_mode)
  {
    return false;
  }
  _bSet_heat_mode = false;
  int iret = 0;
  iret = holdingRegisterWrite(SHI_MODBUS_REGISTER::SHI_HEAT_OFFSET, tempx10);
  iret = holdingRegisterWrite(SHI_MODBUS_REGISTER::SHI_HEAT_MODE, 2);
 
  debug_printf("WriteHeatOffset: mode:%d val:%d\r\n", mode, tempx10);
  AsyncWebLog.printf("[SHI] WriteHeatOffset: mode:%d val:%d\r\n", 2, tempx10);
  return bool(iret);
}

bool LuxModbusSHI::writePCSetpoint(uint16_t imode,  int16_t iVal)
{
  _bSet_pc_mode = false;
  if (iVal < 8)
  {
    iVal = 10;
  };
  
  if (iVal >= 20)
  { imode = 0;}
  else
  {imode = 2;}

  int iret = holdingRegisterWrite(SHI_MODBUS_REGISTER::SHI_PC_SETPOINT, iVal);
  iret = holdingRegisterWrite(SHI_MODBUS_REGISTER::SHI_PC_MODE,     imode);
  
  debug_printf("WritePCSetpoint: val:%d\r\n", iVal);
  AsyncWebLog.printf("[SHI] WritePCOffset: mode:%d val:%d\r\n",imode, iVal);
  return bool(iret);
}






