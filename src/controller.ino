#include <ESP32_easy_wifi_data.h>

#include "OneAnalogJoystick.h"

OneAnalogJoystick stick = OneAnalogJoystick(A0, D5, D0, D7, D6);

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
    EWD::mode = EWD::Mode::connectToNetwork;
    EWD::routerName = "rccar001";
    EWD::routerPassword = "password001";
    EWD::routerPort = 25001;
    EWD::communicateWithIP = "192.168.4.1";
}

void setup()
{
    Serial.begin(115200);
    configWifi();
    EWD::setupWifi(WifiDataToParse, WifiDataToSend);
}

void loop()
{
    x = constrain((stick.readY()-525)/450.0,-1,1);
    rz = constrain((stick.readX()-573)/450.0,-1,1);
    EWD::runWifiCommunication();
    if (EWD::newData()) {
        Serial.println(batteryVoltage);
    }
    delay(10); // shorter delays bork the wifi
}
