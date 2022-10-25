#include <Arduino.h>
#include "configuration.h"
#include "navigation/LRTMenu.h"
#include "lrt_display/LRTDisplay.h"

float releaseTime = RELEASE_TIME_DEFAULT; // Shutter release time for camera
unsigned long previousMillis = 0;         // Timestamp of last shutter release
unsigned long runningTime = 0;

float interval = 4.0; // the current interval
long maxNoOfShots = 0;
int isRunning = 0; // flag indicates intervalometer is running
unsigned long bulbReleasedAt = 0;

int imageCount = 0; // Image count since start of intervalometer

unsigned long rampDuration = 10;    // ramping duration
float rampTo = 0.0;                 // ramping interval
unsigned long rampingStartTime = 0; // ramping start time
unsigned long rampingEndTime = 0;   // ramping end time
float intervalBeforeRamping = 0;    // interval before ramping

// HV Timer Interrupt Definitions

byte cam_Release = notshooting;
int exposureTime = 0; // Time for exposure in Timer Interrupt (10 msec)

namespace Camera

{
    
    int getImageCount()
    {
        return imageCount;
    }
    void setPreviousMillis(unsigned long time)
    {
        previousMillis = time;
    }
    void setRampTo(float position)
    {
        rampTo = position;
    }
    float getRampTo()
    {
        return rampTo;
    }

    void setIntervalBeforeRamping(float duration)
    {
        intervalBeforeRamping = duration;
    }
    float getIntervalBeforeRamping()
    {
        return intervalBeforeRamping;
    }

    void setRampDuration(unsigned long duration)
    {
        rampDuration = duration;
    }
    unsigned long getRampDuration()
    {
        return rampDuration;
    }
    void setBulbReleasedAt(unsigned long time)
    {
        bulbReleasedAt = time;
    }
    unsigned long getBulbReleasedAt()
    {
        return bulbReleasedAt;
    }

    void setRampingEndTime(unsigned long time)
    {
        rampingEndTime = time;
    }
    unsigned long getRampingEndTime()
    {
        return rampingEndTime;
    }
    void setRampingStartTime(unsigned long time)
    {
        rampingStartTime = time;
    }
    unsigned long getRampingStartTime()
    {
        return rampingStartTime;
    }

    void setMaxNoOfShots(int total)
    {
        maxNoOfShots = total;
    }
    int getMaxNoOfShots()
    {
        return maxNoOfShots;
    }

    void setReleaseTime(float time)
    {
        releaseTime = time;
    }
    float getReleaseTime()
    {
        return releaseTime;
    }

    /**
       Update the time display in the main screen
    */
    void updateTime()
    {

        unsigned long finerRunningTime = runningTime + (millis() - previousMillis);

        if (isRunning)
        {

            int hours = finerRunningTime / 1000 / 60 / 60;
            int minutes = (finerRunningTime / 1000 / 60) % 60;
            int secs = (finerRunningTime / 1000) % 60;

            String sHours = LRTMenu::fillZero(hours);
            String sMinutes = LRTMenu::fillZero(minutes);
            String sSecs = LRTMenu::fillZero(secs);

            //     lcd.setCursor(8, 1);
            //     lcd.print( sHours );
            //     lcd.setCursor(10, 1);
            //     lcd.print(":");
            //     lcd.setCursor(11, 1);
            //     lcd.print( sMinutes );
            //     lcd.setCursor(13, 1);
            //     lcd.print(":");
            //     lcd.setCursor(14, 1);
            //     lcd.print( sSecs );
            //   } else {
            //     lcd.setCursor(8, 1);
            //     lcd.print("   Done!");
            //   }
        };
    }

    int isCameraRunning()
    {
        return isRunning;
    }
    void setCameraRunning(int data)
    {
        isRunning = data;
    }

    void setInterval(float duration)
    {
        interval = duration;
    }
    float getInterval()
    {
        return interval;
    }

    void stopShooting()
    {
        isRunning = 0;
        imageCount = 0;
        runningTime = 0;
        bulbReleasedAt = 0;
    }

    /**
       If ramping was enabled do the ramping
    */
    void possiblyRampInterval()
    {

        if ((millis() < rampingEndTime) && (millis() >= rampingStartTime))
        {
            interval = intervalBeforeRamping + ((float)(millis() - rampingStartTime) / (float)(rampingEndTime - rampingStartTime) * (rampTo - intervalBeforeRamping));

            if (releaseTime > interval - MIN_DARK_TIME)
            { // if ramping makes the interval too short for the exposure time in bulb mode, adjust the exposure time
                releaseTime = interval - MIN_DARK_TIME;
            }
        }
        else
        {
            rampingStartTime = 0;
            rampingEndTime = 0;
        }
    }

    /**
       Actually release the camera
    */
    void releaseCamera()
    {

        // short trigger in M-Mode
        if (releaseTime < 1)
        {
            if (LRTMenu::getCurrentMenu() == SCR_RUNNING)
            { // display exposure indicator on running screen only
              //   lcd.setCursor(7, 1);
              //   lcd.print((char)255);
            }
            // HV changes für Interrupt Cam release and Display indicator handling
            if (releaseTime < 0.2)
            {
                exposureTime = releaseTime * 150; // for better viewability
            }
            else
            {
                exposureTime = releaseTime * 100;
            }
            cam_Release = shooting;

            digitalWrite(SHUTTERPIN, HIGH);
            //    delay( releaseTime * 1000 );
            //    digitalWrite(SHUTTERPIN, LOW);

            //    if ( currentMenu == SCR_RUNNING ) { // clear exposure indicator
            //      lcd.setCursor(7, 1);
            //      lcd.print(" ");
            //    }
        }
        else
        { // releaseTime > 1 sec

            // long trigger in Bulb-Mode for longer exposures
            if (bulbReleasedAt == 0)
            {
                bulbReleasedAt = millis();
                // HV changes für Interrupt Cam release and Display indicator handling
                //   lcd.setCursor(7, 1);
                //   lcd.print((char)255);
                exposureTime = releaseTime * 100;
                cam_Release = shooting;
                digitalWrite(SHUTTERPIN, HIGH);
            }
        }
    }

    void firstShutter()
    {

        // lcd.clear();
        //   lcd.setCursor(0,0);
        //   lcd.print("Decoupling...");
        delay(decoupleTime);

        previousMillis = millis();
        runningTime = 0;
        isRunning = 1;

        // lcd.clear();
        LRTDisplay::printRunningScreen();

        // do the first release instantly, the subsequent ones will happen in the loop
        releaseCamera();
        imageCount++;
    }
    /**
       Running, releasing Camera
    */
    void running()
    {

        // do this every interval only
        if ((millis() - previousMillis) >= ((interval * 1000)))
        {

            if ((maxNoOfShots != 0) && (imageCount >= maxNoOfShots))
            { // sequence is finished
                // stop shooting
                isRunning = 0;
                LRTMenu::setCurrentMenu(SCR_DONE);
                // lcd.clear();
                LRTDisplay::printDoneScreen(); // invoke manually
                stopShooting();
            }
            else
            { // is running
                runningTime += (millis() - previousMillis);
                previousMillis = millis();
                releaseCamera();
                imageCount++;
            }
        }

        // do this always (multiple times per interval)
        possiblyRampInterval();
    }
    void loop() {
        // HV Interrupt Cam release and Display indicator handling
        if (cam_Release == shooting)
        {

            if (exposureTime == 0) // End of exposure
            {
                cam_Release = notshooting;
                digitalWrite(SHUTTERPIN, LOW);

                if (LRTMenu::getCurrentMenu() == SCR_RUNNING)
                { // clear exposure indicator
                    // lcd.setCursor(7, 1);
                    // lcd.print(" ");
                }
            }
        }
        if (isRunning)
        { // release camera, do ramping if running
            running();
        }
    }

    /**
      Will be called by the loop and check if a bulb exposure has to end. If so, it will stop the exposure.
    */
    void possiblyEndLongExposure()
    {
        if ((bulbReleasedAt != 0) && (millis() >= (bulbReleasedAt + releaseTime * 1000)))
        {
            bulbReleasedAt = 0;
            // HV changes für Interrupt Cam release and Display indicator handling
            //    digitalWrite(SHUTTERPIN, LOW);
        }

        //  if ( currentMenu == SCR_RUNNING ) { // display exposure indicator on running screen only
        //    if ( bulbReleasedAt == 0 ) {
        //      lcd.setCursor(7, 1);
        //      lcd.print(" ");
        //    } else {
        //      lcd.setCursor(7, 1);
        //      lcd.print((char)255);
        //    }
        //  }

        if (LRTMenu::getCurrentMenu() == SCR_SINGLE)
        {
            LRTDisplay::printSingleScreen();
        }
    }

}