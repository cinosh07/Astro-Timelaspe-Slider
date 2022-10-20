#include <Arduino.h>
#include "Clock.h"
#include "configuration.h"
long _runtime = MINRUNTIME;

namespace Clock
{
    void setSliderRuntime(long runtime)
    {
        _runtime = runtime;
    };
    long getRuntime()
    {
        return _runtime;
    };

}
