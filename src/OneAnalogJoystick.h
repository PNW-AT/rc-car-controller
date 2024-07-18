#ifndef ONE_ANALOG_JOYSTICK_H
#define ONE_ANALOG_JOYSTICK_H
#include <Arduino.h>
class OneAnalogJoystick {
protected:
    byte pinA;
    byte pinXM;
    byte pinXP;
    byte pinYM;
    byte pinYP;

public:
    OneAnalogJoystick(byte _pinA, byte _pinXM, byte _pinXP, byte _pinYM, byte _pinYP)
    {
        pinA = _pinA;
        pinXM = _pinXM;
        pinXP = _pinXP;
        pinYM = _pinYM;
        pinYP = _pinYP;
    }
    /**
     * @brief
     * @note   no one-time setup is needed, this function can just be called when needed
     * @retval
     */
    int readX()
    {
        digitalWrite(pinYP, LOW);
        pinMode(pinYP, INPUT);
        digitalWrite(pinYM, LOW);
        pinMode(pinYM, INPUT);
        pinMode(pinXM, OUTPUT);
        digitalWrite(pinXM, LOW);
        pinMode(pinXP, OUTPUT);
        digitalWrite(pinXP, HIGH);
        return analogRead(pinA);
    }
    /**
     * @brief
     * @note   no one-time setup is needed, this function can just be called when needed
     * @retval
     */
    int readY()
    {
        digitalWrite(pinXP, LOW);
        pinMode(pinXP, INPUT);
        digitalWrite(pinXM, LOW);
        pinMode(pinXM, INPUT);
        pinMode(pinYM, OUTPUT);
        digitalWrite(pinYM, LOW);
        pinMode(pinYP, OUTPUT);
        digitalWrite(pinYP, HIGH);
        return analogRead(pinA);
    }
};
#endif // ONE_ANALOG_JOYSTICK_H
