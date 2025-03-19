/* raibisch (c) 2025
Example for read and write Luxtronik SHI (Smart-Home-Interface) with Modbus TCP
*/

#include <Arduino.h>
#include <WiFi.h>
#include "LuxModbusSHI.h"


#define MY_SSID  "myssid"
#define MY_PW    "mypassword"
//#include "cred.h"

/*
// override LuxModbusSHI to modify readvalues()
class LuxSHI: public LuxModbusSHI
{
  public:
  /// @brief read Values from SHI (Holding-Register or Input-Register)
  void readValues() final
  {
    readInput(SHI_MODBUS_INPUT::TEMP_OUTDOOR);
    readRegister(SHI_MODBUS_REGISTER::PC_SETPOINT);
  } 
};
LuxSHI luxshi;
*/

LuxModbusSHI luxshi;

void setup() {
  Serial.begin(115200);
  WiFi.begin(MY_SSID, MY_PW); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("WiFi connected!, IP: %s\r\n", WiFi.localIP().toString().c_str());
  luxshi.init("192.168.2.101");
  delay(500);
  luxshi.poll();
  luxshi.setHeatOffset(5,2);
}

void loop() 
{
  luxshi.poll();
  delay(2000);
}


