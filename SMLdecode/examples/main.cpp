#include <Arduino.h>
#include "SMLdecode.h"
SMLdecode sml;

#define SML_TIBBER 1
void setup() 
{
    Serial.begin(115200);
    sml.init("192.168.2.88","admin","xxx-xxxx");
}

void loop()
{
    delay(2000);
    sml.read();
    Serial.printf("SML-Watt:%f", sml.getWatt());
    Serial.printf("SML-kWh :%f", sml.getInputkWh());
}