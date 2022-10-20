// #define DEBUG

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