#pragma once
#include <Arduino.h>

namespace LRTMenu
{
    String fillZero(int input);
    void printIntervalMenu();
    void printExposureMenu();
    void printModeMenu();
    void printNoOfShotsMenu();
    void printNoOfShotsMenu();
    void printPauseMenu();
    void printSettingsMenu();
    void printRampDurationMenu();
    void printRampToMenu();
    void setCurrentMenu(int menu);
    int getCurrentMenu();




}
