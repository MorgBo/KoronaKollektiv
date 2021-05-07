/*
---------------------------------- Le Counslee v 0.1 ----------------------------------
*/

// Importerer alle nødvendige biblioteker.
#include <analogWrite.h>
#include <TFT_eSPI.h>
#include <CircusESP32Lib.h>
#include <ESP32_Servo.h>

// Informasjon om internett-tilkobling og tilkobling til CoT.
char ssid[] = "YOUR_SSID";  
char password[] = "YOUR_SSIDPASSWORD";
char token[] = "YOUR_TOKEN";
char server[] = "www.circusofthings.com";
char autoKey[] = "YOUR_KEY";
char tempKey[] = "YOUR_KEY2";

// Initierer CoT biblioteket
CircusESP32Lib circusESP32(server, ssid, password);

// Definerer pinner
const int ldrPin = 36;
const int potWindow = 15;
const int ledGreen = 26;
const int ledBlue = 27;
const int servoPin = 32;
const int tmpPin = 34;
const int potLight = 35;

// Setter tft. til å være kommandoen, brukt senere i koden.
TFT_eSPI tft = TFT_eSPI();
Servo servo;

// void setup() kjører syklisk èn gang igjennom, før den fortsetter med void loop()
void setup() {
  Serial.begin(115200);
  pinMode(ledGreen, OUTPUT);  // Definerer pinnen til å være utgang eller innngang
  pinMode(ledBlue, OUTPUT);
  pinMode(ldrPin, INPUT);
  pinMode(tmpPin, INPUT);
  pinMode(potLight, INPUT);
  pinMode(potWindow, INPUT);
  servo.attach(servoPin, 500, 2500);

  circusESP32.begin();  // Initierer kommunikasjon med CoT

  tft.init();           // Starter informasjonsdeling til OLED-skjerm
  tft.setRotation(3);   // Roterer skjermen 270 grader så den får en naturlig retning for bruker
}

void loop() {
  bool autom = circusESP32.read(autoKey, token);  // Leser CoT-bit om rommet skal styres manuelt eller automatisk

  // Innstillnger for startup på OLED-skjerm
  tft.invertDisplay(true);  // Setter displayet til å bruke RGB farger.
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.println("        Le Counslee\n");

  // Temperaturbehandling med overføring av temperatur til CoT
  int tmpStatus = analogRead(tmpPin);
  float voltage = tmpStatus / 1023.0;
  float tempC = (voltage - 0.5) * 100;
  tft.setTextColor(TFT_BLUE, TFT_WHITE);
  tft.print("Grader inne: ");
  tft.println(tempC);
  circusESP32.write(tempKey, tempC, token);

  // Hvis rommet er aktivert med automatikk, skal alt styres automatisk.
  if (autom == HIGH) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("AUTO-MODUS");

    int ldrStatus = map(analogRead(ldrPin), 0, 4095, 0, 100);
    int pwmStatus = map(ldrStatus, 0, 100, 255, 0);
    analogWrite(ledBlue, pwmStatus);
    tft.println(ldrStatus);
    tft.print(pwmStatus);

    if (tempC <= 21.0) {
    digitalWrite(ledGreen, 1);
    servo.write(0);
    } else {
    digitalWrite(ledGreen, 0);
    servo.write(180);
    }
  }

  // Hvis rommet IKKE er aktivert med automatikk, skal man styre lysstyrken og åpning av vinduet selv.
  else {
    tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
    tft.println("MANUELL-MODUS");

    int servoPos = map(analogRead(potWindow), 0, 4095, 0, 180);
    servo.write(servoPos);
    int potLightStatus = map(analogRead(potLight), 0, 4095, 0, 255);
    analogWrite(ledBlue, potLightStatus);
    tft.println(potLightStatus);
    tft.println(servoPos);
  }
}
