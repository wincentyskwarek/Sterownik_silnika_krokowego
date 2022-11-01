
#include <LiquidCrystal.h>
#include <EEPROM.h>

// Definicja pinów wyświetlacza
#define rs 8 
#define en 9 
#define d4 4 
#define d5 5 
#define d6 6 
#define d7 7 

// Definicja pinów sterowania TB6600
#define KR A1 //definicja pinu zmiany kierunku ruchu 
#define START 3 //definicja pinu zmiany start/stop 
#define SPEED 11 //definicja pinu zmiany NORMAL/MAX W NOWEJ WRSJI A0 -----------------------

// Definicja pinów wykonawczych TB6600
#define PUL 10 //definicja pinów pulsu
#define DIR 12 //definicja kierunków ruchu
#define ENA 13 //definicja zatrzymania wału

// definicja ustawień impulsatora
#define pinA 2  // podłączenie do CLK na KY-040
#define pinB A2  //podłączenie do DT na KY-040
#define pinSW A3 

// initializacja LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Adres do zapisu w EEPROM
int address=0;
bool zapVisible;

int odczyt, odczytS, x=1, xS; // odczyt z potencjometru
int encoderPosCount; 
int pinALast;  
int aVal;
float vobr;
int kierunek, kierunekS;
int praca, pracaS;
int spid, spidS;
unsigned long long ti, ta;
unsigned long t, tzap,czasObecnyMillis;

const int minimum=1; //dedinicja minimum szybkości (odwtotność czasu impulsu)
const int maksimum=1000; //dedinicja minimum szybkości (odwtotność czasu impulsu)
const float mnoznik=0.1398; //mnożnik do linearyzacji 0.1473

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("ElMechPlast");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Imp:");
  lcd.setCursor(0, 1);
  lcd.print("Kier:");
  pinMode (PUL, OUTPUT);
  pinMode (DIR, OUTPUT);
  pinMode (ENA, OUTPUT);
  digitalWrite(DIR,HIGH);
  digitalWrite(ENA,HIGH);
  pinMode(KR,INPUT);
  digitalWrite(KR,HIGH);
  pinMode(START,INPUT);
  digitalWrite(START,HIGH);
  pinMode(SPEED,INPUT);
  digitalWrite(SPEED,HIGH);
  
// odczyty przełączników 
  kierunek=digitalRead(KR);
  spid = digitalRead(SPEED);
  praca=digitalRead(START);
    
// Wyświetlenie START/STOP
  lcd.setCursor(11, 0);
  if (praca)
  {
      digitalWrite(ENA,HIGH);
      lcd.print("START");
  }
  else
  {
      digitalWrite(ENA,LOW);
      lcd.print("STOP ");
  }
//Wyświetlanie kierunku Prawy/Lewy
  lcd.setCursor(5, 1);
  if(kierunek)
  {
      digitalWrite(DIR,HIGH);
      lcd.print("P");
  }
  else
  { 
      digitalWrite(DIR,LOW);
      lcd.print("L ");
  }  
  
 //ustalenie prędkości startowej
  encoderPosCount=EEPROMReadInt(address);
  x=encoderPosCount;
  t=((unsigned long)(1000000.00000/x));
  vobr=mnoznik*x;
  lcd.setCursor(4, 0);
  lcd.print("      ");
  lcd.setCursor(4, 0);
  lcd.print(x);
   
// impulsator START
   pinMode (pinA,INPUT);
   pinMode (pinB,INPUT);
   pinMode (pinSW,INPUT);
   digitalWrite(pinSW,HIGH);
   /* Read Pin A
   Whatever state it's in will reflect the last position   
   */
   pinALast =digitalRead(pinA);   
  zapVisible=false;
  ti=micros(); // odczyt czasu z mikrokontrolera do generowania ruchu
  tzap=millis();// odczyt czasu z mikrokontrolera do zapisu do EEPROM
 // Serial.begin(9600);
}

void loop() {
//
  czasObecnyMillis=millis();
  if((digitalRead(pinSW)==LOW)&&(czasObecnyMillis-tzap>1000)&& (spid))
  {
    tzap=millis();
    lcd.setCursor(13, 1);
    lcd.print("ZAP");

    zapVisible=true;
    EEPROMWriteInt(address, x);
  }
  if ((czasObecnyMillis-tzap>1000) && (zapVisible))
  {
    zapVisible=false;
    lcd.setCursor(13, 1);
    lcd.print("   ");       
  }

 // Serial.println(czasObecnyMillis-tzap);
// odczyty przełączników 
  kierunek=digitalRead(KR);
  spid = digitalRead(SPEED);
  praca=digitalRead(START);
  
// Praca impulsatora
  aVal = digitalRead(pinA);
  if (aVal != pinALast) // Means the knob is rotating
     // if the knob is rotating, we need to determine direction
     // We do that by reading pin B.
  {
     if (digitalRead(pinB) != aVal)   // Means pin A Changed first - We're Rotating Clockwise
     {
       if (encoderPosCount<maksimum) encoderPosCount ++;
     }
     else // Otherwise B changed first and we're moving CCW
     {
       if (encoderPosCount>minimum) encoderPosCount--;
     }
  } 
  pinALast = aVal;

  odczyt = encoderPosCount;


// Generowanie i wyświetlanie prędkości oraz odczyt potencjometru
  if((spid!=spidS) || (odczyt!=odczytS))
  {
    lcd.setCursor(7, 1);
    if (spid==LOW)
    {
      lcd.print("MAX. ");
      x=maksimum;
    }
    else
    {
      lcd.print("NORM.");
      x=odczyt;
    }
    spidS=spid;
    odczytS=odczyt;
  }

// Włączanie i wyświetlanie pracy silnika
  if (pracaS!=praca)
  {
    lcd.setCursor(11, 0);
    if (praca)
    {
       digitalWrite(ENA,HIGH);
       lcd.print("START");
    }
    else
    {
      digitalWrite(ENA,LOW);
      lcd.print("STOP ");
    }
    pracaS=praca;
  }  
 
// Obliczanie i wyświetlanie predkości obrotowej
  if (x!=xS)
  {
    t=((unsigned long)(1000000.00000/x));
    vobr=mnoznik*x;
    lcd.setCursor(4, 0);
    lcd.print("      ");
    lcd.setCursor(4, 0);
    lcd.print(x);
    xS=x;
  }

 // Generowanie i wyświetlanie kierunku ruchu
  if (kierunek!=kierunekS)
  {
      lcd.setCursor(5, 1);
      if(kierunek)
      {
        digitalWrite(DIR,HIGH);
        lcd.print("P");
      }
      else
      { 
        digitalWrite(DIR,LOW);
        lcd.print("L");
      }    
      kierunekS=kierunek;
  }

// realizacja ruchu
      ta=micros();
      if (ta<=ti+t)
      //if(ta<=ti+1000)
        digitalWrite(PUL,HIGH);
       else if((ta>ti+t)&&(ta<=ti+2*t))
        digitalWrite(PUL,LOW);
       else
         ti=micros();
    
}
void EEPROMWriteInt(int address, int value)
{
  byte two = (value & 0xFF);
  byte one = ((value >> 8) & 0xFF);
  
  EEPROM.update(address, two);
  EEPROM.update(address + 1, one);
}
 
int EEPROMReadInt(int address)
{
  long two = EEPROM.read(address);
  long one = EEPROM.read(address + 1);
 
  return ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
}
