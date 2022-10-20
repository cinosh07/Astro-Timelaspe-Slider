#include <Arduino.h>
#include "configuration.h"
#include "TimerOne.h"
#include <stdint.h>
#include "TouchScreen.h"
#include <MCUFRIEND_kbv.h>
#include <Fonts/FreeSans9pt7b.h>      // Hardware-specific library
#include <Fonts/FreeSans12pt7b.h>     // Hardware-specific library
#include <Fonts/FreeSans18pt7b.h>     // Hardware-specific library
#include <Fonts/FreeSans24pt7b.h>     // Hardware-specific library
#include <Fonts/FreeMono24pt7b.h>     // Hardware-specific library
#include <Fonts/FreeMono18pt7b.h>     // Hardware-specific library
#include <Fonts/FreeMonoBold18pt7b.h> // Hardware-specific library
#include <Fonts/FreeMonoBold24pt7b.h> // Hardware-specific library
#include <BfButton.h>
// #include <SPI.h>
// #include <SdFat.h>  
#include <control_knob/Knob.h>
#include <display/Display.h>
#include <motors/Motors.h>
#include <clock/Clock.h>

Display display;

volatile long numruns = 0;
volatile long period = 10000;
long nextBattMillis = 0;
float rawVolt;
float volt;
BfButton btn(BfButton::STANDALONE_DIGITAL, BUTTON, true, LOW);

int counter = 0;

int currentDir = 0;


// #if SPI_DRIVER_SELECT != 2
// // #error edit SdFatConfig.h .  READ THE SKETCH INSTRUCTIONS
// #endif

// SoftSpiDriver<12, 11, 13> softSpi; //Bit-Bang on the Shield pins SDFat.h v2
// SdFat SD;
// #define SD_CS SdSpiConfig(10, DEDICATED_SPI, SD_SCK_MHZ(0), &softSpi)


void callback()
{
  numruns++;
  digitalWrite(STEP, digitalRead(STEP) ^ 1);
}

void updateSliderRuntime()
{
  long rangeToStep = 0;

  rangeToStep = Clock::getRuntime() + (counter * 1000);
  if (rangeToStep < MINRUNTIME)
  {
    rangeToStep = MINRUNTIME;
  }
  Clock::setSliderRuntime(rangeToStep);

#ifdef DEBUG
  Serial.print("Slider Previous Runtime: ");
  Serial.println(previousRuntime);
  Serial.print("Slider new Runtime: ");
  Serial.println(Clock::getRuntime());
#endif
  if (display.getNeedsInits())
  {
    display.updateLCDTime(true);
  }
  else
  {
    display.updateLCDTime(false);
  }

  display.updateLCDStatus();
  counter = 0;
}

void setPeriod()
{
  if ((Clock::getRuntime() < MINRUNTIME) | !Motors::isRunning())
  {
    Timer1.stop();
  }

  else
  {
    float ssspeed = 1000000 / ((float)Clock::getRuntime());
    float sps = ssspeed * STEPSMM;
    float pperiod = 500000 / sps;
    Timer1.setPeriod(pperiod);
  }
}

float toVolt(float rawADC)
{
  return display.mapFloat(rawADC, 0, 1023, 0, 10.8); // empirical calibration
}

// Button press hanlding function
void pressHandler(BfButton *btn, BfButton::press_pattern_t pattern)
{
  switch (pattern)
  {
  case BfButton::SINGLE_PRESS:
#ifdef DEBUG
    Serial.println("Single push");
#endif
    if (Motors::isRunning())
    {
      Knob::setButton(true);
    }

    break;

  case BfButton::DOUBLE_PRESS:
#ifdef DEBUG
    Serial.println("Double push");
#endif
    break;

  case BfButton::LONG_PRESS:
    if (!Motors::isRunning())
    {
      Knob::setButton(true);
    }
#ifdef DEBUG
    Serial.println("Long push");
#endif
    break;
  }
}
void setup()
{

  pinMode(SHUTTERPIN, LOW);
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  // Button settings
  btn.onPress(pressHandler)
      .onDoublePress(pressHandler) // default timeout
      .onPressFor(pressHandler, 1000);

#ifdef DEBUG
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));
  Serial.print("TFT size is ");
  Serial.print(display.width());
  Serial.print("x");
  Serial.println(display.height());
#endif // DEBUG
  display.reset();

  uint16_t identifier = display.readID();

  display.initialize(identifier, BLACK, 1);

  Serial.begin(115200);

  Knob::setLastStateCLK(Knob::readCLK());

  Timer1.initialize(period); // initialize timer1, and set a 1/2 second period
  //  Timer1.pwm(9, 512);                // setup pwm on pin 9, 50% duty cycle
  Timer1.attachInterrupt(callback); // attaches callback() as a timer overflow interrupt
  Timer1.stop();
  display.SDbegin();
 
  // SD.begin(SD_CS)



  // if (!SD.begin(SD_CS)) {
 
  //   Serial.println("Failed to init SD Card");
  // }
  display.showSplashScreen();
  

  display.updateLCDTime(true);
  display.updateLCDStatus();

  rawVolt = ((float)analogRead(VOLTAGEPIN) + analogRead(VOLTAGEPIN) + analogRead(VOLTAGEPIN) + analogRead(VOLTAGEPIN) + analogRead(VOLTAGEPIN)) / 5;
  volt = toVolt(rawVolt);
}

void loop()
{
  btn.read();

  Knob::setCurrentStateCLK(Knob::readCLK());

  if (Knob::getCurrentStateCLK() != Knob::getLastStateCLK() && Knob::getCurrentStateCLK() == 1)
  {

    // If the DT state is different than the CLK state then
    // the encoder is rotating CCW so decrement
    if (Knob::readDT() != Knob::getCurrentStateCLK())
    {
      counter++;
      currentDir = 1;
    }
    else
    {
      // Encoder is rotating CW so increment
      counter--;
      currentDir = -1;
    }
#ifdef DEBUG
    Serial.print("Direction: ");
    Serial.print(currentDir);
    Serial.print(" | Counter: ");
    Serial.println(counter);
#endif
  }

  Knob::setLastStateCLK(Knob::getCurrentStateCLK());

  if (counter != 0)
  {

    updateSliderRuntime();
    setPeriod();
  }

  if (Knob::getButton())
  {
    if ((Motors::getSpeed() == 0) && (Motors::getDecimals() == 0))
    {
      Motors::setRunning(0);
    }
    else
    {
      Motors::setRunning(!Motors::isRunning());
      setPeriod();
    }
    display.updateLCDStatus();
    Knob::setButton(false);
  }

  // a point object holds x y and z coordinates
  TSPoint p = display.getPoint();

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
  {
#ifdef DEBUG
    Serial.print("TSPoint y: ");
    Serial.println(p.y);
    Serial.print("TSPoint z: ");
    Serial.println(p.z);
    Serial.print("p.y < 512: ");
    Serial.println(p.y < 512);
#endif
    
    if (p.x > 600)
    {
        Motors::setSliderDirection(!Motors::getOldSdir());
    }

    if (Motors::getSliderDirection() != Motors::getOldSdir())
    {
      display.updateLCDStatus();
    }
    digitalWrite(DIR, Motors::getSliderDirection());
  }

  if ((long)(millis() - nextBattMillis) >= 0)
  {
    // update Battery status

    nextBattMillis += 10000;
    rawVolt = ((float)analogRead(VOLTAGEPIN)) * 0.02 + rawVolt * 0.98;
#ifdef DEBUG
    Serial.print("Raw voltage ....");
    Serial.println(rawVolt);
#endif
    volt = toVolt(rawVolt);

    display.setVoltage(volt);
#ifdef DEBUG
    Serial.print(volt);
    Serial.print("; ");
    Serial.println(millis());
#endif
  }
}
