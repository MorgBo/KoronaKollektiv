/*
---------------------------------------------------- Le_Counsjie v1.0 -------------------------------------------------------
Dette er kode for den håndholdte konsollen. Den består av 12 tilstander.
Oppkoblingen er som følger:
  - joystick
      - 3.3V, GND, pin 34, 33, 32
  - I2C overgang
      - 5V, GND, pin 22, 21
1. Du trenger to biblioteker
2. Bytt internettnavn og passord under CicusOfThings relatert
3. Endre UserID til din ESP32 bruker, Tall fra 0 til 9
*/

//Biblioteker
#include <CircusESP32Lib.h>
#include <LiquidCrystal_I2C.h>

//CircusOfThings relatert
char ssid[] = "YOUR_SSID";
char password[] = "YOUR_SSIDPASSWORD";
char token[] = "YOUR_TOKEN"; //legg inn token her
char server[] = "www.circusofthings.com";
char pingKey[] = "YOUR_KEY";//legg til keys her
char pongKey[] = "YOUR_KEY2";
char doorKey[] = "YOUR_KEY3";
const int UserID = 1;//endre denne til din ESP-ID
CircusESP32Lib circusESP32(server,ssid,password);

//LCD
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  // set LCD address (må scanne med egen kode), number of columns and rows

//joystick
const int x = 34;
const int y = 33;
const int s = 32;

//tilstander:
#define bedroom_menu 1
#define kitchen_menu 2
#define bathroom_menu 3
#define livingroom_menu 4
#define gone_menu 5
#define bedroom 6
#define kitchen 7
#define bathroom 8
#define livingroom 9
#define gone 10
#define initialize 11
#define doorbell 12

//Rom ID
const int livingroomID = 1;
const int kitchenID = 3;
const int bathroomID = 2;
const int bedroomID = 4; //endre denne til ditt soverom
const int goneID = 0;


//konstanter:
const int right = 1;
const int left = -1;
const int up = 2;
const int down = -2;
const int middlepress = 3;
const int t = 300;

//misc
int state = initialize;
int buttonpress = 0;
int end = 0;
int hours = 12;
int kvarter = 0;
int guests = 0;
bool SE = true; 

void setup() {
  Serial.begin(115200);
  pinMode(x,INPUT);
  pinMode(y,INPUT);
  pinMode(s,INPUT_PULLUP);//joystick har antagelig allerede innebygd pullup
  circusESP32.begin();
  Serial.println("Le Counsjie is ready for your commands");
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
}

void loop() {
  switch(state) {
    
    case initialize:
      Serial.println(millis()%1000);
      if (millis()%5000 > 0 || millis()%5000 < 1000) {
        checkDoorbell();
      }
      displayroom("Le Counsjie v1.0");
      if (SE == true) {
        lcd.clear();
        SE = false;
      }
      if (joystick() == middlepress) {
        //Her kan man legge opstartskommandoer
        state = livingroom_menu;
        SE = true;
        lcd.clear();
        delay(t);
      }
      break;
    //-----------------DOORBELL-----------------------
    case doorbell:
      if (SE == true) {
        lcd.clear();
        SE = false;
      }
      if (millis()%100000 > 10000) {
        state = initialize;
      }
      displayroom("Ringeklokke");
      if (joystick() == left) {
        state = gone_menu;
        delay(t);
        SE = true;   
      }
      if (joystick() == right) {
        state = livingroom_menu;
        delay(t);
        SE = true; 
      }
      if (joystick() == middlepress) {
        circusESP32.write(doorKey, UserID, token);
        state = initialize;
        delay(t);
        SE = true;     
        lcd.clear();   
      }
      break;
    //--------------------------LIVINGROOM_MENU----------------------------
    case livingroom_menu:
      if (SE == true) {
        lcd.clear();
        SE = false;
      }
      displayroom("Stue");
      if (joystick() == left) {
        state = doorbell;
        delay(t);
        SE = true;   
      }
      if (joystick() == right) {
        state = bathroom_menu;
        delay(t);
        SE = true; 
      }
      if (joystick() == middlepress) {
        state = livingroom;
        delay(t);
        SE = true;     
        lcd.clear();   
      }
      break;
    //--------------------------------BATHROOM_MENU--------------------------
    case bathroom_menu:
      if (SE == true) {
        lcd.clear();
        SE = false;
      }
      displayroom("Bad");
      if (joystick() == left) {
        state = livingroom_menu;
        delay(t);
        SE = true;   
      }
      if (joystick() == right) {
        state = kitchen_menu;
        delay(t);
        SE = true; 
      }
      if (joystick() == middlepress) {
        state = bathroom;
        delay(t);
        SE = true;        
        lcd.clear();
      }
      break;
    //---------------------------------KITCHEN_MENU-------------------------
    case kitchen_menu:
      if (SE == true) {
        lcd.clear();
        SE = false;
      }
      displayroom("Kjokken");
      if (joystick() == left) {
        state = bathroom_menu;
        delay(t);
        SE = true;   
      }
      if (joystick() == right) {
        state = bedroom_menu;
        delay(t);
        SE = true; 
      }
      if (joystick() == middlepress) {
        state = bedroom;
        delay(t);
        SE = true;        
        lcd.clear();
      }
      break;
    //-----------------------------------BEDROOM_MENU--------------------
    case bedroom_menu:
      if (SE == true) {
        lcd.clear();
        SE = false;
      }
      displayroom("Soverom");
      if (joystick() == left) {
        state = bathroom_menu;
        delay(t);
        SE = true;   
      }
      if (joystick() == right) {
        state = gone_menu;
        delay(t);
        SE = true; 
      }
      if (joystick() == middlepress) {
        state = bedroom;
        delay(t);
        SE = true;        
        lcd.clear();
      }
      break;
    //----------------------------------GONE_MENU----------------------------------
    case gone_menu:
      if (SE == true) {
        lcd.clear();
        SE = false;
      }
      displayroom("Drar fra leil");
      if (joystick() == left) {
        state = bedroom_menu;
        delay(t);
        SE = true;   
      }
      if (joystick() == right) {
        state = doorbell;
        delay(t);
        SE = true; 
      }
      if (joystick() == middlepress) {
        state = gone;
        delay(t);
        SE = true;    
        lcd.clear();   
      }
      break;
 
    /*----------------------------------------------- AMBIENT STATES ---------------------------------------------------*/

    //-----------------LIVINGROOM-------------------
    case livingroom:
      if (joystick() == middlepress) {
        buttonpress = buttonpress + 1;
        delay(t);
      }
      if (buttonpress == 0) {
        hours = clockHour(hours); 
        display(guests, hours, kvarter, end);
        delay(t);
      }
      if (buttonpress == 1) {
        kvarter = clockKvarter(kvarter);
        display(guests, hours, kvarter, end);   
        delay(t); 
      }
      if (buttonpress == 2) {
        end = clockEnd(end);
        display(guests, hours, kvarter, end);
        delay(t);
      }
      if (buttonpress == 3) {
        guests = clockGuests(guests);
        display(guests, hours, kvarter, end); 
        delay(t);
      }
      if (buttonpress == 4) {
        int pingID = pingIDgenerator(livingroomID, guests, hours, kvarter, end);
        Serial.println(pingID); 
        ping(pingID);
        pongChecker();
        state = initialize;
        buttonpress = 0;
        end = 0;
        hours = 12;
        kvarter = 0;
        guests = 0;
        delay(t);
      }
      break;
    //-----------------BATHROOM-----------------
    case bathroom:
      if (joystick() == middlepress) {
        buttonpress = buttonpress + 1;
        delay(t);
      }
      if (buttonpress == 0) {
        hours = clockHour(hours); 
        display(guests, hours, kvarter, end);
        delay(t);
      }
      if (buttonpress == 1) {
        kvarter = clockKvarter(kvarter);
        display(guests, hours, kvarter, end);   
        delay(t); 
      }
      if (buttonpress == 2) {
        end = clockEnd(end);
        display(guests, hours, kvarter, end);
        delay(t);
      }
      if (buttonpress == 3) {
        guests = 1; //clockGuests(guests);
        display(guests, hours, kvarter, end); 
        delay(t);
      }
      if (buttonpress == 4) {
        int pingID = pingIDgenerator(bathroomID, guests, hours, kvarter, end);
        Serial.println(pingID); 
        ping(pingID);
        pongChecker();
        state = initialize;
        buttonpress = 0;
        end = 0;
        hours = 12;
        kvarter = 0;
        guests = 0;
        delay(t);
      }
      break;
    //---------------------KITCHEN----------------------------------------
    case kitchen:
      if (joystick() == middlepress) {
        buttonpress = buttonpress + 1;
        delay(t);
      }
      if (buttonpress == 0) {
        hours = clockHour(hours); 
        display(guests, hours, kvarter, end);
        delay(t);
      }
      if (buttonpress == 1) {
        kvarter = clockKvarter(kvarter);
        display(guests, hours, kvarter, end);   
        delay(t); 
      }
      if (buttonpress == 2) {
        end = clockEnd(end);
        display(guests, hours, kvarter, end);
        delay(t);
      }
      if (buttonpress == 3) {
        guests = clockGuests(guests);
        display(guests, hours, kvarter, end); 
        delay(t);
      }
      if (buttonpress == 4) {
        int pingID = pingIDgenerator(kitchenID, guests, hours, kvarter, end);
        Serial.println(pingID); 
        ping(pingID);
        pongChecker();
        state = initialize;
        buttonpress = 0;
        end = 0;
        hours = 12;
        kvarter = 0;
        guests = 0;
        delay(t);
      }
      break;
    //--------------------------BEDROOM----------------
    case bedroom:
      if (joystick() == middlepress) {
        buttonpress = buttonpress + 1;
        delay(t);
      }
      if (buttonpress == 0) {
        hours = clockHour(hours); 
        display(guests, hours, kvarter, end);
        delay(t);
      }
      if (buttonpress == 1) {
        kvarter = clockKvarter(kvarter);
        display(guests, hours, kvarter, end);   
        delay(t); 
      }
      if (buttonpress == 2) {
        end = clockEnd(end);
        display(guests, hours, kvarter, end);
        delay(t);
      }
      if (buttonpress == 3) {
        guests = clockGuests(guests);
        display(guests, hours, kvarter, end); 
        delay(t);
      }
      if (buttonpress == 4) {
        int pingID = pingIDgenerator(bedroomID, guests, hours, kvarter, end);
        Serial.println(pingID); 
        ping(pingID);
        pongChecker();
        state = initialize;
        buttonpress = 0;
        end = 0;
        hours = 12;
        kvarter = 0;
        guests = 0;
        delay(t);
      }
      break;
    //--------------------GONE------------------
    case gone:
      displayroom("Forlater leil");
      state = initialize;
      delay(t);
      break;
  }
}

/*------------------------------------------ FUNKSJONER -----------------------------------------*/

//Joystick
//position = 0 --> center
//position = 1 --> right
//position = -1 --> left
//position = 2 --> up
//postition = -2 --> down
//position = 10 --> middlepress
int joystick() {
  int position = 0;
  int xpos = analogRead(x);
  int ypos = analogRead(y);
  int spos = digitalRead(s);
  if (xpos>4000) {
    position = right;
    lcd.clear();
  }
  if (xpos<100) {
    position = left;
    lcd.clear();
  }
  if (spos != 1) {
    position = middlepress;
    lcd.clear();
  }
  if (ypos>4000) {
    position = up;
    lcd.clear();
  }
  if (ypos<100) {
    position = down;
    lcd.clear();
  }
  return position;
}

//MASTER Display
void display(int guests, int hour, int kvarter, int end) {
  int endTid = end*15;
  int kvarterTid = kvarter*15;
  if (guests > 5) {
      guests = 5;
    }
  if (guests < 0) {
    guests = 0;
  }

  if (hour > 23){
    hour = 0;
  }
  if (hour < 0){
    hour = 23;
  }

  if (kvarterTid > 45){
    kvarterTid = 0;
  }
  if (kvarterTid < 0){
    kvarterTid = 0;
  }
  if (endTid < 0){
    endTid = 0;
  }
  if (endTid > 405){
    endTid = 0;
  }

  lcd.setCursor(0, 0);
  lcd.print("Fra");
  lcd.setCursor(0, 1);
  lcd.print("Tid:");
  lcd.setCursor(11, 0);
  lcd.print("Pers:");
  
  if(hour<10) {
    lcd.setCursor(4, 0);
    lcd.print("0");lcd.setCursor(5, 0);
    lcd.print(hour);
  }
  
  if(hour>=10){
    lcd.setCursor(4, 0);
    lcd.print(hour);
  }
  lcd.setCursor(6,0);
  lcd.print(":");

  lcd.setCursor(8, 0);
  if(kvarterTid<10) {
    lcd.print("0");
  }
  lcd.setCursor(7, 0);
  lcd.print(kvarterTid);
  
  lcd.setCursor(5, 1);
  lcd.print(endTid);
  lcd.setCursor(12, 1);
  lcd.print(guests);
}

void displayroom(char* room) {

  lcd.setCursor(0,1);
  lcd.print(room);
}

//endre tid funksjoner
int clockHour(int entry) {
  if (entry > 23) {
    return 0;
  }
  if (entry < 0) {
    return 23;
  }
  if (joystick() == up) {
    int new_time = entry + 1;
    return new_time;
  }
  if (joystick() == down) {
    int new_time = entry - 1;
    return new_time;
  }
  else {
    return entry;
  }
}

int clockKvarter(int entry) {
  if (entry > 3 || entry < 0) {
    return 0;
  }
  if (joystick() == up) {
    int new_time = entry + 1;
    return new_time;
  }
  if (joystick() == down) {
    int new_time = entry - 1;
    return new_time;
  }
  else {
    return entry;
  }
}

int clockEnd(int entry) {
  if (entry > 9 || entry < 0) {
    return 0;
  }
  if (joystick() == up) {
    int new_time = entry + 1;
    return new_time;
  }
  if (joystick() == down) {
    int new_time = entry - 1;
    return new_time;
  }
  else {
    return entry;
  }
}

int clockGuests(int entry) {
  if (entry > 5 || entry < 0) {
    return 0;
  }
  if (joystick() == up) {
    int new_time = entry + 1;
    return new_time;
  }
  if (joystick() == down) {
    int new_time = entry - 1;
    return new_time;
  }
  else {
    return entry;
  }
}
/*
PingID Sifferplass:
x 0 0 0 0 0 0  rom nummer
0 x 0 0 0 0 0  UserID
0 0 x 0 0 0 0  antall gjester 
0 0 0 x x 0 0  timetall
0 0 0 0 0 x 0  kvartertall
0 0 0 0 0 0 x  antall kvarter du vil booke
*/
int pingIDgenerator(int room, int guests, int hours, int kvarter, int end) {
  int pingID = room*1000000 + UserID*100000 + guests*10000 + hours*100 + kvarter*10 + end;
  return pingID;
}
void ping(int pingID) {
  displayroom("Forespor");
  circusESP32.write(pingKey, pingID, token);
}
int pong() {
  int pongMessage = circusESP32.read(pongKey, token);
  return pongMessage;
}
void pongReset() {
  circusESP32.write(pongKey, 0, token);
}
void pongChecker() {
  int pongValue = 0;
  while (pongValue == 0) {
    pongValue = pong();
    if (pongValue == 1) {
      displayroom("Du har booket");
      pongReset();
      delay(t);
    }
    if (pongValue == 2) {
      displayroom("Det er fullt");
      pongReset();
      delay(t);
    }
    if (pongValue == 3) {
      displayroom("Ugyldig verdi");
      pongReset();
      delay(t);
    }
    else {
      displayroom("Venter pa CoT");
      delay(t);
    }
  }
}
//Ringeklokke
void checkDoorbell() {
  int door = circusESP32.read(doorKey, token);
  if (door > 0) {
    lcd.clear();
    displayroom("RING RING RING");
    delay(5000);
    circusESP32.write(doorKey, 0, token);
  }
}
