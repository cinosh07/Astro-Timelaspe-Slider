// #define DEBUG


#define CAPTION "Pro-Timer 0.92";

//Define if LRTimelapse Pro is on original display or not 
// #define LRTimelapse_legacy - TODO

//Define if the firmware is used for LRTimelapse Pro TImer Free only. (Without Slider)
// #define LRTimelapse_Only - TODO


// #define MODE_M 0;
// #define MODE_BULB 1;
#define shooting 1
#define notshooting 0

#define cMinInterval 0.2
#define cMaxInterval 999 // no intervals longer as 999secs - those would scramble the display

// #define keySampleRate 100;      // ms between checking keypad for key

// /* K.H: "Added KeyRepeatRateAcceleration" to   customize dynamic behavior */
// #define keyRepeatRateHighDelay 500;      // delay for speeding up
// #define keyRepeatRateStep 60;      // speeding up steps
// #define keyRepeatRateSlow 500;      // SLOW speed when held, key repeats 1000 / keyRepeatRate times per second
// #define keyRepeatRateHigh 100;      // HIGH speed when held, key repeats 1000 / keyRepeatRate times per second



//LRTMenu Constant
#define SCR_INTERVAL 0			// menu workflow constants
#define SCR_SHOTS 1
#define SCR_MODE 9
#define SCR_EXPOSURE 10
#define SCR_RUNNING 2
#define SCR_CONFIRM_END 3
#define SCR_CONFIRM_END_BULB 12
#define SCR_SETTINGS 4
#define SCR_PAUSE 5
#define SCR_RAMP_TIME 6
#define SCR_RAMP_TO 7
#define SCR_DONE 8

// LRT constants
#define NONE 0
#define SELECT 1
#define LEFT 2
#define UP 3
#define DOWN 4
#define RIGHT 5

//LRT Display Constants
#define Onboard_LED 40
#define BACK_LIGHT 41

#define RELEASE_TIME_DEFAULT 0.1 // default shutter release time for camera
#define MIN_DARK_TIME 0.5

#define keyRepeatRate 100 // when held, key repeats 1000 / keyRepeatRate times per second

#define decoupleTime 1000 // time in milliseconds to wait before doing a single bulb exposure
// K.H. LCD dimming
#define cMinLevel 0 // Min. Background Brightness Levels
#define cMaxLevel 5 // Max. Background Brightness Levels
// left side MENUS
#define SCR_SINGLE 11

// These are the pins for the shield!
#define YP A1 // must be an analog pin, use "An" notation!
#define XM A2 // must be an analog pin, use "An" notation!
#define YM 7  // can be a digital pin
#define XP 6  // can be a digital pin

#define SDCS 10
#define BUFFPIXEL 20 
#define BMPIMAGEOFFSET 54
#define PALETTEDEPTH   8  

#define VOLTAGEPIN 15
#define SHUTTERPIN 51
#define MINPRESSURE 1
#define MAXPRESSURE 10000
#define USE_SDFAT
#define NAMEMATCH "" 

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// Assign human-readable names to some common 16-bit color values
// Modify these to match the colorspace of your LCD
#define WHITE 0xFFFF  //
#define YELLOW 0xFFE0 //
#define CYAN 0x07FF   //
#define PURPLE 0xF81F //
#define RED 0xF800    //
#define GREEN 0x07E0  //
#define BLUE 0x001F   //
#define BLACK 0x0000  //

#define DIR 38
#define STEP 39

#define STEPSMM 160

#define BUTTON 23
#define ENC0 25
#define ENC1 27

// #define DEBOUNCE 300

#define MINRUNTIME 15000

// number of battery voltage points
#define LOOKUP 20