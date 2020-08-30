// galaxy-man Kudos goes to

// mrfaptastic https://github.com/mrfaptastic
// Brian Lough https://github.com/witnessmenow
// Marc Merlin https://github.com/marcmerlin

// and for the FM612x reset routine
// Bob Davis https://github.com/bobdavis321
// Bob Davis https://bobdavis321.blogspot.com/search?q=matrix

#include <Arduino.h>
// mrfaptastic library 
#include <ESP32-RGB64x32MatrixPanel-I2S-DMA.h>

RGB64x32MatrixPanel_I2S_DMA matrix;

/////////////////////////////////////////////////////////////////
// Reset Panel
// Bob Davis https://bobdavis321.blogspot.com/search?q=matrix
// This needs to be near the top of the code and run before begin in setup.
// 
//
// Change these to whatever suits
// recommended settings and patches are by
//
// pinout for ESP38 38pin module
// http://arduinoinfo.mywikis.net/wiki/Esp32#KS0413_keyestudio_ESP32_Core_Board
//
// Yes this code could be shorter but who cares, it works :-)

/////////////////////////////////////////////////////////////////
// R1 | G1
// B1 | GND
// R2 | G2
// B2 | E
// A  | B
// C  | D
// CLK| LAT
// OE | GND

/*--------------------- RGB DISPLAY PINS -------------------------*/
#define R1 25  
#define G1 26  
#define BL1 27
#define R2 5
#define G2 19
#define BL2 23

#define CH_A 12
#define CH_B 16
#define CH_C 17
#define CH_D 18
#define CH_E -1 // assign to pin 14 if using more than two panels

#define CLK 15
#define LAT 32
#define OE 33

/////////////////////////////////////////////////////////////////
// how many pixels wide if you chain panels
// 4 panels of 64x32 is 256 wide.
int MaxLed = MATRIX_WIDTH;

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
/////////////////////////////////////////////////////////////////

void setup(){

resetPanel(); // do this before matrix.begin

// If you experience ghosting, you will need to reduce the brightness level, not all RGB Matrix
// Panels are the same - some seem to display ghosting artefacts at lower brightness levels.
// In the setup() function do something like:
// If using multi panels watch your power levels when running full brightness.

matrix.setPanelBrightness(10); // SETS THE BRIGHTNESS HERE. 60 OR LOWER IDEAL.
matrix.begin(R1, G1, BL1, R2, G2, BL2, CH_A, CH_B, CH_C, CH_D, CH_E, LAT, OE, CLK); // setup the LED matrix
matrix.setTextColor(matrix.Color(96, 0, 96)); // r,g,b
matrix.setCursor(1, 15);
matrix.println("Hello World");

}

void loop(){

}
