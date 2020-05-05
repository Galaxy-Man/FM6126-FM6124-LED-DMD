// Heavy work in progress
// this is used for a test ground but does work
// https://github.com/VGottselig/ESP32-RGB-Matrix-Display/issues/2
// http://bobdavis321.blogspot.com/2019/02/p3-64x32-hub75e-led-matrix-panels-with.html?m=1
#include <arduino.h>

#include <SPI.h>
// #include <Wire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Adafruit_GFX.h"
#include "ESP32RGBmatrixPanel.h"

#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMono18pt7b.h>
#include <Fonts/FreeMono24pt7b.h>
#include <Fonts/FreeSansOblique9pt7b.h>
#include <Fonts/TomThumb.h>
#include <Fonts/RobotoMono_Thin7pt7b.h>
#include <GFX_fonts/GFX_fonts/Font5x7Fixed.h>
#include <GFX_fonts/GFX_fonts/Font4x7Fixed.h>
#include <GFX_fonts/GFX_fonts/Font_5x7_practical8pt7b.h>

#include <fix_fft.h>

#include <Preferences.h>

Preferences preferences;

unsigned int counter;

// RGB LED Matrix panel
// P3-6432-2121-16S-D1.0
// 6432 = 64 pixels x 32 pixels
// 16S = 1/16 scan rate
// P3 = 3mm pitch centre to centre of LED
// 2121 = LED size 2.1mm x 2.1mm
//
//
//  R1 | G1
//  B1 | GND
//  R2 | G2
//  B2 | E
//  A  | B
//  C  | D
//  CLK| LAT
//  OE | GND

// Default connection
// based on the DOIT ESP32 dev kit
// this articl sayd do nto use GPIO 2,4,15 when using wifi.
// https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/

/* ESP32 Pin out (38 pin)
GPIO	Input	      Output	  Notes
0	    pulled up	  OK	      outputs PWM signal at boot
1	    TX pin	    OK	      debug output at boot
2	    OK	        OK	      connected to on-board LED
3	    OK	        RX pin	  HIGH at boot
4.	  OK	        OK	
5.	  OK	        OK	      outputs PWM signal at boot
6	    x	          x	        connected to the integrated SPI flash
7	    x	          x	        connected to the integrated SPI flash
8	    x	          x	        connected to the integrated SPI flash
9	    x	          x	        connected to the integrated SPI flash
10	  x	          x	        connected to the integrated SPI flash
11	  x	          x	        connected to the integrated SPI flash
12	  OK	        OK	      boot fail if pulled high
13	  OK	        OK	
14.	  OK	        OK	      outputs PWM signal at boot
15	  OK	        OK	      outputs PWM signal at boot
16.	  OK	        OK	
17.	  OK	        OK	
18.	  OK	        OK	
19.	  OK	        OK	
21.	  OK	        OK	
22	  OK	        OK	
23.	  OK	        OK	
25.	  OK	        OK	
26.	  OK	        OK	
27.	  OK	        OK	
32	  OK	        OK	
33.	  OK	        OK	
34	  OK		                input only
35	  OK		                input only
36	  OK		                input only
39	  OK		                input only

*/

/*
Changing Serial pins.
https://www.youtube.com/watch?v=GwShqW39jlE

You can reassign UART pins to any other that is not in use by your application.

C:\Users\Ian\Documents\Arduino\hardware\espressif\esp32\cores\esp32

edit hardwareSerial.cpp

 */

// const uint8 OE = 22;  // purple 15
const uint8 OE = 15; // purple

// const uint8 LAT = 32;  // grey 4
const uint8 LAT = 4; // grey

const uint8 CLK = 23; // white

const uint8 R1 = 16; // red
const uint8 G1 = 17; // green
const uint8 BL1 = 5; // blue

const uint8 R2 = 18;  // red
const uint8 G2 = 19;  // green
const uint8 BL2 = 21; // blue

const uint8 CH_A = 14; // brown
const uint8 CH_B = 27; // orange
const uint8 CH_C = 26; // yellow
const uint8 CH_D = 25; // purple
const uint8 CH_E = 33; // grey

ESP32RGBmatrixPanel matrix(OE, CLK, LAT, R1, G1, BL1, R2, G2, BL2, CH_A, CH_B, CH_C, CH_D);

//ESP32RGBmatrixPanel matrix; // use for default settings

// how many pixels wide if you chain panels
// 4 panels of 64x32 is 256 wide.
int MaxLed = 64;

//#define GPIO_PIN_OE_PIN      2
//#define GPIO_PIN_CLK_PIN     14
//#define GPIO_PIN_LATCH_PIN   22

//#define  GPIO_PIN_R1_PIN      13
//#define  GPIO_PIN_G1_PIN      25
//#define  GPIO_PIN_B1_PIN      26

//#define  GPIO_PIN_R2_PIN      27
//#define  GPIO_PIN_G2_PIN      12
//#define  GPIO_PIN_B2_PIN      4

//#define  ADDX_PIN_A   19
//#define  ADDX_PIN_B   23
//#define  ADDX_PIN_C   18
//#define  ADDX_PIN_D   5
//#define  ADDX_PIN_E   15

int C12[16] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int C13[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};

void resetPanel()
{
  pinMode(CLK, OUTPUT);
  pinMode(LAT, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(R1, OUTPUT);
  pinMode(G1, OUTPUT);
  pinMode(BL1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(G2, OUTPUT);
  pinMode(BL2, OUTPUT);
  pinMode(CH_A, OUTPUT);
  pinMode(CH_B, OUTPUT);
  pinMode(CH_C, OUTPUT);
  pinMode(CH_D, OUTPUT);
  pinMode(CH_E, OUTPUT);

  // Send Data to control register 11
  digitalWrite(OE, HIGH); // Display reset
  digitalWrite(LAT, LOW);
  digitalWrite(CLK, LOW);
  for (int l = 0; l < MaxLed; l++)
  {
    int y = l % 16;
    digitalWrite(R1, LOW);
    digitalWrite(G1, LOW);
    digitalWrite(BL1, LOW);
    digitalWrite(R2, LOW);
    digitalWrite(G2, LOW);
    digitalWrite(BL2, LOW);
    if (C12[y] == 1)
    {
      digitalWrite(R1, HIGH);
      digitalWrite(G1, HIGH);
      digitalWrite(BL1, HIGH);
      digitalWrite(R2, HIGH);
      digitalWrite(G2, HIGH);
      digitalWrite(BL2, HIGH);
    }
    if (l > MaxLed - 12)
    {
      digitalWrite(LAT, HIGH);
    }
    else
    {
      digitalWrite(LAT, LOW);
    }
    digitalWrite(CLK, HIGH);
    digitalWrite(CLK, LOW);
  }
  digitalWrite(LAT, LOW);
  digitalWrite(CLK, LOW);
  // Send Data to control register 12
  for (int l = 0; l < MaxLed; l++)
  {
    int y = l % 16;
    digitalWrite(R1, LOW);
    digitalWrite(G1, LOW);
    digitalWrite(BL1, LOW);
    digitalWrite(R2, LOW);
    digitalWrite(G2, LOW);
    digitalWrite(BL2, LOW);
    if (C13[y] == 1)
    {
      digitalWrite(R1, HIGH);
      digitalWrite(G1, HIGH);
      digitalWrite(BL1, HIGH);
      digitalWrite(R2, HIGH);
      digitalWrite(G2, HIGH);
      digitalWrite(BL2, HIGH);
    }
    if (l > MaxLed - 13)
    {
      digitalWrite(LAT, HIGH);
    }
    else
    {
      digitalWrite(LAT, LOW);
    }
    digitalWrite(CLK, HIGH);
    digitalWrite(CLK, LOW);
  }
  digitalWrite(LAT, LOW);
  digitalWrite(CLK, LOW);
}

// End of default setup for RGB Matrix 64x32 panel
///////////////////////////////////////////////////////////////////////////////////////

// these DEC need to be converted to RGB (0-15,0-15,0-15)
int spectColor[16] = {
    2016,  // 0,7,224,
    10208, // 0,39,224,
    20448, // 0,79,224,
    28640, // 0,111,224,
    38880, // 0,151,224,
    47072, // 0,183,224,
    57312, // 0,223,224,
    65504, // 0,255,224, 00FFE0
    65504, // 0,255,224, 00FFE0
    65216, // 0,254,192,
    64928, // 0,253,160,
    64640, // 0,252,128,
    64352, // 0,251,96,
    64064, // 0,250,64,
    63776, // 0,249,32,
    63488  // 0,248,0, 00F800
};

///////////////////////////////////////////////////////////////////////////////////////
int FrameNow = 0;

int invader1Close[8][12] = {
    {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0},
};
int invader1Open[8][12] = {
    {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
};

int invader2Close[8][12] = {
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
};
int invader2Open[8][12] = {
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
};

int invader3Close[8][12] = {
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 0, 1, 1, 0, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 1, 0, 0, 1, 0, 0},
    {0, 1, 0, 1, 1, 0, 1, 0},
    {1, 0, 1, 0, 0, 1, 0, 1},
};
int invader3Open[8][12] = {
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 0, 1, 1, 0, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {0, 1, 0, 1, 1, 0, 1, 0},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {0, 1, 0, 0, 0, 0, 1, 0},
};

///////////////////////////////////////////////////////////////////////////////////////
// https://www.youtube.com/watch?v=NUpaGlB6DAI
// I used an simple Electret Microphone with MAX4466 aplifier from adafruit.
// And an simpleFFT library to get the spectrum. However this didn't work accurate at all,
// but it did look good and react to music.

int static i = 0;
int val;
char im[128];
char data[128];
double data_avgs[64];
double soundAvg = 0;

void collectAudioSample()
{
  for (i = 0; i < 128; i++)
  {
    val = analogRead(A5); // may have to use a different pin for microphone
    data[i] = val / 4;
    im[i] = 0;
  }
  fix_fft(data, im, 7, 0);
  for (i = 0; i < 64; i++)
  {
    data[i] = sqrt(data[i] * data[i] + im[i] * im[i]);
  }
  for (i = 0; i < 64; i++)
  {
    int g = data[i];
    data_avgs[i] = (data_avgs[i] * 0.5) + (g * 0.5);
  }
}
///////////////////////////////////////////////////////////////////////////////////
void spectrum()
{
  for (int y = 0; y < 16; y++)
  {
    for (int x = 1; x <= 64; x++)
    {
      int z = data_avgs[x] + data_avgs[x + 1];
      if (z > y && (x & 0x01) == 0)
        matrix.drawPixel((x / 2), 15 - y, spectColor[y]);
    }
  }
}
///////////////////////////////////////////////////////////////////////////////////////
void gfxScrollMessageDemo()
{

  int num1;
  int num2;
  auto msg1 = "Ianandra Medlock: via Messages @ 11:47 ";
  auto msg2 = "'The quick brown fox jumps over the lazy dog'";

  // Size in pixel width 1 = 6 * 1, 2 = 6 * 2, 3 = 6 * 3, 4 = 6 * 4
  // fontWidth = 18 would be a setTextSize of 3 which is 18 / 6 = 6
  // This is calc is for the default font width of 6 pixels
  // or use the font width if not using default font.
  int fontWidth = 4; // 6, 12, 18 or 24

  //Serial.print(msg1);
  num1 = strlen(msg1);
  //Serial.print("String length is: ");
  num1 = (num1 * fontWidth);
  //Serial.println(num1);

  //Serial.print(msg2);
  num2 = strlen(msg2);
  //Serial.print("String length is: ");
  num2 = (num2 * fontWidth);
  //Serial.println(num2);

  // identify which is bigger num1 or num2
  // use this as num3

  int pos = 64;

  // matrix.setFont(&Font4x7Fixed);
  matrix.setFont(&Font4x7Fixed);
  matrix.setTextWrap(false);

  matrix.setTextSize(1); // default 1 = // font size is 6 x 8 pixels
  // matrix.setTextSize(2); // default 1 = // font size is 12 x 16 pixels
  // matrix.setTextSize(3); // default 1 = // font size is 18 x 24 pixels
  // matrix.setTextSize(4); // default 1 = // font size is 24 x 32 pixels

  while (pos > -num2)
  {
    matrix.black();
    //    matrix.setFont(&Font4x7Fixed);
    matrix.setCursor(pos, 8); // x , y

    matrix.setTextColor(0x000F);
    matrix.print(msg1);

    //    matrix.setFont(&Font4x7Fixed);
    matrix.setCursor(pos, 16); // x , y

    matrix.setTextColor(0x00FF);
    matrix.print(msg2);

    //matrix.setFont(&Font4x7Fixed);
    matrix.setTextColor(0x00FF);
    matrix.setCursor(64 - 20, 30);
    matrix.println(counter);
    //matrix.setFont();

    pos -= 1;
    vTaskDelay(20); // default 10
  }
}

///////////////////////////////////////////////////////////////////////////////////
// https://www.youtube.com/watch?v=NUpaGlB6DAI
//
void invaderDemo()
{

  matrix.black();
  int x, y = 0;

  if (FrameNow >= 158)
  {
    FrameNow = 0;
  }
  else
  {
    FrameNow++;
  }

  for (x = 0; x < 12; x++) // size of data set
  {

    for (y = 0; y < 8; y++) // size of data set
    {
      if ((FrameNow & 0x01) == 0)
      {
        if (invader1Close[y][x] == 1)
          matrix.drawPixel(x + FrameNow - 94, y + 16, 0x000F);
        if (invader2Close[y][x] == 1)
          matrix.drawPixel(x + FrameNow - 82, y, 0x00FF); //
        if (invader3Close[y][x] == 1)
          matrix.drawPixel(x + FrameNow - 70, y + 16, 0x0ff0);

        if (invader1Close[y][x] == 1)
          matrix.drawPixel(x + FrameNow - 62, y, 0x000F); //
        if (invader2Close[y][x] == 1)
          matrix.drawPixel(x + FrameNow - 50, y + 16, 0x00FF);
        if (invader3Close[y][x] == 1)
          matrix.drawPixel(x + FrameNow - 34, y + (FrameNow / 4), 0x0ff0); //

        if (invader1Close[y][x] == 1)
          matrix.drawPixel(x + FrameNow - 30, y + 16, 0x000F);
        if (invader2Close[y][x] == 1)
          matrix.drawPixel(x + FrameNow - 28, y, 0x00FF); //
        if (invader3Close[y][x] == 1)
          matrix.drawPixel(x + FrameNow - 14, y + 16, 0x0ff0);
      }
      else
      {
        if (invader1Open[y][x] == 1)
          matrix.drawPixel(x + FrameNow - 94, y + 16, 0x000F);
        if (invader2Open[y][x] == 1)
          matrix.drawPixel(x + FrameNow - 82, y, 0x00FF);
        if (invader3Open[y][x] == 1)
          matrix.drawPixel(x + FrameNow - 70, y + 16, 0x0ff0);

        if (invader1Open[y][x] == 1)
          matrix.drawPixel(x + FrameNow - 62, y, 0x000F);
        if (invader2Open[y][x] == 1)
          matrix.drawPixel(x + FrameNow - 50, y + 16, 0x00FF);
        if (invader3Open[y][x] == 1)
          matrix.drawPixel(x + FrameNow - 34, y + (FrameNow / 4), 0x0ff0);

        if (invader1Open[y][x] == 1)
          matrix.drawPixel(x + FrameNow - 30, y + 16, 0x000F);
        if (invader2Open[y][x] == 1)
          matrix.drawPixel(x + FrameNow - 28, y, 0x00FF);
        if (invader3Open[y][x] == 1)
          matrix.drawPixel(x + FrameNow - 14, y + 16, 0x0ff0);
      }
    }
  }
}

//runs faster then default void loop(). why? runs on other core?
void loop2_task(void *pvParameter)
{
  while (true)
  {
    // Void Loop main code goes here
    // gfxScrollMessageDemo();
    invaderDemo();
    // spectrum();
    vTaskDelay(100);
  }
}

/* create a hardware timer */
hw_timer_t *displayUpdateTimer = NULL;

void IRAM_ATTR onDisplayUpdate()
{
  matrix.update();
}

// Default setup

void setup()
{
  Serial.begin(115200UL);
  resetPanel();

  matrix.setBrightness(1);

  ///////////////////////////////////////////////////////////////////////////////////
  // reboot counter
  preferences.begin("my-app", false);

  // Remove all preferences under the opened namespace
  //preferences.clear();

  // Or remove the counter key only
  //preferences.remove("counter");

  // Get the counter value, if the key does not exist, return a default value of 0
  // Note: Key name is limited to 15 chars.
  counter = preferences.getUInt("counter", 0);

  // Increase counter by 1
  counter++;

  // Print the counter to Serial Monitor
  Serial.printf("Current counter value: %u\n", counter);

  // Store the counter to the Preferences
  // preferences.putUInt("counter", 0); // uncomment to reset counter
  preferences.putUInt("counter", counter);

  // Close the Preferences
  preferences.end();
  ///////////////////////////////////////////////////////////////////////////////////

  xTaskCreate(&loop2_task, "loop2_task", 2048, NULL, 5, NULL);
  /* 1 tick take 1/(80MHZ/80) = 1us so we set divider 80 and count up */
  displayUpdateTimer = timerBegin(0, 80, true);

  /* Attach onTimer function to our timer */
  timerAttachInterrupt(displayUpdateTimer, &onDisplayUpdate, true);
  timerAlarmWrite(displayUpdateTimer, 2, true);
  timerAlarmEnable(displayUpdateTimer);

  Serial.println("OK");
}

void loop()
{
}
