#include <ESP32_easy_wifi_data.h>

#include "OneAnalogJoystick.h"

OneAnalogJoystick stick = OneAnalogJoystick(A0, D0, D5, D6, D7);

float x = 0;
float rz = 0;

float batteryVoltage = 0;

// data receive callback
void WifiDataToParse()
{
    batteryVoltage = EWD::recvFl();
}

// data send callback
void WifiDataToSend()
{
    EWD::sendBl(true); // enable
    EWD::sendFl(x);
    EWD::sendFl(rz);
}

void configWifi()
{
    EWD::mode = EWD::Mode::createAP;
    EWD::APPort = 25001;
    EWD::APName = "rccar001";
    EWD::APPassword = "password001";
}

void setup()
{
    Serial.begin(115200);
    configWifi();
    EWD::setupWifi(WifiDataToParse, WifiDataToSend);
}

void loop()
{
    x = stick.readY();
    rz = stick.readX();
    EWD::runWifiCommunication();
    if (EWD::newData()) {
        Serial.println(batteryVoltage);
    }
    delay(10); // shorter delays bork the wifi
}
