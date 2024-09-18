
// uncomment either USE_JOYSTICK or USE_BUTTONS
#define USE_JOYSTICK
// #define USE_BUTTONS

#include <ESP32_easy_wifi_data.h>

#ifdef USE_JOYSTICK
#include "OneAnalogJoystick.h"
OneAnalogJoystick stick = OneAnalogJoystick(A0, D7, D6, D5, D0);
#endif
/*
 * connect buttons between pins and ground
 * D7 - right
 * D6 - left
 * D5 - forwards
 * D0 - backwards
 */

// x is forwards, rz is counter clockwise https://www.ros.org/reps/rep-0103.html
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
    EWD::routerName = "rccar001"; // must match car's network (change to pair this controller with a different car)
    EWD::routerPassword = "password001"; // must match car's password
    EWD::routerPort = 25001;
    EWD::communicateWithIP = "192.168.4.1";
    EWD::signalLossTimeout = 200;
    EWD::resendTimeout = 60;
}

void setup()
{
    Serial.begin(115200);
    configWifi();
    EWD::setupWifi(WifiDataToParse, WifiDataToSend);

    pinMode(D3, OUTPUT); // blue (connected)
    pinMode(D1, OUTPUT); // middle red (not connected)
    pinMode(D2, OUTPUT); // bottom red (robot battery low)

#ifdef USE_BUTTONS
    pinMode(D7, INPUT_PULLUP);
    pinMode(D6, INPUT_PULLUP);
    pinMode(D5, INPUT_PULLUP);
    pinMode(D0, INPUT_PULLUP);
#endif
}

void loop()
{
#ifdef USE_JOYSTICK
    x = InputReader_JoystickAxis(stick.readY(), 43, 495, 927, 50); // backwards, center, forwards, deadzone
    rz = InputReader_JoystickAxis(stick.readX(), 927, 495, 43, 50); // right, center, left, deadzone
    // Serial.print(x); // forwards
    // Serial.print(",");
    // Serial.println(rz); // left
#endif
#ifdef USE_BUTTONS
    x = 0;
    rz = 0;
    if (digitalRead(D7) == LOW) { // turn right
        rz = -1;
    }
    if (digitalRead(D6) == LOW) { // turn left
        rz = 1;
    }
    if (digitalRead(D5) == LOW) { // move forwards
        x = 1;
    }
    if (digitalRead(D0) == LOW) { // move backwards
        x = -1;
    }
#endif

    EWD::runWifiCommunication();
    // FOR LEDS LOW IS ON
    if (EWD::notTimedOut()) {
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

    delay(10); // shorter delays bork the wifi

}

/**
 * This function reads an axis of a joystick and scales the input (use two for a two axis joystick).
 * This function was copied from https://github.com/gobabygocarswithjoysticks/car-code/blob/a6707ec557aca6ef06c269f16b703a64f75385cd/gbg_program/_Input_Readers.ino#L2-L22 written by gobabygocarswithjoysticks, MIT License
 * @param  reading:
 * @param  neg: readings beyond this number will be mapped to -1.0
 * @param  cent: readings near this number will be mapped to 0.0
 * @param  pos: readings beyond this number will be mapped to 1.0
 * @param  deadzone: if abs(reading-cent) <= deadzone then result will be 0
 * @retval  a float between -1 and 1
 */
float InputReader_JoystickAxis(int reading, int neg, int cent, int pos, int deadzone)
{
    deadzone = max(deadzone, 0);
    ////////// scale joystick values from adjustible range to -1 to 1
    float output = 0;
    if (neg < pos) { // if the value for when the joystick is towards the left is less then the value for when the joystick is towards the right...
        if (reading < cent - deadzone && neg < cent - deadzone)
            output = floatMap(reading, neg, cent - deadzone, -1, 0);
        if (reading > cent + deadzone && pos > cent + deadzone)
            output = floatMap(reading, pos, cent + deadzone, 1, 0);
    }
    if (neg > pos) { // joystick axis is backwards
        if (reading > cent + deadzone && neg > cent + deadzone)
            output = floatMap(reading, neg, cent + deadzone, -1, 0);
        if (reading < cent - deadzone && pos < cent - deadzone)
            output = floatMap(reading, pos, cent - deadzone, 1, 0);
    }
    return constrain(output, -1, 1);
}
float floatMap(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
