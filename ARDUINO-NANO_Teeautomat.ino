// Teeolino by @woodworking.dennis
// Juni 2018
/* 
 *  Bibliotheken einbinden
 */
#include <Servo.h>  //Servo-Bibliothek
#include <LiquidCrystal.h> //LCD-Bibliothek

/*
 * Konstanten definieren
 */
#define BUZZER 10
#define SERVO 9
#define TASTER 8
#define POTI A0
#define HOCH 180
#define RUNTER 100
#define PARKPOS 180

/*
 * Variablen deklarieren
 */
int zustand = 0;                        //Aktueller Zustand, 0 = Initialisierung
long zeit = 0;                          //Speichern von Start- und Endzeiten
int ziehzeit = 0;                       //Eingestellte Ziehzeit
int ziehzeitMin = 0, ziehzeitSek = 0;   //Ziehzeit in Minuten und Sekunden
int potiWert = 0; //Poti-Position
byte servoPos = 0;
long tasterdruckZeit = 0;               //Variable zur unterscheidung von langem und kurzem Tasterdruck

Servo servo;                            //Servo Objekt anlegen und benennen

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;       //Displayanschlusspins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);                        //Display Objekt mit Pin-Angabe anlegen

void setup() {
  // put your setup code here, to run once:
  /*
   * Ein- und Ausgänge initialisieren
   */
   pinMode(TASTER, INPUT);
   pinMode(BUZZER, OUTPUT);
   pinMode(POTI, INPUT);

   servo.attach(SERVO);                 //Servo an den SERVO-Pin binden
   lcd.begin(16, 2);                    //Display Objekt initialisieren

   servo.write(servoPos);               //Servoarm hoch
   delay(100);

   //Startnachricht
   lcd.setCursor(4, 0);
   lcd.print("Teeolino");
   lcd.setCursor(0, 1);
   lcd.print("by Dennis Wagner");
   delay(3000);
   lcd.clear();
   lcd.print("Technik-Projekt");
   lcd.setCursor(6, 1);
   lcd.print("2018");
   delay(3000);
   
   zustand = 1; //in "Einstellungs"-Zustand gehen

}

void loop() {
  // put your main code here, to run repeatedly:
  /*
   * Zustandsabhängige Aktionen
   */
   if(zustand == 1){
    potiWert = analogRead(POTI);                //Potiposition in Variable speichern
    
    ziehzeit = map(potiWert, 0, 970, 0, 600);   //Poti-Position in Zeit umrechnen
    ziehzeitSek = ziehzeit % 60;                //Sekundenanteil = Rest von ziehzeit / 60
    ziehzeitMin = (ziehzeit - ziehzeitSek)/60;  //Minuten = ziehzeit - Sekundenanteil
    
    lcd.clear();  //LCD rücksetzen
    lcd.print("Zeit einstellen!");
    lcd.setCursor(0, 1);
    lcd.print(ziehzeitMin);
    lcd.print(":");
    if(ziehzeitSek < 10){                       //Wenn Sekunden einstellig null auffüllen
      lcd.print("0");
    }
    lcd.print(ziehzeitSek);
   
    if(ziehzeitMin == 1){
      lcd.print(" Minute");
    }
    else{
      lcd.print(" Minuten");
    }
       
    delay(250);
    
    if(digitalRead(TASTER) == HIGH){            //Bei Tasterdruck Zustand wechseln
      zustand = 2;
    }
   }
   
   else if(zustand == 2){
    lcd.clear();
    lcd.print("Tee zieht noch:");
    
    while(servoPos < RUNTER){                   //Servo schrittweise runterfahren bis #define RUNTER
      servoPos = servoPos + 1;
      servo.write(servoPos);
      delay(5);
    }

    while(ziehzeit > 0){
      ziehzeitSek = ziehzeit % 60;                //Sekundenanteil = Rest von ziehzeit / 60
      ziehzeitMin = (ziehzeit - ziehzeitSek)/60;  //Minuten = ziehzeit - Sekundenanteil
      
      lcd.setCursor(0, 1);
      lcd.print(ziehzeitMin);
      lcd.print(":");
      if(ziehzeitSek < 10){                       //Wenn Sekunden einstellig null auffüllen
       lcd.print("0");
      }
      lcd.print(ziehzeitSek);
   
      if(ziehzeitMin == 1){
       lcd.print(" Minute");
      }
      else{
       lcd.print(" Minuten");
      }
      
      //Verbleibende Zeit im Sekundentakt verringern
      delay(1000);
      ziehzeit = ziehzeit - 1; 
    }

    //Endzeit anzeigen
    lcd.setCursor(0, 1);
    lcd.print("0:00 Minuten");
    
     //Tunkvorgang
    for(int i = 0; i < 4; i++){                     //Variable i anlegen um Durchlaufanzahl zu speichern
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("Tunken");
      servo.write(RUNTER);
      delay(500);
      servo.write(HOCH);
      delay(500);
    }
    
    zustand = 3;                                    //In Entnahmezustand wechseln
   }
   
   else if(zustand == 3){
    
     while(servoPos > 0){                           //Servo schrittweise hochfahren bis 0 Grad
      servoPos = servoPos - 1;
      servo.write(servoPos);
      delay(15);
      digitalWrite(BUZZER, HIGH);
    }
    digitalWrite(BUZZER, LOW);

    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Tee ist fertig");
    delay(250);

    if(digitalRead(TASTER) == HIGH){
      while(digitalRead(TASTER) == HIGH){
        tasterdruckZeit = tasterdruckZeit + 1;
        delay(100);
      }
      if(tasterdruckZeit > 10){
        zustand = 4;        
      }
      else{
        zustand = 1;
        delay(250);
      }
    }
   }
   else if(zustand == 4){
    //Ausschaltzustand
      servo.write(PARKPOS);                       //Servo in "Park-Position"
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Ausschalten!");
      while(1);                                   //Endlosschleife
   }
}
