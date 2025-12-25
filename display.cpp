#include "display.h"

// Display pin setup
#define R1_PIN 19
#define B1_PIN 18 //G1
#define G1_PIN 13 //B1
#define R2_PIN 5
#define B2_PIN 17 //G2
#define G2_PIN 12 //B2

#define A_PIN 27
#define B_PIN 14
#define C_PIN 4
#define D_PIN 26
#define E_PIN 25  //--> required for 1/32 scan panels, safe to leave defined

#define LAT_PIN 16
#define OE_PIN 15
#define CLK_PIN 2

#define PANEL_RES_X 64  // Width of each panel
#define PANEL_RES_Y 32  // Height of each panel
#define PANEL_CHAIN 1   // Number of chained panels



Display::Display() {
  init();
}

void Display::init() {
   // Initialize the connected PIN between Panel P5 and ESP32.
  HUB75_I2S_CFG::i2s_pins _pins = {R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN};
  delay(10);

  //----------------------------------------Module configuration.
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,   //--> module width.
    PANEL_RES_Y,   //--> module height.
    PANEL_CHAIN,   //--> Chain length.
    _pins          //--> pin mapping.
  );
  delay(10);

  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_20M;  // I2S clock speed

  // mxconfig.driver = HUB75_I2S_CFG::FM6126A;   // for panels using FM6126A chips
  mxconfig.latch_blanking = 6;
  mxconfig.clkphase = false;

  // 👇👇👇 ADDED SCAN CONFIG HERE 👇👇👇
  //mxconfig.scan = HUB75_I2S_CFG::SCAN_16;  // <-- Your panel is 1/16 scan
  
  // 👆👆👆 ADDED SCAN CONFIG HERE 👆👆👆

  //----------------------------------------Display Setup.
  _display = new MatrixPanel_I2S_DMA(mxconfig);
  _black = _display->color565(0, 0, 0);
  _white = _display->color565(255, 255, 255);
  _red = _display->color565(255, 0, 0);
  _green = _display->color565(0, 255, 0);
  _blue = _display->color565(0, 0, 255);
  _gray = _display->color565(200, 200, 200);
  _yeish = _display->color565(200, 200, 170);
}

void Display::start() {
  _display->begin();
  _display->setBrightness8(30); // 0-255
  _display->clearScreen();
  _display->fillScreen(_white);
  delay(1000);
  _display->fillScreen(_red);
  delay(1000);
  _display->fillScreen(_green);
  delay(1000);
  _display->fillScreen(_blue);
  delay(1000);
  _display->clearScreen();
}

void Display::run(int cursor) {
  _display->clearScreen();
  _display->setTextSize(1);
  _display->setTextWrap(false);
  _display->setTextColor(_white);
  _display->setCursor(cursor, 30);
  _display->println("Telefonplan");
}

void Display::showNext(String depStr, String depStr2, int cursor) {
  //_display->clearScreen();
  _display->fillScreen(_black);
  //_display->setFont(&TomThumb);
  _display->setTextSize(1);
  _display->setTextWrap(false);
  _display->setTextColor(_white);
  _display->setCursor(0, 10);
  _display->println(depStr);
  _display->setCursor(cursor, 25);
  _display->println(depStr2);
}

int xPos[] = {1, 9, 45};

void Display::drawDeps(Departure deps[], int count) {
  _display->fillScreen(_black);
  _display->setFont(&TomThumb);
  _display->setTextSize(1);
  _display->setTextWrap(false);
  _display->setTextColor(_yeish);

  for (int i = 0; i < count; i++) {
    int yPos = i * 7 + 8;
    _display->setCursor(xPos[0], yPos);
    _display->println(deps[i].id);
    _display->setCursor(xPos[1], yPos);
    _display->println(deps[i].direction);
    _display->setCursor(xPos[2], yPos);
    _display->println(deps[i].arrival);
  } 
  
}

String temp1[] = {"14", "Morby c.", "nu"};
String temp2[] = {"14", "Fruangen", "2 min"};
String temp3[] = {"14", "Fruangen", "20 min"};
String temp4[] = {"14", "Fruangen", "20 min"};

void Display::drawDeps2(String deps, int count) {
  _display->fillScreen(_black);
  _display->setFont(&TomThumb);
  _display->setTextSize(1);
  _display->setTextWrap(false);
  _display->setTextColor(_white);

  for (int i = 0; i < count; i++) {
    for (int j = 0; j < count; j++) {
      _display->setCursor(xPos[j], i * 7 + 8);
      if (i == 0)
      {
        _display->println(temp1[j]);
      }
      if (i == 1)
      {
        _display->println(temp2[j]);
      }
      if (i == 2)
      {
        _display->println(temp3[j]);
      }
      if (i == 3)
      {
        _display->println(temp4[j]);
      }
    } 
  } 
  
}

  