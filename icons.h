#pragma once
#include <Arduino.h>

/*
  5x5 Weather Icons for LED matrix
  Each icon = 5 rows, 5 bits per row
*/

/* ========= ICON DATA ========= */

const uint8_t ICON_SUN[5] PROGMEM = {
  0b00100,
  0b10101,
  0b01110,
  0b10101,
  0b00100
};

const uint8_t ICON_CLOUD[5] PROGMEM = {
  0b00000, 
  0b01110, 
  0b11111, 
  0b11111, 
  0b01110
};

const uint8_t ICON_RAIN[5] PROGMEM = {
  0b01110,
  0b11111,
  0b11111,
  0b01010,
  0b10101
};

const uint8_t ICON_SNOW[5] PROGMEM = {
  0b10101,
  0b01110,
  0b11111,
  0b01110,
  0b10101
};

const uint8_t ICON_THUNDER[5] PROGMEM = {
  0b00100,
  0b01100,
  0b00110,
  0b01100,
  0b00100
};

const uint8_t ICON_FOG[5] PROGMEM = {
  0b00000,
  0b11111,
  0b00000,
  0b11111,
  0b00000
};

const uint8_t ICON_CLEAR_NIGHT[5] PROGMEM = {
  0b00111,
  0b01110,
  0b01100,
  0b01110,
  0b00111
};

/* ========= WEATHER SYMBOL MAPPING (SMHI Wsymb2) ========= */

inline const uint8_t* getWeatherIcon5(int wsymb) {
  switch (wsymb) {
    case 1:  return ICON_SUN;        // Clear sky
    case 2:
    case 3:  return ICON_CLEAR_NIGHT;
    case 4:
    case 5:  return ICON_CLOUD;
    case 6:
    case 7:  return ICON_RAIN;
    case 8:  return ICON_SNOW;
    case 9:
    case 10: return ICON_RAIN;
    case 11: return ICON_THUNDER;
    case 12:
    case 13:
    case 14: return ICON_SNOW;
    case 15:
    case 16: return ICON_FOG;
    default: return ICON_CLOUD;
  }
}
