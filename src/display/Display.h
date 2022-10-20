#ifndef Display_h
#define Display_h
#include "Arduino.h"
#include <motors/Motors.h>
#include "TouchScreen.h"
#include <control_knob/Knob.h>
#include <clock/Clock.h>

class Display
{
public:
    Display();
    void updateLCDTime(boolean firstrun);
    void updateLCDStatus();
    int width();
    int height();
    void reset();
    uint16_t readID();
    void setVoltage(float volt);
    void initialize(uint16_t identifier, uint16_t color, int rotation);
    TSPoint getPoint();
    void setNeedInits(boolean init);
    boolean getNeedsInits();
    float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);
    void showSplashScreen();
    void setAddrWindow(int x, int y, int width, int height);
    uint8_t Display::showBMP(char *nm, int x, int y);
    void SDbegin();

private:
    boolean _needsInit;
};
#endif