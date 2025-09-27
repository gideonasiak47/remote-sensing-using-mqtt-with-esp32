#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h> 

// DHT sensor setup !!!ENSURE THEY CORRESPOND TO THE PIN CONNECTION ON THE BREADBOARD
#define DHTPIN 4             // DHT11 data pin connected to GPIO4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// LDR and Ultrasonic pins !!!ENSURE THEY CORRESPOND TO THE PIN CONNECTION ON THE BREADBOARD
const int LDR_PIN = 36;      // Analog pin for LDR
const int TRIG_PIN = 5;      // Trigger pin for ultrasonic sensor
const int ECHO_PIN = 18;     // Echo pin for ultrasonic sensor

 

const char* ssid = "Jail_break67";
const char* password = "Gideon1?";
const char* mqtt_server = "10.229.86.18";


WiFiClient espClient;
PubSubClient client(espClient);


void setup_wifi() {
  delay(10000);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}


float readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  int duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}


void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);

  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}
void loop() {
  if (!client.connected()) {
    setup_wifi();
    if (!client.connect("ESP32Client")) return;
  }
  client.loop();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float distance = readDistanceCM();
  int ldrValue = analogRead(LDR_PIN);
  int brightness = map(ldrValue, 0, 4000, 0, 100);  // returns 0–100

  StaticJsonDocument<200> doc;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["distance"] = distance;
  doc["light"] = ldrValue;
  doc["brightness"] = brightness;

  char buffer[200];
  serializeJson(doc, buffer);

  // CHANGE IN THE ESP32 ADDRESS BELOW TO YOUR INDEX NUMBER
  client.publish("esp32/7342623/data", buffer);
  Serial.println(buffer);

  delay(5000); //publish every five seconds
}