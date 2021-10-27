#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {23, 21, 3, 22}; //connect to the row pinouts of the kpd
byte colPins[COLS] = {19, 18, 5, 17}; //connect to the column pinouts of the kpd

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
const String password = "1234";
const String password2 = "4321";
String input_password;

char auth[] = ""; // auth token

char ssid[] = "user";
char pass[] = "password";

const int m1 = 15;
const int m2 = 2;
const int L = 4;
int a = 99;

//ultrasonic 1
const int pinTrig1 = 14;
const int pinEcho1 = 12;
//ultrasonic 2
const int pinTrig2 = 32;
const int pinEcho2 = 33;

double echoTime;
float jarak1, jarak2;

WidgetLCD lcd(V2);

BLYNK_WRITE(V1) {
  a = param.asInt();
}

const int tombolBuka = 27;
const int tombolTutup = 26;

int statusBuka = 0;
int statusTutup = 0;

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);
  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);
  pinMode(4, OUTPUT);

  //  ultrasonic 1
  pinMode(pinTrig1, OUTPUT);
  pinMode(pinEcho1, INPUT);

  //  ultrasonic 2
  pinMode(pinTrig2, OUTPUT);
  pinMode(pinEcho2, INPUT);

  pinMode(tombolBuka, INPUT);
  pinMode(tombolTutup, INPUT);

  digitalWrite(m1, HIGH);
  digitalWrite(m2, HIGH);

  input_password.reserve(32);

}

void loop()
{
  Blynk.run();

  char key = keypad.getKey();

  //ultrasonic 1
  digitalWrite(pinTrig1, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig1, LOW);
  echoTime = pulseIn(pinEcho1, HIGH);
  jarak1 = 0.0001 * ((float)echoTime * 340.0) / 2.0;

  //ultrasonic 2
  digitalWrite(pinTrig2, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig2, LOW);
  echoTime = pulseIn(pinEcho2, HIGH);
  jarak2 = 0.0001 * ((float)echoTime * 340.0) / 2.0;

  if (a == 1 ) {
    digitalWrite(L, HIGH);
    Serial.println("Buka");
    lcd.clear();
    lcd.print(0, 0, "Sedang Dibuka");
    digitalWrite(m1, HIGH);
    digitalWrite(m2, LOW);
    delay(2000);
    Serial.println(jarak1);
    Serial.println(jarak2);
    a = 2;
  } else if (a == 0) {
    Serial.println("tutup");
    lcd.clear();
    lcd.print(0, 0, "Sedang Ditutup");
    digitalWrite(m2, HIGH);
    digitalWrite(m1, LOW);
    delay(2000);
    Serial.println(jarak1);
    Serial.println(jarak2);
    a = 3;
  }

  if (jarak1 < 10 && jarak2 < 10 && a == 2) {
    Serial.println("sudah terbuka");
    digitalWrite(m2, HIGH);
    digitalWrite(m1, HIGH);
    delay(1000);
    lcd.clear();
    lcd.print(0, 0, "Sudah Terbuka");
    delay(1000);
    a = 4;
  } else if (jarak2 > 10 && jarak1 > 10 && a == 3) {
    Serial.println("Sudah Tertutup");
    digitalWrite(m2, HIGH);
    digitalWrite(m1, HIGH);
    delay(5000);
    digitalWrite(L, LOW);
    lcd.clear();
    lcd.print(0, 0, "Sudah Tertutup");
    delay(1000);
    a = 5;
  }

  if ( a == 4) {
    Blynk.notify("Pagar Anda Sudah Terbuka");
    delay(1000);
    a = 6;
  } else if (a == 5) {
    Blynk.notify("Pagar Anda Sudah Tertutup");
    delay(1000);
    a = 7;
  }


  if (key) {
    Serial.println(key);
    if (key == '*') {
      input_password = ""; // clear input password
    } else if (key == '#') {
      if (password == input_password) {
        Serial.println("password is correct");
        digitalWrite(L, HIGH);
        Serial.println("Buka dengan password");
        a = 1;
        lcd.clear();
        lcd.print(0, 0, "Sedang Dibuka");
        digitalWrite(m1, HIGH);
        digitalWrite(m2, LOW);
        delay(2000);
        a = 2;

      } else if (password2 == input_password) {

        Serial.println("password is correct");
        Serial.println("Tutup dengan password");
        a = 0;
        lcd.clear();
        lcd.print(0, 0, "Sedang Ditutup");
        digitalWrite(m2, HIGH);
        digitalWrite(m1, LOW);
        delay(2000);
        a = 3;
      } else {
        Serial.println("password is incorrect, try again");
      }

      input_password = ""; // clear input password
    } else {
      input_password += key; // append new character to input password string
    }
  }

  if (statusBuka == HIGH) {
    digitalWrite(L, HIGH);
    a = 1;
    lcd.clear();
    lcd.print(0, 0, "Sedang Dibuka");
    digitalWrite(m1, HIGH);
    digitalWrite(m2, LOW);
    delay(2000);
    a = 2;
  }

  if (statusTutup == HIGH) {
    a = 0;
    lcd.clear();
    lcd.print(0, 0, "Sedang Ditutup");
    digitalWrite(m2, HIGH);
    digitalWrite(m1, LOW);
    delay(2000);
    a = 3;
  }


}
