#pragma once
#include <Arduino.h>

namespace Camera
{
    float getReleaseTime();
    void setReleaseTime(float time);
    void stopShooting();
    void possiblyRampInterval();
    void releaseCamera();
    void firstShutter();
    void running();
    void possiblyEndLongExposure();
    void setInterval(float duration);
    float getInterval();
    int isCameraRunning();
    void setCameraRunning(int data);
    void setRampTo(float value);
    float getRampTo();
    void setReleaseTime(float time);
    float getReleaseTime();
    void setMaxNoOfShots(int total);
    int getMaxNoOfShots();
    void setRampingEndTime(unsigned long time);
    unsigned long getRampingEndTime();
    void setRampingStartTime(unsigned long time);
    unsigned long getRampingStartTime();
    void setBulbReleasedAt(unsigned long time);
    unsigned long getBulbReleasedAt();
    void setRampDuration(unsigned long duration);
    unsigned long getRampDuration();
    void setRampTo(float position) ;
    float getRampTo();
    void setIntervalBeforeRamping(float duration);
    float getIntervalBeforeRamping();
    void setPreviousMillis(unsigned long time);
    int getImageCount();
    void loop();




}