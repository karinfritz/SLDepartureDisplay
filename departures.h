#ifndef departure_h
#define departure_h
#include "Arduino.h" 

class Departure {
public:
  Departure();

  String id = "";
  String arrival = "";
  String direction = "";

  // functions
  void setDirection(String s);
  void setArrival(String s);
  void setId(String s);

  
private:

};
#endif