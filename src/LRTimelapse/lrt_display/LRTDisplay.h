#pragma once
#include <Arduino.h>

namespace LRTDisplay
{
    void printScreen();
    boolean getBacklight();
    void setBacklight(boolean open);
    void changeBackLightBrightness(char AMode);
    byte getAct_BackLightBrightness();
    void setAct_BackLightBrightness(byte data);
    void printRunningScreen();
    void printDoneScreen();
    void printSingleScreen();
    void updateTime();
    void setupFinished();

}
