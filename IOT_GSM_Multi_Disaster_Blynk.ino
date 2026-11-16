#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <ADXL345_WE.h>
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleEsp32.h>

// --- CONFIG - Apna Data Yaha Dalna ---
char auth[] = "YOUR_BLYNK_TOKEN";
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASS";

LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_BME280 bme;
ADXL345_WE myAcc = ADXL345_WE(0x53);

#define FLAME_PIN 34
#define MQ2_PIN 35
#define WATER_PIN 32
#define BUZZER_PIN 33

BlynkTimer timer;

void sendToCloud() {
  float temp = bme.readTemperature();
  int gas = analogRead(MQ2_PIN);
  int water = analogRead(WATER_PIN);
  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1, gas);
  Blynk.virtualWrite(V2, water);
}

void checkDisaster() {
  float temp = bme.readTemperature();
  int flame = digitalRead(FLAME_PIN);
  int gas = analogRead(MQ2_PIN);
  int water = analogRead(WATER_PIN);

  String msg = "";
  if(temp > 50 || flame == 0) msg = "FIRE ALERT!";
  else if(gas > 500) msg = "GAS LEAKAGE!";
  else if(water > 400) msg = "FLOOD ALERT!";

  if(msg != "") {
    lcd.clear(); lcd.print(msg);
    digitalWrite(BUZZER_PIN, HIGH);
    Blynk.logEvent("disaster_alert", msg);
    delay(2000);
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(); 
  lcd.init(); lcd.backlight();
  bme.begin(0x76); 
  myAcc.init();
  pinMode(FLAME_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(2000L, sendToCloud);
  timer.setInterval(3000L, checkDisaster);
}

void loop() {
  Blynk.run();
  timer.run();
}
