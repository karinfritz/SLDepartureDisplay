#include "Arduino.h" 
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Adafruit_GFX.h>
#include <Fonts/TomThumb.h>
#include "departures.h"
#include "icons.h"

class Display {
private:
  MatrixPanel_I2S_DMA* _display;
  int _black;
  int _white;
  int _red;
  int _green;
  int _blue;
  int _gray;
  int _yeish;
  int _darkyellow;
  void prepare();
  void drawIcon5(int x, int y, const uint8_t* icon, uint16_t color, int i);

public:
  // functions
  Display();
  void init();
  void start();
  void run(int cursor);
  void showNext(String depStr, String depStr2, int cursor);
  void drawDeps(Departure deps[], int count);
  void drawDeps2(String deps, int count);
  void drawText(String text);
  void drawTimeAndWeather(String timeStr, String tempStr, int i);
};