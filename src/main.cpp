#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "[Your WiFi Name]";   // Your WiFi name
const char *password = "[Your WiFi Password]";            // Your WiFi password

// MQTT Broker
const char *mqtt_broker = "[Your Broker Address]"; // broker address
const char *mqtt_username = "[Broker User]"; // username for authentication
const char *mqtt_password = "[Broker Password]"; // password for authentication
const int mqtt_port = 8883; // port of MQTT over TCP
const char *client_id = "[Client-Id]";

// Publish topics
const char *mqtt_topic_pub_01 = "pub01";
// Subscribe topics
const char *mqtt_topic_sub_01 = "sub01";

// init wifi client
WiFiClientSecure espClient;
PubSubClient client(espClient);

const char* fingerprint = "42:AE:D8:A3:42:F1:C4:1F:CD:64:9C:D7:4B:A1:EE:5B:5E:D7:E2:B5";
void callback(char* topic, byte* payload, unsigned int length);

void setup() {
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
  client.publish(mqtt_topic_pub_01, "Hi I'am ESP8266");
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