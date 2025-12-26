#include "weather.h"

Weather::Weather() {
}

void Weather::setTemp(int i) {
  temp = i;
}

void Weather::setIcon(int i) {
  icon = i;
}

void Weather::setWind(float f) {
  wind = f;
}

String Weather::getTempStr() {
  String t = "";
  if (temp > 0) {
    t = "+" + String(temp) + "°C";
  }
  else {
    t = "-" + String(temp) + "°C";
  }
  return t;
}