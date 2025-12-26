#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "display.h"
#include "caCert.h"
#include "departures.h"
#include "secrets.h"
#include "timer.h"
#include "weather.h"

const char*  serverName = "transport.integration.sl.se"; // SL:s API (HTTPS)

String latCoord = "59.300";
String longCoord = "18.003";

String weatherServerName = "https://opendata-download-metfcst.smhi.se/api/category/pmp3g/version/2/geotype/point/lon/" + longCoord + "/lat/" + latCoord + "/data.json";

String siteName = "Telefonplan";
String siteID = "";

const unsigned long API_INTERVAL = 5UL * 60UL * 1000UL; // 5 minuter
unsigned long lastApiCall = -API_INTERVAL;

unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 45UL * 1000UL; // 45 sekunder

int numDeps = 4;

Departure dep1;
Departure dep2;
Departure dep3;
Departure dep4;

Departure depObjList[]= {
    dep1,
    dep2,
    dep3,
    dep4
};

Display display = Display();
Weather weather = Weather();

const word bufferSize = 65535;
char response[bufferSize];

void getWeather() {
  HTTPClient http;
  http.begin(weatherServerName.c_str());
  if (http.GET() != 200){
    Serial.println("Failed to get weather info.");
  };

  DynamicJsonDocument doc(40 * 1024);
  deserializeJson(doc, http.getString());
  http.end();

  JsonArray params = doc["timeSeries"][0]["parameters"];

  for (JsonObject p : params) {
    const char* name = p["name"];
    float v = p["values"][0];
    if (String(name) == "t") {
        int vInt = round(v);
        weather.setTemp(vInt);
    }
    if (String(name) == "ws") {
        weather.setWind(v);
    }
    if (String(name) == "Wsymb2") {
        weather.setIcon(v);
    }
  }
}

String getSiteID(String wantedName) {
    Serial.println("Fetching Site ID for " + siteName);
    String url = "https://transport.integration.sl.se/v1/sites?expand=true";
    String id = httpGETRequest(url, true, false);
    return id;
}

String getDep(String siteId){
    String url = "https://transport.integration.sl.se/v1/sites/9263/departures";
    String deps = httpGETRequest(url, false, true);
    Serial.println("Got deps: " + deps);

    if (deps != "[")
    {
        deps.remove(deps.length() - 1);
        deps += "]";

        // parse and verify object
        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson(doc, deps);

        if (err) {
            Serial.println("JSON parse error");
        }

        JsonArray departures = doc.as<JsonArray>();

        for (size_t i = 0; i < departures.size(); i++) {
            JsonObject dep = departures[i];
            int id = dep["line"]["id"] | -1;
            const char* direction_temp = dep["direction"] | "";
            String direction = String(direction_temp);
            const char* arrival = dep["display"] | "";
            const char* scheduled = dep["scheduled"] | "";
            direction.replace("ö", "o");
            direction.replace("ä", "a");
            direction.replace("å", "a");
            direction.replace("centrum", "c.");
            if (i < numDeps) {
                String idStr = String(id);
                depObjList[i].setId(idStr);
                depObjList[i].setDirection(direction);
                depObjList[i].setArrival(arrival);
                depObjList[i].setScheduled(scheduled);
            } 
        }
    }
    return deps;
}


String httpGETRequest(String url, bool getSiteIdFunc, bool getDepFunc) {
    HTTPClient https;
    WiFiClientSecure client;
    client.setCACert(test_root_ca);
    String departures = "[";

    if (!client.connect(serverName, 443)) {
        Serial.println("Connection failed!");
    }
    else {
        Serial.println("Connected to server!");
        String request_str = "GET " + url + " HTTP/1.0";
        client.println(request_str);
        client.print("Host: ");
        client.println(serverName);
        client.println("Connection: close");
        client.println();

        while (client.connected()) {
            String line = client.readStringUntil('\n');
            if (line == "\r") {
                Serial.println("headers received");
                break;
            }
        }

        char objBuffer[2048];
        size_t objPos = 0;

        int braceDepth = 0;
        bool inObject = false;

        // parse objects
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                bool insideDepartures = false;
                

                if (getDepFunc) {
                    // 1. Leta efter "departures":[
                    static const char target[] = "departures";
                    static int matchIndex = 0;
                    //Serial.println("checking");
                    
                    if (!insideDepartures) {
                        if (c == target[matchIndex]) {
                            matchIndex++;
                            if (matchIndex == strlen(target)) {
                                insideDepartures = true;
                                inObject = false;
                                objPos = 0;
                                Serial.println("▶ Found departures array");
                            }
                        } else {
                            matchIndex = 0;
                        }
                    }
                }  

                if (c == '{' && !inObject) {
                    inObject = true;
                    braceDepth = 1;
                    objPos = 0;
                    objBuffer[objPos++] = c;
                }

                else if (inObject) {
                    if (objPos < sizeof(objBuffer) - 1) {
                        objBuffer[objPos++] = c;
                    }

                    if (c == '{') braceDepth++;
                    if (c == '}') braceDepth--;

                    if (braceDepth == 0) {
                        objBuffer[objPos] = '\0';
                        inObject = false;

                        // parse and verify object
                        StaticJsonDocument<1024> doc;
                        DeserializationError err = deserializeJson(doc, objBuffer);
                        if (err) {
                            Serial.println("JSON parse error: ");
                        }
                        if (getSiteIdFunc) {

                            int id = doc["id"] | -1;
                            const char* name = doc["name"] | "";

                            if (String(name) == siteName) {
                                return String(id);
                            }
                        }

                        if (getDepFunc) {
                            if (String(doc["stop_area"]["type"]) == "METROSTN")
                            {
                                departures += objBuffer;
                                departures += ",";
                            }
                        }  
                        objPos = 0;
                    }
                }
            }
        }
     client.stop();
    }
    return departures;
}

void setup() {
  Serial.begin(115200);
  Serial.println("setup");
  delay(1000);

  display.start();
  delay(1000);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  display.drawText("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("\nWiFi OK!");
  display.drawText("Setup local time...");
  setupTime();
  display.drawText("Loading weather...");
  getWeather();
  display.drawText("Fetching deps...");

  // Hämta plats-id
  //siteID = getSiteID(siteName);
  //Serial.println("Site ID = " + siteID);
}

void loop() {
  unsigned long now = millis();
  String nowStr = getCurrTime();
  String tempStr = weather.getTempStr();

  if (now - lastApiCall >= API_INTERVAL) {
    getWeather();
    lastApiCall = now;
    lastUpdate = now;
    getDep(siteID);
    
    display.drawTimeAndWeather(nowStr, tempStr, weather.icon);
    display.drawDeps(depObjList, numDeps - 1);
  }

  else if (now - lastUpdate >= UPDATE_INTERVAL) {
    
    lastUpdate = now;
    if (depObjList[0].remaining == 0) {
        for (int i = 0; i < numDeps - 1; i++) {
            depObjList[i] = depObjList[i + 1];
        }
        
    }
    for (int i = 0; i < numDeps; i++) {
        int r = getRemainingMinutes(depObjList[i].scheduled);
        depObjList[i].setRemaining(r);
        depObjList[i].updateArrival();
    }
    
    display.drawTimeAndWeather(nowStr, tempStr, weather.icon);
    display.drawDeps(depObjList, numDeps - 1);
  }
  
  delay(1000);
}