#include "LuxModbusSHI.h"

//----------------------------------------------------public Functions----------------------------------------------------

bool LuxModbusSHI::init(const char* sIP)
{
  _ipInitok = _ipLux.fromString(sIP);
  setTimeout(1000);
  debug_printf("LuxModbusSHI-IP: %s\r\n",sIP);
  return _ipInitok;
}


int LuxModbusSHI::readRegister(SHI_MODBUS_REGISTER reg)
{
 int ival =holdingRegisterRead(reg);
 debug_printf("REG:%d, value:%d\r\n", reg, ival);  
 return ival;
}

int LuxModbusSHI::readInput(SHI_MODBUS_INPUT reg)
{ 
 int ival =inputRegisterRead(reg);
 debug_printf("INP:%d, value:%d\r\n",reg, ival);  
 return ival;
}

/// @brief  read 
void LuxModbusSHI::readValues()
{
  readInput(SHI_MODBUS_INPUT::TEMP_OUTDOOR);

  readRegister(SHI_MODBUS_REGISTER::HEAT_MODE);
  readRegister(SHI_MODBUS_REGISTER::HEAT_OFFSET);
  //readRegister(SHI_MODBUS_REGISTER::HEAT_SETPOINT);
  //readRegister(SHI_MODBUS_REGISTER::PC_MODE);
  //readRegister(SHI_MODBUS_REGISTER::PC_SETPOINT);
  
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
      debug_println("LuxModbusSHI: call init() before loop()");
      return false;
    }
    // client not connected, start the Modbus TCP client
    debug_println("try to connect to Modbus TCP server");

    if (!this->begin(_ipLux))
    {
      debug_println("Modbus TCP Client failed to connect!");
      delay(2000);
    
    } else {
      debug_println("Modbus TCP Client connected");
    
    }
  }
  else
  {
    readValues();
  }
  return connected();
}


bool LuxModbusSHI::setHeatOffset(uint16_t tempx10, uint16_t mode)
{
  int iret = 0;
  iret = this->holdingRegisterWrite(SHI_MODBUS_REGISTER::HEAT_OFFSET, tempx10);
  iret = this->holdingRegisterWrite(SHI_MODBUS_REGISTER::HEAT_MODE, uint16_t(0));
 
  debug_printf("setHeatOffset: val:%d  mode:%d\r\n", tempx10, mode);
  return iret;
}

bool LuxModbusSHI::setPCSetpoint(uint16_t tempx10, uint16_t mode)
{
  int iret = 0;
  iret = this->holdingRegisterWrite(SHI_MODBUS_REGISTER::PC_SETPOINT, tempx10);
  if (iret > 0)
  {
   iret = this->holdingRegisterRead(SHI_MODBUS_REGISTER::PC_MODE, 2);
  }
  debug_printf("setPCSetpoint: val:%d  mode:%d\r\n", tempx10, mode);
 
  return iret;
}






