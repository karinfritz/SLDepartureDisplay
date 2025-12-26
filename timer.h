#include <time.h>

void setupTime() {
  configTzTime(
    "CET-1CEST,M3.5.0/2,M10.5.0/3",  // Europe/Stockholm
    "pool.ntp.org",
    "time.nist.gov"
  );

  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n⏰ Time synced");
}

unsigned long parseISO(const char* iso) {
  struct tm t = {};
  strptime(iso, "%Y-%m-%dT%H:%M:%S", &t);
  return mktime(&t);
}

String getCurrTime() {
  time_t now;
  time(&now);
  struct tm t;
  localtime_r(&now, &t);

  char buf[6];
  strftime(buf, sizeof(buf), "%H:%M", &t);
  return String(buf);
}

int getRemainingMinutes(String scheduledDeparture) {
  time_t now;
  time(&now);   // SEKUNDER (epoch)

  long diff = parseISO(scheduledDeparture.c_str()) - now;

  int min = diff / 60;
  int sec = diff % 60;

  return min;
}