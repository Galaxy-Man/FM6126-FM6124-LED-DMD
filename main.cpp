// work in progress
// based on 

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Adafruit_GFX.h>
#include "ESP32RGBmatrixPanel.h"

// FM6126 based LED matrix panels with the HUB75e connectors
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
const uint8 OE = 15; // purple
const uint8 CLK = 2; // white
const uint8 LAT = 4; // grey

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
