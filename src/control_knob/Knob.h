
#pragma once
#include <Arduino.h>
#include <Encoder.h>
namespace Knob
{

    int read();
    void setoldPosition(int position);
    int getoldPosition();
    void setButton(boolean active);
    boolean getButton();
    void setOffset(long offset);
    long getOffset();
    int readCLK();
    int readDT();
    void setCurrentStateCLK(long state);
    long getCurrentStateCLK();
    void setLastStateCLK(long state);
    long getLastStateCLK();
}
