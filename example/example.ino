#include "PCF2129.h"

const int pin = 5;
PCF2129 rtc(pin);

void setup()
{
    Serial.begin(115200);
    rtc.setup();
    rtc.start();
}

void loop()
{
}
