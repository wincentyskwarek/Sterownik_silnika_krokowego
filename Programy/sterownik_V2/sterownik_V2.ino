
#include <LiquidCrystal.h>

//Sterownik TB6600
#define KR A1 //definicja pinu kierunku ruchu
#define START 3 //definicja start
#define SPEED 11 //definicja speed
// Impulsator

// initializacja LCD
const int rs = 12, en = 13, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int odczyt, odczytS, x=1, xS; // odczyt z potencjometru

int PUL=9; //definicja pinów pulsu
int DIR=8; //definicja kierunków ruchu
int ENA=10; //definicja zatrzymania wału
// definicja ustawień impulsatora
 int pinA = 2;  // podłączenie do CLK na KY-040
 int pinB = A2;  //podłączenie do DT na KY-040
 int pinSW = A3; 
 int encoderPosCount = 0; 
 int pinALast;  
 int aVal;
 boolean bCW;
 unsigned long tk, tp;


float vobr=00.00;
int kierunek, kierunekS;
int praca, pracaS;
int spid, spidS;
int t;
const int minimum=1; //dedinicja minimum szybkości (odwtotność czasu impulsu)
const int maksimum=1000; //dedinicja minimum szybkości (odwtotność czasu impulsu)
const float mnoznik=0.1398; //mnożnik do linearyzacji 0.1473
void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("ElMechPlast");
  //delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("imp:    ");
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
        lcd.print("Prawy");
      }
      else
      { 
        digitalWrite(DIR,LOW);
        lcd.print("Lewy ");
      }  
 //ustalenie prędkości startowej
   odczyt = 1;
   if(x>=100)
      t=((int)(1000000.00000/x));
   else
      t=(int)(1000.00000/x);
   vobr=mnoznik*x;
   lcd.setCursor(4, 0);
   lcd.print("      ");
   lcd.setCursor(4, 0);
   lcd.print(x);
   
// impulsator START
   pinMode (pinA,INPUT);
   pinMode (pinB,INPUT);
   pinMode (pinSW,INPUT);
   /* Read Pin A
   Whatever state it's in will reflect the last position   
   */
   attachInterrupt(0, impulsator, CHANGE);
/*  
// Wyświetlenie NORM./MAXI
  //spid = digitalRead(SPEED);
  lcd.setCursor(11, 1);
  lcd.print("INIT. ");
  x=1;
  spidS = digitalRead(SPEED);
//  if (spid==LOW)
//   lcd.print("MAXI. ");
//  else
//    lcd.print("NORM."); 



  */
 //   Serial.begin(9600);
//  Serial.println("Basic Encoder Test:");
}

void loop() {
  Serial.println(digitalRead(pinA));
// odczyty przełączników 
  kierunek=digitalRead(KR);
  spid = digitalRead(SPEED);
  praca=digitalRead(START);
// Praca impulsatora
  

  
  odczyt = encoderPosCount;
  //odczyt=1000;

// Generowanie i wyświetlanie prędkości oraz odczyt potencjometru
  if((spid!=spidS) || (odczyt!=odczytS))
  {
    lcd.setCursor(11, 1);
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
    if(x>=100)
      t=((int)(1000000.00000/x));
    else
      t=(int)(1000.00000/x);
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
        lcd.print("Prawy");
      }
      else
      { 
        digitalWrite(DIR,LOW);
        lcd.print("Lewy ");
      }    
      kierunekS=kierunek;
  }
  

  
// realizacja ruchu
      digitalWrite(PUL,HIGH);
      if(x>=100)
        delayMicroseconds(t);
      else
        delay(t);
      digitalWrite(PUL,LOW);
      if(x>=100)
        delayMicroseconds(t);
      else
        delay(t);
    
}
void impulsator()
{
  aVal = digitalRead(pinA);
     if (digitalRead(pinB) != aVal)   // Means pin A Changed first - We're Rotating Clockwise
     {
       
       if (encoderPosCount<maksimum) encoderPosCount ++;
      } 
     else // Otherwise B changed first and we're moving CCW
     {
       if (encoderPosCount>minimum) encoderPosCount--;
     } 
     delay(10);
}

