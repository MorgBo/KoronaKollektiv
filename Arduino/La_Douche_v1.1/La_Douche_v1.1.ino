/*
---------------------------------------- La Douche v1.0 ---------------------------------------------

Denne koden er til badet i hybelkollektivet
Koden inneholder:
  - Manuell styring av vifte* i bad (Av/På) fra CoT
  - Automatisk styring av vifte i bad basert på fuktighet
  - Logging av temperatur og fuktighet til CoT

*vifte er en DC-motor som styres av en N-channel powermosfet

Koden er en tilstandsmaskin med 3 tilstander:
 1. OFF
 2. ON
 3. AUTO

1. Bytt wifi ssid og passord til ditt eget
2. Endre humidityThreshold til hva du selv vil ha før vifta slår seg på
*/

//Biblioteker
#include <CircusESP32Lib.h>
#include <DHT.h>

//CoT setup
char ssid[] = "YOUR_SSID";//Wifi navn
char password[] =  "YOUR_SSIDPASSWORD";//wifi passord
char token[] = "YOUR_TOKEN";//CoT token
char server[] = "www.circusofthings.com";
char temperature_key[] = "YOUR_KEY";//finner på CoT
char humidity_key[] = "YOUR_KEY2";
char fan_key[] = "YOUR_KEY3";
CircusESP32Lib circusESP32(server,ssid,password);

//Tilstander
#define OFF 1
#define ON 2
#define AUTO 3
int state = OFF;

//DHT sensor
#define DHTPIN 17
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//Motor
const int motorPin = 22;

//Misc
const int t = 100; //ms
const int humidityThreshold = 69;//% fuktigheten før vifta slår seg på
 
void setup() {
    Serial.begin(115200);
    dht.begin();
    circusESP32.begin();
    pinMode(motorPin, OUTPUT);
}
 
void loop() {
  switch(state) {
    //-------------------- OFF STATE -----------------------
    case OFF:
      Serial.println("OFF");
      digitalWrite(motorPin,LOW);
      writeTemp();
      if (readCOT() == 1) {
        state = ON;
      }
      if (readCOT() == 2) {
        state = AUTO;
      }
      delay(t);
      break;
    //--------------------- ON STATE ------------------------
    case ON:
      Serial.println("ON");
      writeTemp();
      digitalWrite(motorPin, HIGH);
      if (readCOT() == 0) {
        state = OFF;
      }
      if (readCOT() == 2) {
        state = AUTO;
      }
      delay(t);
    //--------------------- AUTO STATE -----------------------
    case AUTO:
      Serial.println("AUTO");
      writeTemp();
      autoControl();
      if (readCOT() == 0) {
        state = OFF;
      }
      if (readCOT() == 1) {
        state = ON;
      }
      delay(t);
  }
}


//--------------------------------------------- FUNKSJONER ----------------------------------

//leser av hvilken tilstand den skal være i fra CoT
int readCOT() {
  int A = circusESP32.read(fan_key, token);
  return A;
}
//Skriver temperatur og fuktighet til CoT
void writeTemp() {
  float t = dht.readTemperature(); 
  if (isnan(t))
      t=-1;
  float h = dht.readHumidity();
  if (isnan(h))
      h=-1;
  Serial.println("");
  Serial.print("Temperature: ");
  Serial.println(t);
  Serial.print("Humidity: ");
  Serial.println(h);
  circusESP32.write(temperature_key,t,token);
  circusESP32.write(humidity_key,h,token);
}
//Hvordan autofunksjonen skal fungere
void autoControl() {
  float t = dht.readTemperature(); 
  if (isnan(t))
      t=-1;
  float h = dht.readHumidity();
  if (isnan(h))
      h=-1;
  if (h>humidityThreshold) {
    Serial.println("Fan On");
    digitalWrite(motorPin,HIGH);
  }
  else {
    Serial.println("Fan Off");
    digitalWrite(motorPin,LOW);
  }
}
