#include <Servo.h>
#include <math.h>

Servo aim; // 90 = center, 0 = right, 180 = left
Servo winch; // 0-89 = wind, 91-180 = unwind
Servo feeder; // 180 = recieve, ~90 = load
Servo latch; // 5 = open, 35 = closed

const int WIND_TIME = 1000;
const int WIND_SPEED = 30;
const int BEEP_DELAY = 300;
const int BEEP_TIME = 100;
const int SHOT_DELAY = 3000;

// calculating angle
float camFOV = 75.8; // fov of camera used
float offsetX = 0;
float offsetY = 0; // x, y offset from catapult pivot to camera in ft.
float targetX = 0; // (right=+x, left=-x, forward=+y, backwards=-y)
float targetY = 0; // position of targeted person

// switch and buzzer
const int switchPin = 8
const int buzzer = 7;

float lastBeep;
float lastShot;

void setup() {
  Serial.begin(9600);

  pinMode(switchPin, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT)

  aim.attach(12);
  aim.write(40); // center
  delay(500);

  winch.attach(11);
  winch.write(90); // stopped
  delay(500);

  feeder.attach(10);
  feeder.write(90); // above ramp
  delay(500);

  latch.attach(9);
  latch.write(5); // open
  delay(1000);
  
  reload();
  delay(500);
  lastShot = millis();
}
void loop() 
{
  if (Serial.available()) // is there an input of a detected person
  {
    int angle = Serial.parseFloat() * PI / 180;
    int dist = Serial.parseFloat();
    Serial.read(); // clear \n at end of line
    targetX = dist * tan(angle) - offsetX;
    targetY = dist - offsetY;
    int angleToTarget = constrain(90 - math.atan(targetX/targetY)*180/PI, 0, 180);
    aim.write(angleToTarget);
    // shoot after delay
    if (digitalRead(switchPin, LOW))
    {
      // beeping
      if (millis() - lastBeep > BEEP_DELAY)
        digitalWrite(buzzer, HIGH);
      if (millis() - lastBeep > BEEP_DELAY + BEEP_TIME)
      {
        digitalWrite(buzzer, LOW);
        lastBeep = millis();
      }
      // shooting
      if (millis() - lastShot > SHOT_DELAY)
      {
        digitalWrite(buzzer, LOW);
        fire(math.atan(targetX/targetY)*180/PI);
        delay(1500);
        lastShot = millis();
      }
    }
    else if(digitalRead(switchPin, HIGH)) // continously reset timer if not active
      lastShot = millis();
  }
}

void fire(int angleFromCenter) // positive is clockwise
{
  int newAngle = constrain(90 - angleFromCenter, 0, 180);
  aim.write(newAngle);
  delay(500);
  windWinch(-1);
  delay(500);
  openLatch();
  delay(1500);
  reload();
}

void reload() // put new candy in catapult
{
  aim.write(90);
  delay(500);
  windWinch(1);
  delay(500);
  closeLatch();
  delay(1000);
  loadNewCandy();
  reloaded = true;
}
void windWinch(int direction) // 1 is to wind, -1 to unwind
{
  winch.write(90-WIND_SPEED*direction);
  delay(WIND_TIME);
  winch.write(90);
}
void loadNewCandy()
{
  feeder.write(180);
  delay(2000);
  feeder.write(90);
}
void closeLatch()
{
  latch.write(35);
}
void openLatch()
{
  latch.write(5);
}
