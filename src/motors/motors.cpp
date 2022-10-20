#include <Arduino.h>
#include "Motors.h"
#include "configuration.h"
int _sliderDirection = 1;
int _oldSliderDirection = 0;
int _speed = 0;
int _decimals = 0;
boolean _isRunning = false;

namespace Motors
{
    void setSpeed(int speed)
    {
        _speed = speed;
    };
    int getSpeed()
    {
        return _speed;
    };
    void setDecimals(int decimals)
    {
        _decimals = decimals;
    };
    int getDecimals()
    {
        return _decimals;
    };
    boolean isRunning()
    {
        return _isRunning;
    };
    void setRunning(boolean running)
    {
        _isRunning = running;
    };

    void setSliderDirection(int dir)
    {
        _sliderDirection = dir;
    };
    int getSliderDirection()
    {
        return _sliderDirection;
    };
    void setOldSdir(int dir)
    {
        #ifdef DEBUG
        Serial.print("oldSsdir has been set to ....: ");
        Serial.println(dir);
        #endif
        _oldSliderDirection = dir;
    };
    int getOldSdir()
    {
        return _oldSliderDirection;
    };

}
