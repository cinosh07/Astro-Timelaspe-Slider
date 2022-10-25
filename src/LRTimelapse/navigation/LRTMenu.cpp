#include <Arduino.h>
#include "LRTMenu.h"
#include <LRTimelapse/LRTimelapse.h>
#include "../Camera.h"

int currentMenu = 0; // the currently selected menu
// left side MENUS
const int SCR_SINGLE = 11;

namespace LRTMenu
{

    void setCurrentMenu(int menu)
    {
        currentMenu = menu;
    }
    int getCurrentMenu()
    {
        return currentMenu;
    }
    // ----------- HELPER METHODS -------------------------------------

    /**
       Fill in leading zero to numbers in order to always have 2 digits
    */
    String fillZero(int input)
    {

        String sInput = String(input);
        if (sInput.length() < 2)
        {
            sInput = "0";
            sInput.concat(String(input));
        }
        return sInput;
    }

    String printFloat(float f, int total, int dec)
    {

        static char dtostrfbuffer[8];
        String s = dtostrf(f, total, dec, dtostrfbuffer);
        return s;
    }

    String printInt(int i, int total)
    {
        float f = i;
        static char dtostrfbuffer[8];
        String s = dtostrf(f, total, 0, dtostrfbuffer);
        return s;
    }
    // ---------------------- SCREENS AND MENUS -------------------------------

    /**
       Configure Interval setting (main screen)
    */
    void printIntervalMenu()
    {

        //   lcd.setCursor(0, 0);
        //   lcd.print("Interval        ");
        //   lcd.setCursor(0, 1);
        //   if( interval < 20 ){
        //     lcd.print( printFloat( interval, 5, 1 ) );
        //   } else {
        //     lcd.print( printFloat( interval, 3, 0 ) );
        //   }
        //   lcd.print( "          " );
    }

    /**
       Configure Exposure setting in Bulb mode
    */
    void printExposureMenu()
    {

        //   lcd.setCursor(0, 0);
        //   lcd.print("Exposure        ");
        //   lcd.setCursor(0, 1);

        int hours = (int)Camera::getReleaseTime() / 60 / 60;
        int minutes = ((int)Camera::getReleaseTime() / 60) % 60;
        int secs = ((int)Camera::getReleaseTime()) % 60;
        String sHours = fillZero(hours);
        String sMinutes = fillZero(minutes);
        String sSecs = fillZero(secs);

        //   lcd.print(releaseTime);
        //   lcd.print( "           " );
    }

    /**
       Configure Mode setting
    */
    void printModeMenu()
    {

        //   lcd.setCursor(0, 0);
        //   lcd.print("Mode");
        //   lcd.setCursor(0, 1);
        //   if ( mode == MODE_M ) {
        //     lcd.print( "M (Default)     " );
        //   } else {
        //     lcd.print( "Bulb (Astro)    " );
        //   }
    }

    /**
       Configure no of shots - 0 means infinity
    */
    void printNoOfShotsMenu()
    {

        // lcd.clear();
        //   lcd.setCursor(0, 0);
        //   lcd.print("No of shots");
        //   lcd.setCursor(0, 1);
        //   if ( maxNoOfShots > 0 ) {
        //     lcd.print( printInt( maxNoOfShots, 4 ) );
        //   } else {
        //     lcd.print( "unlimited" );
        //   }
        //   lcd.print( "       "); // clear rest of display
    }

    /**
      Pause Mode
   */
    void printPauseMenu()
    {

        //   lcd.setCursor(0, 0);
        //   lcd.print("PAUSE...        ");
        //   lcd.setCursor(0, 1);
        //   lcd.print("< Continue");
    }
    /**
       Print Settings Menu
    */
    void printSettingsMenu(){

        //   lcd.setCursor(1, 0);
        //   lcd.print("Pause          ");
        //   lcd.setCursor(1, 1);
        //   lcd.print("Ramp Interval  ");
        //   lcd.setCursor(0, settingsSel - 1);
        //   lcd.print(">");
        //   lcd.setCursor(0, 1 - (settingsSel - 1));
        //   lcd.print(" ");
    };

    /**
       Print Ramping Duration Menu
    */
    void printRampDurationMenu()
    {

        //   lcd.setCursor(0, 0);
        //   lcd.print("Ramp Time (min) ");

        //   lcd.setCursor(0, 1);
        //   lcd.print( rampDuration );
        //   lcd.print( "     " );
    }

    /**
       Print Ramping To Menu
    */
    void printRampToMenu()
    {

        //   lcd.setCursor(0, 0);
        //   lcd.print("Ramp to (Intvl.)");

        //   lcd.setCursor(0, 1);
        //   if( rampTo < 20 ){
        //     lcd.print( printFloat( rampTo, 5, 1 ) );
        //   } else {
        //     lcd.print( printFloat( rampTo, 3, 0 ) );
        //   }
        //   lcd.print( "             " );
        // }

        // ISR(TIMER1_OVF_vect)
        // {
        //   TCNT1 = 64912;             // Reload Timer Counter
        //   if (exposureTime >0) {
        //      exposureTime --;
        //   }
    }

}