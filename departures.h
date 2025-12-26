#ifndef departure_h
#define departure_h
#include "Arduino.h" 

class Departure {
public:
  Departure();

  String id = "";
  String arrival = "";
  String direction = "";
  String scheduled = "";
  int remaining = 0; // remaining minutes to departure

  // functions
  void setDirection(String s);
  void setArrival(String s);
  void setId(String s);
  void setScheduled(String s);
  void setRemaining(int i);
  void updateArrival();

  
private:

};
#endif