#include "departures.h"

Departure::Departure() {
}

void Departure::setId(String i) {
  id = i;
}

void Departure::setArrival(String arr) {
  arrival = arr;
}

void Departure::setDirection(String dir) {
  direction = dir;
}

void Departure::setScheduled(String s) {
  scheduled = s;
}

void Departure::setRemaining(int i) {
  remaining = i;
}

void Departure::updateArrival() {
  String newArrival = "";
  if (remaining < 1) {
    arrival = "Nu";
  }
  else {
    arrival = String(remaining) + " min";
  }
}


