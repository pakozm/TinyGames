#include <Arduino.h>
#include "sprites.h"

PROGMEM const byte ProgMemArrays::boat[18] = {
  B00001100,
  B00000000,
  B00000001,
  B11100000,
  B00000000,
  B00111100,
  B00000000,
  B00001111,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11101111,
  B11111111,
  B11111100,
  B11111111,
  B11111111,
  B10000000
}; // W=20, h=7
  
PROGMEM const byte ProgMemArrays::chopper[25] = {
  B11111111,
  B11000000,
  B00000000,
  B11000000,
  B00000000,
  B00011110,
  B00011111,
  B11110001,
  B11100000,
  B00010000,
  B00111111,
  B00000011,
  B10000011,
  B11111111,
  B11111100,
  B00011111,
  B11111111,
  B11000000,
  B11111111,
  B11111000,
  B00000111,
  B11111111,
  B00000000,
  B00110000,
  B01100000
}; // w=20, h=10
  
PROGMEM const byte ProgMemArrays::coco[2] = {
  B0101111,
  B1010000
}; // w=3, h=4

PROGMEM const byte ProgMemArrays::shark[15] = {
  B00000000,
  B00001100,
  B00000000,
  B01111100,
  B00000111,
  B11110000,
  B01111111,
  B10000111,
  B11111100,
  B00011111,
  B11110001,
  B10001111,
  B11111111,
  B00001111,
  B11111000
}; // w=15, h=8

PROGMEM const byte ProgMemArrays::soldier[13] = {
  B00011100,
  B00011111,
  B00011000,
  B11011011,
  B10111111,
  B01111110,
  B11101101,
  B11111100,
  B00111000,
  B00110110,
  B00111011,
  B10011000,
  B11011000
}; // w=9, h=11

ByteArrayInProgMem<18> ProgMemByteArrays::boat(ProgMemArrays::boat);
ByteArrayInProgMem<25> ProgMemByteArrays::chopper(ProgMemArrays::chopper);
ByteArrayInProgMem<2>  ProgMemByteArrays::coco(ProgMemArrays::coco);
ByteArrayInProgMem<15> ProgMemByteArrays::shark(ProgMemArrays::shark);
ByteArrayInProgMem<13> ProgMemByteArrays::soldier(ProgMemArrays::soldier);
