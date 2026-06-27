#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo gateServo;        // ENTRY gate
Servo exitServo;        // EXIT gate

// ---------------- PINS ----------------
#define t1 13
#define t2 10
#define t3 9
#define t4 8  

#define entrySensor 7  
#define exitSensor 3

#define buzzer 12

#define entryServoPin 11
#define exitServoPin 2

// ---------------- THRESHOLDS ----------------
int parkingThreshold = 150;
int alertThreshold = 50;
int entryThreshold = 100;
int exitThreshold  = 110;

// ---------------- SETUP ----------------
void setup() {

  lcd.init();
  lcd.backlight();

  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  gateServo.attach(entryServoPin);
  exitServo.attach(exitServoPin);

  gateServo.write(0); // entry gate closed
  exitServo.write(0);  // exit gate closed

  Serial.begin(9600);

  lcd.setCursor(0,0);
  lcd.print("SMART PARKING");
  delay(1000);
  lcd.clear();
}

// ---------------- SENSOR FUNCTION ----------------
long readDistance(int pin) {

  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delayMicroseconds(2);

  digitalWrite(pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin, LOW);

  pinMode(pin, INPUT);

  long duration = pulseIn(pin, HIGH, 30000);

  if (duration == 0) return 999;

  return duration * 0.034 / 2;
}

// ---------------- LOOP ----------------
void loop() {

  float d1 = readDistance(t1);
  float d2 = readDistance(t2);
  float d3 = readDistance(t3);
  float d4 = readDistance(t4);

  float entryDist = readDistance(entrySensor);
  float exitDist  = readDistance(exitSensor);

  // ---------------- SLOT STATUS ----------------
  bool s1 = (d1 > parkingThreshold);
  bool s2 = (d2 > parkingThreshold);
  bool s3 = (d3 > parkingThreshold);
  bool s4 = (d4 > parkingThreshold);

  int freeSlots = s1 + s2 + s3 + s4;

  // ---------------- BUZZER ----------------
  if (d1 < alertThreshold || d2 < alertThreshold ||
      d3 < alertThreshold || d4 < alertThreshold) {
    digitalWrite(buzzer, HIGH);
  } else {
    digitalWrite(buzzer, LOW);
  }

  // ---------------- ENTRY GATE CONTROL ----------------
  if (entryDist < entryThreshold && entryDist > 0 
      && freeSlots > 0)
  {
    gateServo.write(90);   // OPEN ENTRY
  } 
  else {
    gateServo.write(0);    // CLOSE ENTRY
  }

  // ---------------- EXIT GATE CONTROL (NEW) ----------------
  if (exitDist < exitThreshold && exitDist > 0)
  {
    exitServo.write(90);   // OPEN EXIT GATE
  } 
  else {
    exitServo.write(0);    // CLOSE EXIT GATE
  }

  // ---------------- LCD DISPLAY ----------------
  lcd.setCursor(0,0);
  lcd.print("FREE:");
  lcd.print(freeSlots);
  lcd.print(" SLOTS   ");

  lcd.setCursor(0,1);

  if (s1 || s2 || s3 || s4) {

    if (s1) lcd.print("S1 ");
    else lcd.print("-- ");

    if (s2) lcd.print("S2 ");
    else lcd.print("-- ");

    if (s3) lcd.print("S3 ");
    else lcd.print("-- ");

    if (s4) lcd.print("S4 ");
    else lcd.print("-- ");

  } else {
    lcd.print("PARKING FULL   ");
  }

  delay(500);
}