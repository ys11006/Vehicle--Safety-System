#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

/* WIFI + MQTT */
const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASS";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);
TinyGPSPlus gps;

SoftwareSerial gpsSerial(D5, D6);

/* ===== WIFI ===== */
void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

/* ===== MQTT ===== */
void reconnect() {
  while (!client.connected()) {
    if (client.connect("Vehicle_Device")) break;
    delay(2000);
  }
}

void setup() {
  Serial.begin(115200);     // STM32
  gpsSerial.begin(9600);    // GPS

  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {

  if (!client.connected()) reconnect();
  client.loop();

  /* ===== GPS UPDATE ===== */
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  /* ===== RECEIVE STM32 ===== */
  if (Serial.available()) {

    String msg = Serial.readStringUntil('\n');
    msg.trim();

    if (msg.startsWith("CRASH")) {

      float acc = msg.substring(6).toFloat();

      float lat = 0, lon = 0;

      if (gps.location.isValid()) {
        lat = gps.location.lat();
        lon = gps.location.lng();
      }

      /* ===== GOOGLE MAP LINK ===== */
      String link = "https://maps.google.com/?q=" + String(lat,6) + "," + String(lon,6);

      /* ===== JSON PAYLOAD ===== */
      String payload = "{";
      payload += "\"event\":\"CRASH\",";
      payload += "\"acc\":" + String(acc,2) + ",";
      payload += "\"lat\":" + String(lat,6) + ",";
      payload += "\"lon\":" + String(lon,6) + ",";
      payload += "\"map\":\"" + link + "\"";
      payload += "}";

      client.publish("vehicle/crash", payload.c_str());

      Serial.println("Published: " + payload);
    }
  }
}
