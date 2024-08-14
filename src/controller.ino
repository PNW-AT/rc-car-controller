#include <ESP32_easy_wifi_data.h>

#include "OneAnalogJoystick.h"

OneAnalogJoystick stick = OneAnalogJoystick(A0, D7, D6, D5, D0);

float x = 0;
float rz = 0;

float batteryVoltage = 5;

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
    EWD::signalLossTimeout = 200;
    EWD::resendTimeout = 50;
}

void setup()
{
    Serial.begin(115200);
    configWifi();
    EWD::setupWifi(WifiDataToParse, WifiDataToSend);

    pinMode(D3, OUTPUT); // blue (connected)
    pinMode(D1, OUTPUT); // middle red (not connected)
    pinMode(D2, OUTPUT); // bottom red (robot battery low)
}

void loop()
{
    x = constrain((stick.readY() - 475) / 425.0, -1, 1);
    rz = constrain((stick.readX() - 601) / 400.0, -1, 1);

    // print x, rz to serial
    // Serial.print(x);
    // Serial.print(" ");
    // Serial.println(rz);

    EWD::runWifiCommunication();
    // FOR LEDS LOW IS ON
    if (EWD::getWifiConnected()) {
        digitalWrite(D3, LOW);
        digitalWrite(D1, HIGH);
    } else {
        digitalWrite(D3, HIGH);
        digitalWrite(D1, LOW);
    }

    if (batteryVoltage >= 3.55) {
        digitalWrite(D2, HIGH);
    } else { // battery low
        digitalWrite(D2, LOW);
    }

    // if (EWD::newData()) {
    //     Serial.println(batteryVoltage);
    // }
    delay(10); // shorter delays bork the wifi
}
