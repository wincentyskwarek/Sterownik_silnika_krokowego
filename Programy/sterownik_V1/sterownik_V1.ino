
#include <LiquidCrystal.h>
#include <Encoder.h>
// Definicja pinów sterownika TB6600
#define KR A1 //definicja pinu kierunku ruchu
#define START 3 //definicja start
#define SPEED 11 //definicja speed
// Definicja pinów impulsatora
#define CLK 2 
#define DT A2 
#define SW A3 
// initializacja LCD
const int rs = 12, en = 13, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int odczyt, odczytS, x=1, xS; // odczyt z potencjometru

int PUL=9; //definicja pinów pulsu
int DIR=8; //definicja kierunków ruchu
int ENA=10; //definicja zatrzymania wału

// definicja ustawień impulsatora
Encoder myEnc(DT, CLK);
long oldPosition  = -999;

float vobr=00.00;
int kierunek, kierunekS;
int praca, pracaS;
int spid, spidS;
int t;
const int minimum=1; //dedinicja minimum szybkości (odwtotność czasu impulsu)
const int maksimum=1000; //dedinicja minimum szybkości (odwtotność czasu impulsu)
const float mnoznik=0.1398; //mnożnik do linearyzacji 0.1473
//const int czulosc=1; //czułość potencjometra
unsignet long czas;
void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("ElMechPlast");
  //delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("rpm:00.00");
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
   x=map(odczyt,0,1023,minimum,maksimum);
   if(x>=100)
      t=((int)(1000000.00000/x));
   else
      t=(int)(1000.00000/x);
   vobr=mnoznik*x;
   lcd.setCursor(4, 0);
   lcd.print("      ");
   lcd.setCursor(4, 0);
   lcd.print(vobr);
   

  
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

}

void loop() {
// odczyty przełączników 
  kierunek=digitalRead(KR);
  spid = digitalRead(SPEED);
  praca=digitalRead(START);
// Praca impulsatora
  
long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
  odczyt = newPosition;
  //odczyt=1000;
  }

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
      x=map(odczyt,0,1023,minimum,maksimum);
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

