#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "Restricted Connection";   // Your WiFi name
const char *password = "24681355";            // Your WiFi password

// MQTT Broker
const char *mqtt_broker = "dfe88d08.ala.us-east-1.emqxsl.com"; // broker address
const char *mqtt_username = "User01"; // username for authentication
const char *mqtt_password = "Play#KB05"; // password for authentication
const int mqtt_port = 8883; // port of MQTT over TCP
const char *client_id = "KBs_Device_01";

// Publish
const char *mqtt_topic_pub_01 = "pub01";
// Subscribe
const char *mqtt_topic_sub_01 = "sub01";

// init wifi client
WiFiClientSecure espClient;
PubSubClient client(espClient);

/*
  The common fingerprints of EMQX broker, for reference only.
  If you are not using EMQX Cloud Serverless or public EMQX broker,
  you need to calculate the sha1 fingerprint of your server certificate
  and update the 'fingerprint' variable below.
*/
// 1. fingerprint of public emqx broker. Host: broker.emqx.io
// const char* fingerprint = "B6 C6 FF 82 C6 59 09 BB D6 39 80 7F E7 BC 10 C9 19 C8 21 8E";
// 2. fingerprint of EMQX Cloud Serverless. Host: *.emqxsl.com
const char* fingerprint = "42:AE:D8:A3:42:F1:C4:1F:CD:64:9C:D7:4B:A1:EE:5B:5E:D7:E2:B5";
// 3. fingerprint of EMQX Cloud Serverless. Host: *.emqxsl.cn
// const char* fingerprint = "7E:52:D3:84:48:3C:5A:9F:A4:39:9A:8B:27:01:B1:F8:C6:AD:D4:47";

void callback(char* topic, byte* payload, unsigned int length);

void setup() {
  // Set software serial baud to 115200;
  Serial.begin(9600);
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the WiFi network");

  // connecting to a mqtt broker
  espClient.setFingerprint(fingerprint);
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.printf("The client %s connects to the mqtt broker\n", client_id);
    if (client.connect(client_id, mqtt_username, mqtt_password)) {
        Serial.println("Connected to MQTT broker.");
        client.subscribe(mqtt_topic_sub_01);
    } else {
        Serial.print("Failed to connect to MQTT broker, rc=");
        Serial.print(client.state());
        Serial.println(" Retrying in 5 seconds.");
        delay(5000);
    }
  }
  client.publish(mqtt_topic_pub_01, "Hi EMQX I'm KB9520");
}

String message = "";
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    message = "";
    for (int i = 0; i < length; i++) {
        message = "" + ((String) payload[i]);
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.print("Variable Massage: ");
    Serial.println(message);
    Serial.println("-----------------------");
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Reconnecting to MQTT broker...");
    if (client.connect(client_id, mqtt_username, mqtt_password)) {
        Serial.println("Reconnected to MQTT broker.");
        client.subscribe(mqtt_topic_sub_01);
    } else {
        Serial.print("Failed to reconnect to MQTT broker, rc=");
        Serial.print(client.state());
        Serial.println("Retrying in 5 seconds.");
        delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}