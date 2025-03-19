#include "LuxModbusSHI.h"

//----------------------------------------------------public Functions----------------------------------------------------

bool LuxModbusSHI::init(const char* sIP)
{
  _ipInitok = _ipLux.fromString(sIP);
  setTimeout(1000);
  debug_printf("[SHI] LuxModbusSHI-IP: %s\r\n",sIP);
  return _ipInitok;
}


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

/// @brief  read 
void LuxModbusSHI::readValues()
{
  _temp_outdoor = readInput(SHI_MODBUS_INPUT::SHI_TEMP_OUTDOOR);
  //_power_out    = readInput(SHI_MODBUS_INPUT::SHI_PWR_HEAT_OUT);
  //_power_in     = readInput(SHI_MODBUS_INPUT::SHI_PWR_ELECT_IN);
  debug_println();
  //_heat_mode   = readRegister(SHI_MODBUS_REGISTER::SHI_HEAT_MODE);
  //_heat_offset = readRegister(SHI_MODBUS_REGISTER::SHI_HEAT_OFFSET);
  debug_println();
  //_pc_mode     = readRegister(SHI_MODBUS_REGISTER::SHI_PC_MODE);
  //_pc_setpoint =readRegister(SHI_MODBUS_REGISTER::SHI_PC_SETPOINT);
  
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
    // client not connected, start the Modbus TCP client
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
    readValues();

    if (_bSet_heat_mode)
    {
      //setHeatOffset(_heat_offset, _heat_mode);
      _bSet_heat_mode = false;
    }
  }
  return connected();
}

bool LuxModbusSHI::writeHeatOffset(uint16_t tempx10, uint16_t mode)
{
  if (!_bSet_heat_mode)
  {
    return false;
  }
  _bSet_heat_mode = false;
  int iret = 0;
  iret = holdingRegisterWrite(SHI_MODBUS_REGISTER::SHI_HEAT_OFFSET, tempx10);
  iret = holdingRegisterWrite(SHI_MODBUS_REGISTER::SHI_HEAT_MODE, mode);
 
  debug_printf("setHeatOffset: val:%d  mode:%d\r\n", tempx10, mode);
  return bool(iret);
}

bool LuxModbusSHI::writePCSetpoint(uint16_t tempx10, uint16_t mode)
{
  if (!_bSet_pc_mode)
  {
    return false;
  }
  _bSet_pc_mode = false;
  int iret = 0;
  iret = holdingRegisterWrite(SHI_MODBUS_REGISTER::SHI_PC_SETPOINT, tempx10);
  iret = holdingRegisterRead(SHI_MODBUS_REGISTER::SHI_PC_MODE, mode);
  debug_printf("setPCSetpoint: val:%d  mode:%d\r\n", tempx10, mode);
 
  return bool(iret);
}






