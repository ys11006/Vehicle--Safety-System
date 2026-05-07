#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

/* MQTT CONFIG */
const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASS";
const char* mqtt_server = "broker.hivemq.com";


WiFiClient espClient;
PubSubClient client(espClient);
TinyGPSPlus gps;


SoftwareSerial gpsSerial(D5, D6); // RX, TX

/*  WIFI SETUP  */
void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

/* ===== MQTT RECONNECT ===== */
void reconnect() {
  while (!client.connected()) {
    client.connect("Vehicle_Client");
  }
}

void setup() {
  Serial.begin(115200);   // STM32 communication
  gpsSerial.begin(9600);  // GPS module

  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {

  if (!client.connected()) reconnect();
  client.loop();

  /* ===== CONTINUOUS GPS READ ===== */
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  /* ===== RECEIVE FROM STM32 ===== */
  if (Serial.available()) {

    String event = Serial.readStringUntil('\n');
    event.trim();

    if (event.startsWith("CRASH")) {

      if (gps.location.isValid()) {

        float lat = gps.location.lat();
        float lon = gps.location.lng();

        String payload = "{";
        payload += "\"event\":\"CRASH\",";
        payload += "\"lat\":" + String(lat, 6) + ",";
        payload += "\"lon\":" + String(lon, 6);
        payload += "}";

        client.publish("vehicle/crash", payload.c_str());

        Serial.println("Published: " + payload);
      }
      else {
        Serial.println("GPS not fixed yet!");
      }
    }
  }
}