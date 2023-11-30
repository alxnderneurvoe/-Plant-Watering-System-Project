/***********************************************************************************
Plant Watering System

By : Monarchy Team
1. Muhammad Habibillah
2. Hilman Asy'ari
3. Lil Hamdi
4. Yuliani Zahra
5. Susani Sarafina

email : mhabibillah2303@gmail.com
pass  : Blynkbibi23!
************************************************************************************/

#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

char ssid[] = "Iphone";
char pass[] = "1234567890";
char auth[] = "zckwzzsNXyZqsmfLsx5Me0V6ye6fYQxp";

#define SensorPin A0
#define DHTPin D7
#define RelayPin D3
#define AutoLed D0
#define ManLed D5
#define BuzzerPin D8

#define DHTTYPE DHT11

#define VPIN_MoistPer V1
#define VPIN_TEMPERATURE V2
#define VPIN_HUMIDITY V3
#define VPIN_MODE_SWITCH V4
#define VPIN_RELAY V5

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int moisturePercentage = 0;
int temperature = 0;
int humidity = 0;
bool isAutomaticMode = true;

DHT dht(DHTPin, DHTTYPE);

BlynkTimer timer;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextWrap(false);
  display.setTextSize(1);

  display.setCursor(23, 0);
  display.println("Plant Watering");
  display.setTextSize(1);
  display.setCursor(45, 10);
  display.println("System");
  display.setTextSize(1);
  display.setCursor(10, 20);
  display.println("By: Monarchy Team");
  display.display();
  delay(5000);

  WiFi.begin(ssid, pass);
  do {
    delay(100);
    if (WiFi.status() == WL_CONNECTED) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Connected to WiFi!");
      display.println("IP Address:");
      display.println(WiFi.localIP());
      display.display();
      break;
    } else {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Connecting to WiFi...");
      display.display();
      delay(5000);
    }
  } while (true);
  delay(5000);

  Blynk.begin(auth, ssid, pass);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connected to WiFi!");
  display.println("IP Address:");
  display.println(WiFi.localIP());
  display.display();
  delay(2000);

  dht.begin();

  pinMode(AutoLed, OUTPUT);
  digitalWrite(AutoLed, !isAutomaticMode);
  pinMode(ManLed, OUTPUT);
  digitalWrite(ManLed, isAutomaticMode);
  Blynk.virtualWrite(VPIN_MODE_SWITCH, isAutomaticMode ? 1 : 0);

  pinMode(RelayPin, OUTPUT);
  toggleRelay();

  timer.setInterval(3000L, updateSensorValues);
  timer.setInterval(2000L, toggleRelay);
}

void displayData(String line1, String line2) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(30, 2);
  display.print(line1);

  display.setTextSize(1);
  display.setCursor(1, 15);
  display.print("Mode: " + String(isAutomaticMode ? "A" : "M"));

  display.setCursor(1, 25);
  display.print(line2);

  display.display();
}

void updateSensorValues() {
  int sensorValue = analogRead(SensorPin);
  moisturePercentage = map(sensorValue, 1024, 0, 100, 0);

  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  Blynk.virtualWrite(VPIN_MoistPer, moisturePercentage);
  Blynk.virtualWrite(VPIN_TEMPERATURE, temperature);
  Blynk.virtualWrite(VPIN_HUMIDITY, humidity);

  displayData("Moisture: " + String(moisturePercentage) + "%", "Temp: " + String(temperature) + "C  Humid: " + String(humidity) + "%");
}

void toggleRelay() {
  if (isAutomaticMode) {
    // Mode otomatis
    if (moisturePercentage > 70) {
      digitalWrite(RelayPin, HIGH);
      Blynk.virtualWrite(V5, HIGH);
    } else if (moisturePercentage < 70) {
      digitalWrite(RelayPin, LOW);
      Blynk.virtualWrite(V5, LOW);
    }
  } else {
    BLYNK_WRITE(V5);
  }
}

BLYNK_WRITE(V4) {
  int modeValue = param.asInt();
  if (modeValue == 1) {
    isAutomaticMode = true;
    digitalWrite(AutoLed, HIGH);
    digitalWrite(ManLed, LOW);
    tone(BuzzerPin, 500);
    delay(1000);
    tone(BuzzerPin, 0);
  } else {
    isAutomaticMode = false;
    digitalWrite(AutoLed, LOW);
    digitalWrite(ManLed, HIGH);
    tone(BuzzerPin, 500);
    delay(1000);
    tone(BuzzerPin, 0);
  }
}

BLYNK_WRITE(V5) {
  int pinValue = param.asInt();  // membaca nilai dari tombol virtual pada halaman Blynk

  if (pinValue == 1) {
    digitalWrite(RelayPin, HIGH);  // menghidupkan relay 3
  } else {
    digitalWrite(RelayPin, LOW);  // mematikan relay 3
  }
}

void loop() {
  Blynk.run();
  timer.run();
}
