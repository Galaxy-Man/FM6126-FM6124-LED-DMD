/*
Author:	Vitali Gottselig
https://github.com/
*/

#ifndef ESP32RGB_MATRIX_PANEL_H
#define ESP32RGB_MATRIX_PANEL_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Adafruit_GFX.h"
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef short int16_t;
typedef unsigned short uint16_t;

//G1	R1 |
//GND	B1 |
//G2	R2 |
//GND	B2  |
//B		A   |
//D		C  |
//LAT	CLK|
//GND	OE |
//OE connected with LAT

struct color
{
public:
	uint8 r;
	uint8 g;
	uint8 b;
};

#define COLUMNS 64
#define ROWS 32
#define layers 256
#define colorDepthPerChannel 4 //(2^colorDepthPerChannel)^3  Colors. 4096@4 bit.  262144 @ 6 bit.

//0b0000RRRRGGGGBBBB
#define rmask 0b0000000000001111
#define gmask 0b0000000011110000
#define bmask 0b0000111100000000

class Colors
{
public:
	static const int16_t BLACK = 0;
	static const int16_t RED = rmask;
	static const int16_t GREEN = gmask;
	static const int16_t BLUE = bmask;
	static const int16_t WHITE = rmask | gmask | bmask;
	static const int16_t CYAN = gmask | bmask;
	static const int16_t YELLOW = rmask | gmask;
	static const int16_t MAGENTA = rmask | bmask;
};

class ESP32RGBmatrixPanel : public Adafruit_GFX
{
public:
	/*variable pin setup*/
	ESP32RGBmatrixPanel(uint8 oe, uint8 clk, uint8 lat, uint8 r1, uint8 g1, uint8 b1, uint8 r2, uint8 g2, uint8 b2, uint8 a, uint8 b, uint8 c, uint8 d);

	/*
	uint8 OE = 23;
	uint8 CLK = 22;
	uint8 LAT = 03;
	uint8 CH_A = 21;
	uint8 CH_B = 19;
	uint8 CH_C = 18;
	uint8 CH_D = 5;
	uint8 R1 = 17;
	uint8 G1 = 16;
	uint8 BL1 = 4;
	uint8 R2 = 0;
	uint8 G2 = 2;
	uint8 BL2 = 15;
	*/
	ESP32RGBmatrixPanel();
	void drawPixel(int16_t x, int16_t y, uint16_t c);
	void drawPixel(int16_t x, int16_t y, uint8 r, uint8 g, uint8 b);

	/* reset all Pixels to black */
	void black();
	/*  call every 1-3µs! */
	void IRAM_ATTR update();
	/*  from 0 = off to 10 = max */
	void setBrightness(byte brightness);
	/*  returns Color for call Adafruit_GFS methods */
	static int16_t AdafruitColor(uint8 r, uint8 g, uint8 b);

	void drawBitmap(String *bytes);

private:
	volatile byte loopNr = 0;
	volatile byte loopNrOn = 0;
	void IRAM_ATTR initGPIO();
	void IRAM_ATTR drawRow();
	void IRAM_ATTR on();

// RGB LED Matrix panel
// P3-6432-2121-16S-D1.0
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
	uint8 OE = 15; // purple
	uint8 CLK = 2; // white
	uint8 LAT = 4; // grey

	uint8 R1 = 16; // red
	uint8 G1 = 17; // green
	uint8 BL1 = 5; // blue

	uint8 R2 = 18;  // red
	uint8 G2 = 19;  // green
	uint8 BL2 = 21; // blue

	uint8 CH_A = 14; // brown
	uint8 CH_B = 27; // orange
	uint8 CH_C = 26; // yellow
	uint8 CH_D = 25; // purple
	uint8 CH_E = 33; // grey

	color pixels[ROWS][COLUMNS];

	uint32_t gpio;

	uint8 layerStep = 256 / (1 << colorDepthPerChannel);
	uint8 layer = layerStep - 1;
	uint8 row = 0;
	uint8 column = 0;
	uint8 yS;
};

#endif
