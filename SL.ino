#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "display.h"
#include "caCert.h"
#include "departures.h"
#include "secrets.h"

String siteName = "Telefonplan";
String siteID = "";

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

// SL:s API (HTTPS)
Display display = Display();

const char*  serverName = "transport.integration.sl.se";
const word bufferSize = 65535;
char response[bufferSize];

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
            direction.replace("ö", "o");
            direction.replace("ä", "a");
            direction.replace("å", "a");
            direction.replace("centrum", "c.");
            if (i < numDeps) {
                String idStr = String(id);
                depObjList[i].setId(idStr);
                depObjList[i].setDirection(direction);
                depObjList[i].setArrival(arrival);
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
  
int cursor = 0;
int requestDelay = 18;
int i = requestDelay - 1;

void setup() {
  Serial.begin(115200);
  Serial.println("setup");
  delay(1000);

  //display.init();
  display.start();
 
  delay(1000);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("\nWiFi OK!");

  // Hämta plats-id
  //siteID = getSiteID(siteName);
  //Serial.println("Site ID = " + siteID);
}

void loop() {
  getDep(siteID);
  display.drawDeps(depObjList, numDeps);
  delay(80000);
}