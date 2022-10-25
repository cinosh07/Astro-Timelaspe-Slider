#include <Arduino.h>
#include "LRTNavigation.h"
#include "configuration.h"
#include "../navigation/LRTMenu.h"
#include "../lrt_display/LRTDisplay.h"
#include "../Camera.h"
#include "../EEPROMConfig.h"
#include "../LCD_Keypad_Reader.h" // credits to: http://www.hellonull.com/?p=282

// K.H: EPROM Params
EEPParams EEProm;

const int MODE_M = 0;
const int MODE_BULB = 1;

int localKey = 0; // The current pressed key
int lastKeyPressed = -1;
unsigned long lastKeyCheckTime = 0;
unsigned long lastKeyPressTime = 0;
unsigned long lastDispTurnedOffTime = 0;
int mode = MODE_M;    // mode: M or Bulb
int settingsSel = 1;  // the currently selected settings option
int sameKeyCount = 0; // The last pressed key

LCD_Keypad_Reader keypad;
// LiquidCrystal lcd(8, 9, 4, 5, 6, 7);	//Pin assignments for SainSmart LCD Keypad Shield


namespace LRTNavigation
{

    void setup()
    {
        // Load EEPROM params
        EEProm.ParamsRead();
        // check ranges
        EEProm.Params.BackgroundBrightnessLevel = constrain(EEProm.Params.BackgroundBrightnessLevel, cMinLevel, cMaxLevel);
        EEProm.Params.Interval = constrain(EEProm.Params.Interval, cMinInterval, cMaxInterval);
        LRTDisplay::setAct_BackLightBrightness(EEProm.Params.BackgroundBrightnessLevel);
        Camera::setInterval(EEProm.Params.Interval);
    }

    void loop() {
        if (millis() > lastKeyCheckTime + keySampleRate)
        {
            lastKeyCheckTime = millis();
            localKey = keypad.getKey();
            // Serial.println( localKey );
            // Serial.println( lastKeyPressed );

            if (localKey != lastKeyPressed)
            {
                processKey();
                keypad.RepeatRate = keyRepeatRateSlow;
            }
            else
            {
                // key value has not changed, key is being held down, has it been long enough?
                // (but don't process localKey = 0 = no key pressed)

                /* H.K.: implemented function ActRepeateRate instead of constant */
                if (localKey != 0 && millis() > lastKeyPressTime + keypad.ActRepeatRate())
                {
                    // yes, repeat this key
                    if ((localKey == UP) || (localKey == DOWN) || (localKey == SELECT))
                    {
                        processKey();
                    }
                }
            }

            if (LRTMenu::getCurrentMenu() == SCR_RUNNING)
            {
                LRTDisplay::printScreen(); // update running screen in any case

                if (mode == MODE_BULB)
                {
                    Camera::possiblyEndLongExposure(); // checks regularly if a long exposure has to be cancelled
                }
            }

            if (LRTMenu::getCurrentMenu() == SCR_SINGLE)
            {
                Camera::possiblyEndLongExposure();
            }
        }

    }

    /**
           K.H. save actual level in EPROM
        */
    void save_Params()
    {
        String str;
        if (not Camera::isCameraRunning())
        {
            boolean save = false;
            LRTDisplay::setAct_BackLightBrightness(constrain(LRTDisplay::getAct_BackLightBrightness(), cMinLevel, cMaxLevel));
            if (LRTDisplay::getAct_BackLightBrightness() != EEProm.Params.BackgroundBrightnessLevel)
            {
                str = "saved level ";
                str += (LRTDisplay::getAct_BackLightBrightness() + 1);
                str += "   ";
                // lcd.setCursor(0, 0);
                // lcd.print(str);
                delay(2000);
                save = true;
            };
            if (Camera::getInterval() != EEProm.Params.Interval)
            {
                str = "saved int. ";
                str += String(Camera::getInterval(), 2);
                str += "  ";
                // lcd.setCursor(0, 0);
                // lcd.print(str);
                delay(2000);
                save = true;
            };
            if (save)
            {
                EEProm.Params.BackgroundBrightnessLevel = LRTDisplay::getAct_BackLightBrightness();
                EEProm.Params.Interval = Camera::getInterval();
                if (not EEProm.ParamsWrite())
                {
                    // lcd.setCursor(0, 0); lcd.print("PROBLEM SAVING  ");
                    // lcd.setCursor(0, 1); lcd.print("PARAMS TO EEPROM");
                    delay(3000);
                    // lcd.clear();
                }
            }
            else
            {
                // lcd.setCursor(0, 0);
                // lcd.print("OK, no changes");
                delay(1500);
            }
            // lcd.setCursor(0, 0);
            // lcd.print("                ");
        }
    }

    void processKey()
    {

        // select key will switch backlight at any time
        if (localKey == SELECT)
        {
            if (lastKeyPressed == SELECT)
            {
                if (millis() > lastKeyPressTime + 500)
                {
                    LRTDisplay::changeBackLightBrightness('R');
                    lastKeyPressTime = millis();
                    LRTDisplay::setBacklight(1);
                }
            }
            else
            {
                LRTDisplay::setBacklight(!LRTDisplay::getBacklight());
                if (LRTDisplay::getBacklight())
                {
                    analogWrite(BACK_LIGHT, LRTDisplay::getAct_BackLightBrightness()); // Turn PWM backlight on.
                    if (millis() < lastDispTurnedOffTime + 450)
                    {
                        save_Params();
                        lastDispTurnedOffTime = 0;
                    }
                }
                else
                {
                    digitalWrite(BACK_LIGHT, LOW); // Turn backlight off.
                    lastDispTurnedOffTime = millis();
                }
                lastKeyPressed = localKey;
                lastKeyPressTime = millis();
            }
        }
        else
        {
            lastKeyPressed = localKey;
            lastKeyPressTime = millis();
        }

        // do the menu navigation
        switch (LRTMenu::getCurrentMenu())
        {

        case SCR_INTERVAL:

            if (localKey == UP)
            {
                if (Camera::getInterval() < 20)
                {
                    Camera::setInterval((float)((int)(Camera::getInterval() * 10) + 1) / 10); // round to 1 decimal place
                }
                else
                {
                    Camera::setInterval((float)((int)Camera::getInterval() + 1)); // round to 1 decimal place
                }
                if (Camera::getInterval() > cMaxInterval)
                {
                    Camera::setInterval(cMaxInterval);
                }
            }

            if (localKey == DOWN)
            {
                if (Camera::getInterval() > cMinInterval)
                {
                    if (Camera::getInterval() < 20)
                    {
                        Camera::setInterval((float)((int)(Camera::getInterval() * 10) - 1) / 10); // round to 1 decimal place
                    }
                    else
                    {
                        Camera::setInterval((float)((int)Camera::getInterval() - 1));
                    }
                }
            }

            if (localKey == RIGHT)
            {
                // lcd.clear();
                Camera::setRampTo(Camera::getInterval()); // set rampTo default to the current interval
                LRTMenu::setCurrentMenu(SCR_MODE);
            }

            if (localKey == LEFT)
            {
                // lcd.clear();
                LRTMenu::setCurrentMenu(SCR_SINGLE);
            }

            break;

        case SCR_MODE:
            if (localKey == RIGHT)
            {
                LRTMenu::setCurrentMenu(SCR_SHOTS);
            }
            else if (localKey == LEFT)
            {
                LRTMenu::setCurrentMenu(SCR_INTERVAL);
            }
            else if ((localKey == UP) || (localKey == DOWN))
            {
                if (mode == MODE_M)
                {
                    mode = MODE_BULB;
                }
                else
                {
                    mode = MODE_M;
                    Camera::setReleaseTime(RELEASE_TIME_DEFAULT); // when switching to M-Mode, set the shortest shutter release time.
                }
            }
            break;

        case SCR_SHOTS:

            if (localKey == UP)
            {
                if (Camera::getMaxNoOfShots() >= 2500)
                {
                    Camera::setMaxNoOfShots(Camera::getMaxNoOfShots() + 100);
                }
                else if (Camera::getMaxNoOfShots() >= 1000)
                {
                    Camera::setMaxNoOfShots(Camera::getMaxNoOfShots() + 50);
                }
                else if (Camera::getMaxNoOfShots() >= 100)
                {
                    Camera::setMaxNoOfShots(Camera::getMaxNoOfShots() + 25);
                }
                else if (Camera::getMaxNoOfShots() >= 10)
                {
                    Camera::setMaxNoOfShots(Camera::getMaxNoOfShots() + 10);
                }
                else
                {
                    Camera::setMaxNoOfShots(Camera::getMaxNoOfShots() + 1);
                }
                if (Camera::getMaxNoOfShots() >= 9999)
                { // prevents screwing the ui
                    Camera::setMaxNoOfShots(9999);
                }
            }

            if (localKey == DOWN)
            {
                if (Camera::getMaxNoOfShots() > 2500)
                {
                    Camera::setMaxNoOfShots(Camera::getMaxNoOfShots() - 100);
                }
                else if (Camera::getMaxNoOfShots() > 1000)
                {
                    Camera::setMaxNoOfShots(Camera::getMaxNoOfShots() - 50);
                }
                else if (Camera::getMaxNoOfShots() > 100)
                {
                    Camera::setMaxNoOfShots(Camera::getMaxNoOfShots() - 25);
                }
                else if (Camera::getMaxNoOfShots() > 10)
                {
                    Camera::setMaxNoOfShots(Camera::getMaxNoOfShots() - 10);
                }
                else if (Camera::getMaxNoOfShots() > 0)
                {
                    Camera::setMaxNoOfShots(Camera::getMaxNoOfShots() - 1);
                }
                else
                {
                    Camera::setMaxNoOfShots(0);
                }
            }

            if (localKey == LEFT)
            {
                LRTMenu::setCurrentMenu(SCR_MODE);
                Camera::stopShooting();
                // lcd.clear();
            }

            if (localKey == RIGHT)
            { // Start shooting
                if (mode == MODE_M)
                {
                    LRTMenu::setCurrentMenu(SCR_RUNNING);
                    Camera::firstShutter();
                }
                else
                {
                    LRTMenu::setCurrentMenu(SCR_EXPOSURE); // in Bulb mode ask for exposure
                }
            }
            break;

        // Exposure Time
        case SCR_EXPOSURE:

            if (localKey == UP)
            {
                Camera::setReleaseTime((float)((int)(Camera::getReleaseTime() * 10) + 1) / 10); // round to 1 decimal place
            }

            if (Camera::getReleaseTime() > Camera::getInterval() - MIN_DARK_TIME)
            { // no release times longer then (interval-Min_Dark_Time)
                Camera::setReleaseTime(Camera::getInterval() - MIN_DARK_TIME);
            }

            if (localKey == DOWN)
            {
                if (Camera::getReleaseTime() > RELEASE_TIME_DEFAULT)
                {
                    Camera::setReleaseTime((float)((int)(Camera::getReleaseTime() * 10) - 1) / 10); // round to 1 decimal place
                }
            }

            if (localKey == LEFT)
            {
                LRTMenu::setCurrentMenu(SCR_SHOTS);
            }

            if (localKey == RIGHT)
            {
                LRTMenu::setCurrentMenu(SCR_RUNNING);
                Camera::firstShutter();
            }

            break;

        case SCR_RUNNING:

            if (localKey == LEFT)
            { // LEFT from Running Screen aborts

                if (Camera::getRampingEndTime() == 0)
                { // if ramping not active, stop the whole shot, other otherwise only the ramping
                    if (Camera::isCameraRunning())
                    { // if is still runing, show confirm dialog
                        LRTMenu::setCurrentMenu(SCR_CONFIRM_END);
                    }
                    else
                    { // if finished, go to start screen
                        LRTMenu::setCurrentMenu(SCR_INTERVAL);
                    }
                    //   lcd.clear();
                }
                else
                { // stop ramping
                    Camera::setRampingStartTime(0);
                    Camera::setRampingEndTime(0);
                }
            }

            if (localKey == RIGHT)
            {
                LRTMenu::setCurrentMenu(SCR_SETTINGS);
                // lcd.clear();
            }

            if (localKey == UP)
            {
                LRTDisplay::changeBackLightBrightness('U');
            }
            if (localKey == DOWN)
            {
                LRTDisplay::changeBackLightBrightness('D');
            }
            break;

        case SCR_CONFIRM_END:
            if (localKey == LEFT)
            { // Really abort
                LRTMenu::setCurrentMenu(SCR_INTERVAL);

                if (Camera::getBulbReleasedAt() > 0)
                { // if we are shooting in bulb mode, instantly stop the exposure
                    Camera::setBulbReleasedAt(0);
                    // HV changes für Interrupt Cam release and Display indicator handling
                    //          digitalWrite(SHUTTERPIN, LOW);
                }
                Camera::stopShooting();
                // lcd.clear();
            }
            if (localKey == RIGHT)
            { // resume
                LRTMenu::setCurrentMenu(SCR_RUNNING);
                // lcd.clear();
            }
            break;

        case SCR_SETTINGS:

            if (localKey == DOWN && settingsSel == 1)
            {
                settingsSel = 2;
            }

            if (localKey == UP && settingsSel == 2)
            {
                settingsSel = 1;
            }

            if (localKey == LEFT)
            {
                settingsSel = 1;
                LRTMenu::setCurrentMenu(SCR_RUNNING);
                // lcd.clear();
            }

            if (localKey == RIGHT && settingsSel == 1)
            {
                Camera::setCameraRunning(0);
                LRTMenu::setCurrentMenu(SCR_PAUSE);
                // lcd.clear();
            }

            if (localKey == RIGHT && settingsSel == 2)
            {
                LRTMenu::setCurrentMenu(SCR_RAMP_TIME);
                // lcd.clear();
            }
            break;

        case SCR_PAUSE:

            if (localKey == LEFT)
            {
                LRTMenu::setCurrentMenu(SCR_RUNNING);
                Camera::setCameraRunning(1);
                Camera::setPreviousMillis(millis() - (Camera::getImageCount() * 1000)); // prevent counting the paused time as running time;
                                                                                        // lcd.clear();
            }
            break;

        case SCR_RAMP_TIME:

            if (localKey == RIGHT)
            {
                LRTMenu::setCurrentMenu(SCR_RAMP_TO);
                // lcd.clear();
            }

            if (localKey == LEFT)
            {
                LRTMenu::setCurrentMenu(SCR_SETTINGS);
                settingsSel = 2;
                // lcd.clear();
            }

            if (localKey == UP)
            {
                if (Camera::getRampDuration() >= 10)
                {
                    Camera::setRampDuration(Camera::getRampDuration() + 10);
                }
                else
                {
                    Camera::setRampDuration(Camera::getRampDuration() + 1);
                }
            }

            if (localKey == DOWN)
            {
                if (Camera::getRampDuration() > 10)
                {
                    Camera::setRampDuration(Camera::getRampDuration() - 10);
                }
                else
                {
                    Camera::setRampDuration(Camera::getRampDuration() - 1);
                }
                if (Camera::getRampDuration() <= 1)
                {
                    Camera::setRampDuration(1);
                }
            }
            break;

        case SCR_RAMP_TO:

            if (localKey == LEFT)
            {
                LRTMenu::setCurrentMenu(SCR_RAMP_TIME);
                // lcd.clear();
            }

            if (localKey == UP)
            {
                if (Camera::getRampTo() < 20)
                {
                    Camera::setRampTo((float)((int)(Camera::getRampTo() * 10) + 1) / 10); // round to 1 decimal place
                }
                else
                {
                    Camera::setRampTo((float)((int)Camera::getRampTo() + 1)); // round to 1 decimal place
                }
                if (Camera::getRampTo() > cMaxInterval)
                {
                    Camera::setRampTo(cMaxInterval);
                }
            }

            if (localKey == DOWN)
            {
                if (Camera::getRampTo() > cMinInterval)
                {
                    if (Camera::getRampTo() < 20)
                    {
                        Camera::setRampTo((float)((int)(Camera::getRampTo() * 10) - 1) / 10); // round to 1 decimal place
                    }
                    else
                    {
                        Camera::setRampTo((float)((int)Camera::getRampTo() - 1));
                    }
                }
            }

            if (localKey == RIGHT)
            { // start Interval ramping
                if (Camera::getRampTo() != Camera::getInterval())
                { // only if a different Ramping To interval has been set!
                    Camera::setIntervalBeforeRamping(Camera::getInterval());
                    Camera::setRampingStartTime(millis());
                    Camera::setRampingEndTime(Camera::getRampingStartTime() + Camera::getRampDuration() * 60 * 1000);
                }

                // go back to main screen
                LRTMenu::setCurrentMenu(SCR_RUNNING);
                // lcd.clear();
            }
            break;

        case SCR_DONE:

            if (localKey == LEFT || localKey == RIGHT)
            {
                LRTMenu::setCurrentMenu(SCR_INTERVAL);
                Camera::stopShooting();
                // lcd.clear();
            }
            break;

        case SCR_SINGLE:
            if (localKey == UP)
            {
                if (Camera::getReleaseTime() < 60)
                    Camera::setReleaseTime((float)((int)(Camera::getReleaseTime() + 1)));
                else
                    Camera::setReleaseTime((float)((int)(Camera::getReleaseTime() + 10)));
            }

            if (localKey == DOWN)
            {
                if (Camera::getReleaseTime() > RELEASE_TIME_DEFAULT)
                {
                    if (Camera::getReleaseTime() < 60)
                        Camera::setReleaseTime((float)((int)(Camera::getReleaseTime() - 1)));
                    else
                        Camera::setReleaseTime((float)((int)(Camera::getReleaseTime() - 10)));
                }
                if (Camera::getReleaseTime() < RELEASE_TIME_DEFAULT)
                { // if it's too short after decrementing, set to the default release time.
                    Camera::setReleaseTime(RELEASE_TIME_DEFAULT);
                }
            }

            if (localKey == LEFT)
            {
                // lcd.clear();
                // lcd.setCursor(0,0);
                // lcd.print("Decoupling...");
                delay(decoupleTime);
                // lcd.clear();
                Camera::releaseCamera();
            }

            if (localKey == RIGHT)
            {
                // lcd.clear();
                if (Camera::getBulbReleasedAt() == 0)
                { // if not running, go to main screen
                    LRTMenu::setCurrentMenu(SCR_INTERVAL);
                }
                else
                { // if running, go to confirm screen
                    LRTMenu::setCurrentMenu(SCR_CONFIRM_END_BULB);
                }
            }
            break;

        case SCR_CONFIRM_END_BULB:
            if (localKey == RIGHT)
            { // Really abort

                digitalWrite(SHUTTERPIN, LOW);
                Camera::stopShooting();

                LRTMenu::setCurrentMenu(SCR_SINGLE);
                // lcd.clear();
            }
            if (localKey == LEFT)
            { // resume
                LRTMenu::setCurrentMenu(SCR_SINGLE);
                // lcd.clear();
            }
            break;
        }
        LRTDisplay::printScreen();
    }

}