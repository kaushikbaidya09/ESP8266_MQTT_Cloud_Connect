#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "xxxxxxxx"
#define WLAN_PASS       "xxxxxxxx"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME  "xxxxxxxx"
#define AIO_KEY       "xxxxxxxx"

/**************************** IO Pin Setup ***********************************/

int Bed_light = D5;
int Night_lamp = D6;

/************ Global State (you don't need to change this!) ******************/

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/xxxx");

/*************************** Sketch Code ************************************/

void MQTT_connect();

void setup() {
  Serial.begin(9600);

  pinMode(Bed_light,OUTPUT);
  pinMode(Night_lamp,OUTPUT);

  Serial.println(F("Adafruit MQTT"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&onoffbutton);
}

uint32_t x=0;

void loop() {
  MQTT_connect();

/********************* MQTT SUBSCRIBE STUFF ******************************/

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);

      if (strcmp((char *)onoffbutton.lastread, "ON") == 0) {
        Serial.println("Bed Light ON"); 
        digitalWrite(Bed_light, HIGH);
      }
      if (strcmp((char *)onoffbutton.lastread, "OFF") == 0) {
        Serial.println("Bed Light OFF");
        digitalWrite(Bed_light,LOW);
      }
    }
  }
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         ESP.reset();
         //while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
