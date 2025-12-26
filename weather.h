#ifndef weather_h
#define weather_h
#include "Arduino.h" 

class Weather {
public:
  Weather();

  int temp = 0;
  String tempStr = "";
  float wind = 0.0;
  String windStr = "";
  int icon = 0;

  // functions
  void setTemp(int i);
  void setWind(float f);
  void setIcon(int i);
  String getTempStr();

  
private:

};
#endif