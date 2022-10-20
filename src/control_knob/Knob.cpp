#include <Arduino.h>
#include "Knob.h"
#include "configuration.h"
int _oldPosition = 0;
int currentStateCLK;
int lastStateCLK;
Encoder myEnc(ENC0, ENC1);
boolean _button = false;
long _offset = 0;
namespace Knob
{
    void setCurrentStateCLK(long state) {
        currentStateCLK = state;
    }
    long getCurrentStateCLK() {
        return currentStateCLK;
    }
    void setLastStateCLK(long state) {
        lastStateCLK = state;
    }
    long getLastStateCLK() {
        return lastStateCLK;
    }
    void setOffset(long offset)
    {
        _offset = offset;
    };
    long getOffset()
    {
        return _offset;
    };
    void setButton(boolean active)
    {
        _button = active;
    };
    boolean getButton()
    {
        return _button;
    };
    int readCLK()
    {
        return digitalRead(ENC0);
    }
    int readDT()
    {
        return digitalRead(ENC1);
    }
    int read()
    {
        return myEnc.read();
    }
    void setoldPosition(int position)
    {
        _oldPosition = position;
    };
    int getoldPosition()
    {
        return _oldPosition;
    };

}
