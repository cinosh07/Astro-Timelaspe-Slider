#include <Arduino.h>
#include "LRTDisplay.h"
#include "../navigation/LRTMenu.h"
#include "configuration.h"
#include "../Camera.h"

// The current settings for the backlight
boolean backLight = HIGH;

int act_BackLightLevel = 4;
char act_BackLightDir = 'D';

namespace LRTDisplay
{

    byte getAct_BackLightBrightness()
    {
        return constrain(act_BackLightLevel * (255 / cMaxLevel), 12, 255);
    }
    void setAct_BackLightBrightness(byte data)
    {
        act_BackLightLevel = data;
    }
    /**
 K.H: Change Backlight Brightness in Steps
*/
    void changeBackLightBrightness(char AMode)
    { // U=up, D=douwn, R=rolling
        if ((AMode == 'U') || (AMode == 'D'))
        {
            act_BackLightDir = AMode;
        }
        if (act_BackLightDir == 'D')
        {
            act_BackLightLevel--;
        }
        else
        {
            act_BackLightLevel++;
        }

        if ((act_BackLightLevel == -1) && (AMode == 'R'))
        {
            act_BackLightDir = 'U';
        }
        if ((act_BackLightLevel == cMaxLevel + 1) && (AMode == 'R'))
        {
            act_BackLightDir = 'D';
        }
        if (AMode == 'R')
        {
            act_BackLightLevel = constrain(act_BackLightLevel, cMinLevel - 1, cMaxLevel + 1);
        }
        else
        {
            act_BackLightLevel = constrain(act_BackLightLevel, cMinLevel, cMaxLevel);
        }

        analogWrite(BACK_LIGHT, getAct_BackLightBrightness()); // Turn PWM backlight on.
    }
    boolean getBacklight()
    {
        return backLight;
    }
    void setBacklight(boolean open)
    {
        backLight = open;
    }

    /*
      K.H: dimming LCD BAckground light
    */
    void DimLCD(byte startval, byte endval, byte stepdelay)
    {
        if (endval < startval)
        {
            for (int bl = startval; bl >= endval; bl--)
            {
                analogWrite(BACK_LIGHT, bl); // dimming backlight off.
                delay(stepdelay);
            }
        }
        else
        {
            for (int bl = startval; bl <= endval; bl++)
            {
                analogWrite(BACK_LIGHT, bl); // dimming backlight off.
                delay(stepdelay);
            }
        }
    }
    void setupFinished()
    {
        DimLCD(255, getAct_BackLightBrightness(), 2);
    }
    void printConfirmEndScreen()
    {
        //   lcd.setCursor(0, 0);
        //   lcd.print( "Stop shooting?");
        //   lcd.setCursor(0, 1);
        //   lcd.print( "< Stop    Cont.>");
    }

    void printConfirmEndScreenBulb()
    {
        //   lcd.setCursor(0, 0);
        //   lcd.print( "Stop exposure?");
        //   lcd.setCursor(0, 1);
        //   lcd.print( "< Cont.   Stop >");
    }

    void printDoneScreen()
    {

        // print elapsed image count))
        //   lcd.setCursor(0, 0);
        //   lcd.print("Done ");
        //   lcd.print( imageCount );
        //   lcd.print( " shots.");

        //   // print elapsed time when done
        //   lcd.setCursor(0, 1);
        //   lcd.print( "t=");
        //   lcd.print( fillZero( runningTime / 1000 / 60 / 60 ) );
        //   lcd.print( ":" );
        //   lcd.print( fillZero( ( runningTime / 1000 / 60 ) % 60 ) );
        //   lcd.print("    ;-)");
    }

    void printSingleScreen()
    {
        //   lcd.setCursor(0, 0);

        //   if ( releaseTime < 1 ) {
        //     lcd.print( "Single Exposure");
        //     lcd.setCursor(0, 1);
        //     lcd.print(releaseTime); // under one second
        //     lcd.print("       ");
        //   } else {
        //     lcd.print( "Bulb Exposure  ");
        //     lcd.setCursor(0, 1);

        //     if ( bulbReleasedAt == 0 ) { // if not shooting
        //       // display exposure time setting
        //       int hours = (int)releaseTime / 60 / 60;
        //       int minutes = ( (int)releaseTime / 60 ) % 60;
        //       int secs = ( (int)releaseTime ) % 60;
        //       String sHours = fillZero( hours );
        //       String sMinutes = fillZero( minutes );
        //       String sSecs = fillZero( secs );

        //       lcd.print( sHours );
        //       lcd.print(":");
        //       lcd.print( sMinutes );
        //       lcd.print( "\'");
        //       lcd.print( sSecs );
        //       lcd.print( "\"");
        //       lcd.print( " " );
        //     }
        //   }

        //   if ( bulbReleasedAt == 0 ) { // currently not running

        //     lcd.setCursor(10, 1);
        //     lcd.print( "< FIRE" );

        //   } else { // running
        //     unsigned long runningTime = ( bulbReleasedAt + releaseTime * 1000 ) - millis();

        //     int hours = runningTime / 1000 / 60 / 60;
        //     int minutes = ( runningTime / 1000 / 60 ) % 60;
        //     int secs = ( runningTime / 1000 ) % 60;

        //     String sHours = fillZero( hours );
        //     String sMinutes = fillZero( minutes );
        //     String sSecs = fillZero( secs );

        //     lcd.setCursor(0, 1);
        //     lcd.print("        "); // clear time setting display
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
        //   }
    }
    void printRunningScreen()
    {

        //   lcd.setCursor(0, 0);
        //   lcd.print( printInt( imageCount, 4 ) );

        //   if ( maxNoOfShots > 0 ) {
        //     lcd.print( " R:" );
        //     lcd.print( printInt( maxNoOfShots - imageCount, 4 ) );
        //     lcd.print( " " );

        //     lcd.setCursor(0, 1);
        //     // print remaining time
        //     unsigned long remainingSecs = (maxNoOfShots - imageCount) * interval;
        //     lcd.print( "T-");
        //     lcd.print( fillZero( remainingSecs / 60 / 60 ) );
        //     lcd.print( ":" );
        //     lcd.print( fillZero( ( remainingSecs / 60 ) % 60 ) );
        //   }

        //   updateTime();

        //   lcd.setCursor(11, 0);
        //   if ( millis() < rampingEndTime ) {
        //     lcd.print( "*" );
        //   } else {
        //     lcd.print( " " );
        //   }
        //   if( interval < 100 ){ // prevent the interval display from being cut
        //     lcd.print( printFloat( interval, 4, 1 ) );
        //   } else {
        //     lcd.print( printFloat( interval, 4, 0 ) );
        //   }
    }
    void printScreen()
    {

        switch (LRTMenu::getCurrentMenu())
        {

        case SCR_INTERVAL:
            LRTMenu::printIntervalMenu();
            break;

        case SCR_MODE:
            LRTMenu::printModeMenu();
            break;

        case SCR_SHOTS:
            LRTMenu::printNoOfShotsMenu();
            break;

        case SCR_EXPOSURE:
            LRTMenu::printExposureMenu();
            break;

        case SCR_RUNNING:
            printRunningScreen();
            break;

        case SCR_CONFIRM_END:
            printConfirmEndScreen();
            break;

        case SCR_CONFIRM_END_BULB:
            printConfirmEndScreenBulb();
            break;

        case SCR_SETTINGS:
            LRTMenu::printSettingsMenu();
            break;

        case SCR_PAUSE:
            LRTMenu::printPauseMenu();
            break;

        case SCR_RAMP_TIME:
            LRTMenu::printRampDurationMenu();
            break;

        case SCR_RAMP_TO:
            LRTMenu::printRampToMenu();
            break;

        case SCR_DONE:
            printDoneScreen();
            break;

        case SCR_SINGLE:
            printSingleScreen();
            break;
        }
    }

}