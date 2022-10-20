
#pragma once
#include <Arduino.h>
namespace Motors
{
    void setSliderDirection(int dir);
    int getSliderDirection();
    void setOldSdir(int dir);
    int getOldSdir();
    boolean isRunning();
    void setRunning(boolean running);
    void setSpeed(int speed);
    int getSpeed();
    void setDecimals(int decimals);
    int getDecimals();
    void begin();
}
