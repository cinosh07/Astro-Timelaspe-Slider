#include <Arduino.h>
#include "Display.h"
#include "configuration.h"
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
#include <SPI.h>
#include <SdFat.h>
#include <control_knob/Knob.h>
#include <motors/Motors.h>
#include <clock/Clock.h>
// #include <images/splash.h>

long day = 86400000; // 86400000 milliseconds in a day
long hour = 3600000; // 3600000 milliseconds in an hour
long minute = 60000; // 60000 milliseconds in a minute
long second = 1000;  // 1000 milliseconds in a second
byte oldDays = 0;
byte oldHours = 0;
byte oldMinutes = 0;
byte oldSeconds = 0;

boolean drawnStatusBlank = 1;
boolean drawnStatus = Motors::isRunning();
boolean _needsInit = false;

// Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
//  If using the shield, all control and data lines are fixed, and
//  a simpler declaration can optionally be used:
//  Adafruit_TFTLCD tft;
MCUFRIEND_kbv tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#if SPI_DRIVER_SELECT != 2
// #error edit SdFatConfig.h .  READ THE SKETCH INSTRUCTIONS
#endif

SoftSpiDriver<12, 11, 13> softSpi; // Bit-Bang on the Shield pins SDFat.h v2
SdFat SD;

#define SD_CS SdSpiConfig(SDCS, DEDICATED_SPI, SD_SCK_MHZ(0), &softSpi)

char namebuf[32] = "/splash.bmp"; // BMP files in root directory
File root;
int pathlen;

float Display::mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
Display::Display()
{
}
void Display::initialize(uint16_t identifier, uint16_t color, int rotation)
{
  Serial.println("Display is initializing ....");
  tft.begin(identifier);
  tft.setRotation(rotation);
  tft.fillScreen(color);
}
void Display::setNeedInits(boolean init)
{
  _needsInit = init;
};
boolean Display::getNeedsInits()
{
  return _needsInit;
};
void Display::setAddrWindow(int x, int y, int width, int height)
{
  tft.setAddrWindow(x, y, width, height);
};
void Display::SDbegin()
{
  SD.begin(SD_CS);
};

void Display::showSplashScreen()
{

  root = SD.open(namebuf);
  pathlen = strlen(namebuf);
  char *nm = namebuf + pathlen;
  File f = root;
  uint8_t ret;
  uint32_t start;

//TODO Tester ce changement en enlevant la carte SD et faire un reset
// https://stackoverflow.com/questions/6040382/how-to-get-rid-of-warning-converting-to-non-pointer-type-char-from-null
  // if (f != NULL)
   if (f != 0)
  {
#ifdef USE_SDFAT
    f.getName(nm, 32 - pathlen);
#else
    strcpy(nm, (char *)f.name());
#endif
    f.close();
    strlwr(nm);
    if (strstr(nm, ".bmp") != NULL && strstr(nm, NAMEMATCH) != NULL)
    {
      Serial.print(namebuf);
      Serial.print(F(" - "));
      tft.fillScreen(0);
      start = millis();
      ret = showBMP(namebuf, 5, 5);
      switch (ret)
      {
      case 0:
        Serial.print(millis() - start);
        Serial.println(F("ms"));
        delay(5000);
        break;
      case 1:
        Serial.println(F("bad position"));
        break;
      case 2:
        Serial.println(F("bad BMP ID"));
        break;
      case 3:
        Serial.println(F("wrong number of planes"));
        break;
      case 4:
        Serial.println(F("unsupported BMP format"));
        break;
      case 5:
        Serial.println(F("unsupported palette"));
        break;
      default:
        Serial.println(F("unknown"));
        break;
      }
    }
  }

  showBMP("splash.bmp", 5, 5);
  tft.setFont(&FreeSans18pt7b);
  tft.setCursor(60, 100);
  tft.setTextColor(WHITE);
  tft.print("Astro Timelapse Slider");
  tft.setCursor(140, 230);
  tft.setTextColor(RED);
  tft.print("Initialization ...");
  tft.setCursor(25, 265);
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(WHITE);
  tft.print("LRTimelapse Pro Timer Free - by LRTimelapse");
  tft.setCursor(25, 290);
  tft.print("Version 0.1 - by Carl Tremblay - Copyright 2022");

  delay(5000);
  tft.fillScreen(BLACK);
};
TSPoint Display::getPoint()
{
  return ts.getPoint();
};
void Display::setVoltage(float volt)
{
  tft.setTextColor(WHITE);

  if (volt > 7.2)
  {
    tft.setTextColor(GREEN);
  }

  if (volt < 7.2)
  {
    tft.setTextColor(YELLOW);
  }

  if (volt < 6.5)
  {
    tft.setTextColor(RED);
  }

  tft.fillRect(0, 110, 150, 22, BLACK);
  tft.setTextSize(1);
  tft.setCursor(0, 130);
  tft.setFont(&FreeSans12pt7b);

  tft.print(mapFloat(volt, 6, 8.4, 0, 100));
  tft.print("% ");
  tft.print(volt);
  tft.print("V ");
};
void Display::reset()
{
  tft.reset();
};
uint16_t Display::readID()
{
  return tft.readID();
};
int Display::width()
{
  return tft.width();
};
int Display::height()
{
  return tft.height();
};
void Display::updateLCDTime(boolean firstrun)
{
#ifdef DEBUG
  Serial.println("Display is updateLCDTime ....");
#endif
  int days = Clock::getRuntime() / day;                        // number of days
  int hours = (Clock::getRuntime() % day) / hour;              // the remainder from days division (in milliseconds) divided by hours, this gives the full hours
  int minutes = ((Clock::getRuntime() % day) % hour) / minute; // and so on...
  int seconds = (((Clock::getRuntime() % day) % hour) % minute) / second;

  tft.setCursor(0, 20);
  tft.setTextSize(4);
  tft.setFont();

  // if (Knob::getoldPosition() == 0)
  // {

  //   tft.setCursor(42, 70);
  //   tft.setFont(&FreeMonoBold24pt7b);

  //   tft.setTextSize(2);

  //   tft.fillRect(0, 5, 480, 28, BLACK);  // clear time
  //   tft.fillRect(0, 54, 480, 35, BLACK); // clear speed

  //   tft.setTextColor(RED);
  //   tft.print("HALT");

  //   Motors::setRunning(false);

  //   tft.setTextSize(1);

  //   tft.setFont(&FreeMono18pt7b);

  //   tft.setCursor(0, 100);
  //   tft.setTextColor(WHITE);
  //   //   tft.print("0 mm/s");

  //   Motors::setSpeed(0);
  //   Motors::setDecimals(0);
  //   _needsInit = true;
  // }

  // else
  // {

  tft.setTextColor(WHITE);

  if (firstrun)
  {
    tft.fillRect(42, 24, 230, 30, BLACK); // clear HALT/STOP
  }

  tft.setFont(&FreeMonoBold18pt7b);
  tft.setTextSize(1);

  // DAYS

  if ((days != oldDays) || firstrun)
  {
    tft.setCursor(0, 30);

    tft.fillRect(0, 5, 42, 28, BLACK);
    if (days < 10)
    {
      tft.print("0");
    }
    tft.print(days);
    oldDays = days;
  }

  // HOURS

  if (firstrun)
  {

    tft.print("d");
  }

  tft.setCursor(80, 30);

  if ((hours != oldHours) || firstrun)
  {
    tft.fillRect(80, 5, 42, 28, BLACK);
    if (hours < 10)
    {
      tft.print("0");
    }
    tft.print(hours);
    oldHours = hours;
  }

  // MINUTES

  if (firstrun)
  {

    tft.print("h");
  }

  tft.setCursor(160, 30);

  if ((minutes != oldMinutes) || firstrun)
  {
    tft.fillRect(160, 5, 42, 28, BLACK);
    if (minutes < 10)
    {
      tft.print("0");
    }
    tft.print(minutes);
    oldMinutes = minutes;
  }

  // SECONDS

  if (firstrun)
  {

    tft.print("m");
  }

  tft.setCursor(240, 30);

  if ((seconds != oldSeconds) || firstrun)
  {
    tft.fillRect(240, 5, 42, 28, BLACK);
    if (seconds < 10)
    {
      tft.print("0");
    }
    tft.print(seconds);
    oldSeconds = seconds;
  }

  if (firstrun)
  {

    tft.print("s");

    _needsInit = false;
  }

  // TODO fix the speed doest display correct valur cause of this if beeing trigger when not necessary
  //  if ((Knob::getoldPosition() + Knob::getOffset()) != Knob::read())
  //  {
  //    return;
  //  }

  tft.fillRect(0, 54, 480, 35, BLACK);
  tft.setCursor(0, 80);
  tft.setTextSize(1);
  tft.setFont(&FreeMono18pt7b);
  tft.setTextColor(WHITE);

  Motors::setSpeed((1000000) / (Clock::getRuntime()));

  if (Clock::getRuntime() < (MINRUNTIME * 2))
  {
    tft.setTextColor(YELLOW);
  }
  if (Clock::getRuntime() < MINRUNTIME)
  {

    tft.setTextColor(RED);
  }

  tft.print(Motors::getSpeed());
  tft.print(".");
  Motors::setDecimals(((1000000000) / (Clock::getRuntime())) - (((1000000) / (Clock::getRuntime())) * 1000));

  if (Motors::getDecimals() < 100)
  {
    if (Motors::getDecimals() < 10)
    {
      tft.print("0");
    }
    tft.print("0");
  }

  tft.print(Motors::getDecimals());
  tft.print(" mm/s");
  // }
}

void Display::updateLCDStatus()
{
#ifdef DEBUG
  Serial.println("Display is updateLCDStatus ....");

  Serial.println("Motors::getSliderDirection() != Motors::getOldSdir() ....");
  Serial.println(Motors::getSliderDirection() != Motors::getOldSdir());

  Serial.println("Motors::getSliderDirection()  ....");
  Serial.println(Motors::getSliderDirection());

  Serial.println("Motors::getOldSdir() ....");
  Serial.println(Motors::getOldSdir());
#endif

  if (Motors::getSliderDirection() != Motors::getOldSdir())
  {
    if (!Motors::getSliderDirection())
    {



tft.fillTriangle(
          0, 200,  // peak
          0, 320,  // bottom left
          200, 320, // bottom right
          BLACK);

      tft.fillTriangle(
          480, 200, // peak
          280, 320, // bottom left
          480, 320, // bottom right
          tft.color565(255, 0, 50));
      
#ifdef DEBUG
      Serial.println("Right bracket is red ....");
#endif
    }

    else
    {
      tft.fillTriangle(
          480, 200, // peak
          280, 320, // bottom left
          480, 320, // bottom right
          BLACK);

      tft.fillTriangle(
          0, 200,  // peak
          0, 320,  // bottom left
          200, 320, // bottom right
          tft.color565(255, 0, 50));
#ifdef DEBUG
      Serial.println("Left bracket is red ....");
#endif
    }

    Motors::setOldSdir(Motors::getSliderDirection());
  }
#ifdef DEBUG
  Serial.println("Clock::getRuntime() < MINRUNTIME ....");
  Serial.println(Clock::getRuntime() < MINRUNTIME);

  Serial.println("drawnStatusBlank || (drawnStatus != Motors::isRunning()) ....");
  Serial.println(drawnStatusBlank || (drawnStatus != Motors::isRunning()));
#endif

  if (Clock::getRuntime() < MINRUNTIME)
  {
    if (!drawnStatusBlank)
    {

      tft.fillRect(75, 200, 170, 35, BLACK);
      Motors::setRunning(0);
      drawnStatus = 0;
      drawnStatusBlank = 1;
#ifdef DEBUG
      Serial.println("drawnStatusBlank set to 1 ....");
#endif
    }
    return;
  }

  else if (drawnStatusBlank || (drawnStatus != Motors::isRunning()))
  {

    tft.fillRect(140, 190, 225, 75, BLACK);
    tft.setTextSize(1);
    tft.setCursor(0, 0);

    tft.setFont(&FreeSans18pt7b);

    if (Motors::isRunning())
    {

      tft.setCursor(170, 230);
      tft.setTextColor(RED);
      tft.print("RUNNING");
      tft.setCursor(172, 255);
      tft.setTextColor(WHITE);
      tft.setFont(&FreeSans9pt7b);
      tft.print("Press button to stop");
      tft.setFont(&FreeSans18pt7b);
    }

    else
    {

      tft.setCursor(190, 230);
      tft.setTextColor(GREEN);
      tft.print("READY");
      tft.setCursor(140, 255);
      tft.setTextColor(WHITE);
      tft.setFont(&FreeSans9pt7b);
      tft.print("Long press button to start");
      tft.setFont(&FreeSans18pt7b);
    }
    drawnStatus = Motors::isRunning();
    drawnStatusBlank = 0;
#ifdef DEBUG
    Serial.println("Motor is running: ");
    Serial.println(Motors::isRunning());
#endif
  }
}
// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File &f)
{
  uint16_t result; // read little-endian
  f.read(&result, sizeof(result));
  return result;
}

uint32_t read32(File &f)
{
  uint32_t result;
  f.read(&result, sizeof(result));
  return result;
}
uint8_t Display::showBMP(char *nm, int x, int y)
{
  File bmpFile;
  int bmpWidth, bmpHeight;         // W+H in pixels
  uint8_t bmpDepth;                // Bit depth (currently must be 24, 16, 8, 4, 1)
  uint32_t bmpImageoffset;         // Start of image data in file
  uint32_t rowSize;                // Not always = bmpWidth; may have padding
  uint8_t sdbuffer[3 * BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
  uint16_t lcdbuffer[(1 << PALETTEDEPTH) + BUFFPIXEL], *palette = NULL;
  uint8_t bitmask, bitshift;
  boolean flip = true; // BMP is stored bottom-to-top
  int w, h, row, col, lcdbufsiz = (1 << PALETTEDEPTH) + BUFFPIXEL, buffidx;
  uint32_t pos;          // seek position
  boolean is565 = false; //

  uint16_t bmpID;
  uint16_t n; // blocks read
  uint8_t ret;

  if ((x >= tft.width()) || (y >= tft.height()))
    return 1; // off screen

  bmpFile = SD.open(nm);            // Parse BMP header
  bmpID = read16(bmpFile);          // BMP signature
  (void)read32(bmpFile);            // Read & ignore file size
  (void)read32(bmpFile);            // Read & ignore creator bytes
  bmpImageoffset = read32(bmpFile); // Start of image data
  (void)read32(bmpFile);            // Read & ignore DIB header size
  bmpWidth = read32(bmpFile);
  bmpHeight = read32(bmpFile);
  n = read16(bmpFile);        // # planes -- must be '1'
  bmpDepth = read16(bmpFile); // bits per pixel
  pos = read32(bmpFile);      // format
  if (bmpID != 0x4D42)
    ret = 2; // bad ID
  else if (n != 1)
    ret = 3; // too many planes
  else if (pos != 0 && pos != 3)
    ret = 4; // format: 0 = uncompressed, 3 = 565
  else if (bmpDepth < 16 && bmpDepth > PALETTEDEPTH)
    ret = 5; // palette
  else
  {
    bool first = true;
    is565 = (pos == 3); // ?already in 16-bit format
    // BMP rows are padded (if needed) to 4-byte boundary
    rowSize = (bmpWidth * bmpDepth / 8 + 3) & ~3;
    if (bmpHeight < 0)
    { // If negative, image is in top-down order.
      bmpHeight = -bmpHeight;
      flip = false;
    }

    w = bmpWidth;
    h = bmpHeight;
    if ((x + w) >= tft.width()) // Crop area to be loaded
      w = tft.width() - x;
    if ((y + h) >= tft.height()) //
      h = tft.height() - y;

    if (bmpDepth <= PALETTEDEPTH)
    { // these modes have separate palette
      // bmpFile.seek(BMPIMAGEOFFSET); //palette is always @ 54
      bmpFile.seek(bmpImageoffset - (4 << bmpDepth)); // 54 for regular, diff for colorsimportant
      bitmask = 0xFF;
      if (bmpDepth < 8)
        bitmask >>= bmpDepth;
      bitshift = 8 - bmpDepth;
      n = 1 << bmpDepth;
      lcdbufsiz -= n;
      palette = lcdbuffer + lcdbufsiz;
      for (col = 0; col < n; col++)
      {
        pos = read32(bmpFile); // map palette to 5-6-5
        palette[col] = ((pos & 0x0000F8) >> 3) | ((pos & 0x00FC00) >> 5) | ((pos & 0xF80000) >> 8);
      }
    }

    // Set TFT address window to clipped image bounds
    tft.setAddrWindow(x, y, x + w - 1, y + h - 1);
    for (row = 0; row < h; row++)
    { // For each scanline...
      // Seek to start of scan line.  It might seem labor-
      // intensive to be doing this on every line, but this
      // method covers a lot of gritty details like cropping
      // and scanline padding.  Also, the seek only takes
      // place if the file position actually needs to change
      // (avoids a lot of cluster math in SD library).
      uint8_t r, g, b, *sdptr;
      int lcdidx, lcdleft;
      if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
        pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
      else // Bitmap is stored top-to-bottom
        pos = bmpImageoffset + row * rowSize;
      if (bmpFile.position() != pos)
      { // Need seek?
        bmpFile.seek(pos);
        buffidx = sizeof(sdbuffer); // Force buffer reload
      }

      for (col = 0; col < w;)
      { // pixels in row
        lcdleft = w - col;
        if (lcdleft > lcdbufsiz)
          lcdleft = lcdbufsiz;
        for (lcdidx = 0; lcdidx < lcdleft; lcdidx++)
        { // buffer at a time
          uint16_t color;
          // Time to read more pixel data?
          if (buffidx >= sizeof(sdbuffer))
          { // Indeed
            bmpFile.read(sdbuffer, sizeof(sdbuffer));
            buffidx = 0; // Set index to beginning
            r = 0;
          }
          switch (bmpDepth)
          { // Convert pixel from BMP to TFT format
          case 32:
          case 24:
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            if (bmpDepth == 32)
              buffidx++; // ignore ALPHA
            color = tft.color565(r, g, b);
            break;
          case 16:
            b = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            if (is565)
              color = (r << 8) | (b);
            else
              color = (r << 9) | ((b & 0xE0) << 1) | (b & 0x1F);
            break;
          case 1:
          case 4:
          case 8:
            if (r == 0)
              b = sdbuffer[buffidx++], r = 8;
            color = palette[(b >> bitshift) & bitmask];
            r -= bmpDepth;
            b <<= bmpDepth;
            break;
          }
          lcdbuffer[lcdidx] = color;
        }
        tft.pushColors(lcdbuffer, lcdidx, first);
        first = false;
        col += lcdidx;
      }                                                         // end cols
    }                                                           // end rows
    tft.setAddrWindow(0, 0, tft.width() - 1, tft.height() - 1); // restore full screen
    ret = 0;                                                    // good render
  }
  bmpFile.close();
  return (ret);
}