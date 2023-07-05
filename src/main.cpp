#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#define ssid "Restricted Connection"
#define pass "24681355"
#define CurrentVersion 0.0

ESP8266WiFiMulti WiFiMulti;

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println("version 0.0");
  Serial.println();
  pinMode(2,OUTPUT);
  
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  Serial.println("Connect to a Wifi");
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, pass);
  while (WiFiMulti.run() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
}

void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

void otaUpdate(String url)
{
  WiFiClient client;
  ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

  ESPhttpUpdate.onStart(update_started);
  ESPhttpUpdate.onEnd(update_finished);
  ESPhttpUpdate.onProgress(update_progress);
  ESPhttpUpdate.onError(update_error);

  t_httpUpdate_return ret = ESPhttpUpdate.update(client, url);
    
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      break;
  }
}

void check_for_updates()
{
  WiFiClient WifiClient;
  Serial.print("Current Firmware - ");
  Serial.print(CurrentVersion); 
  Serial.println(" Checking for Updates ...");

  HTTPClient http;  //Object of class HTTPClient
  http.begin(WifiClient, "https://github.com/kaushikbaidya09/ESP8266_MQTT_Cloud_Connect/blob/OTA/firmware/firmware.json");
  int httpCode = http.GET();          
  String payload = "";                                           
  if (httpCode > 0) {
    payload = http.getString();
  }
  http.end();   //Close connection

  String json = "";
  DynamicJsonDocument doc(1024);
  json = payload.c_str();
  DeserializationError error = deserializeJson(doc,json);
  if(error)
  {
    Serial.println(" deserialization failed");
  }
  if(doc["ver"] > CurrentVersion && doc["ver"] != CurrentVersion)
  {
    Serial.println("New Firmware Available. Downloading new Firmware ... ");
    otaUpdate(doc["https://github.com/kaushikbaidya09/ESP8266_MQTT_Cloud_Connect/blob/OTA/firmware/firmware.bin"]);
  }
  else
  {
    Serial.println("Already Running Latest Firmware.");
  }
}

void loop()
{
  check_for_updates();
  delay(1000);
}