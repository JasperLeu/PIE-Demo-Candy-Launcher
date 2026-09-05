#include <Servo.h>

Servo aim; // 90 = center, 0 = right, 180 = left
Servo winch; // 0-89 = wind, 91-180 = unwind
Servo feeder; // 180 = recieve, ~90 = load
Servo latch; // 5 = open, 35 = closed

int WIND_TIME = 1000;
int WIND_SPEED = 30;
bool reloaded = false;

void setup() {
  Serial.begin(9600);
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
  fire(0);
}
void loop() 
{
  
}

void fire(int angleFromCenter) // positive is clockwise
{
  if (!reloaded)
    reload();
  int newAngle = constrain(90 - angleFromCenter, 0, 180);
  aim.write(newAngle);
  delay(1000);
  windWinch(-1);
  delay(500);
  openLatch();
  delay(1000);
  reloaded = false;
}

void reload() // put new candy in catapult
{
  windWinch(1);
  delay(200);
  closeLatch();
  delay(1000);
  loadNewCandy();
  delay(2000);
  reloaded = true;
}
void windWinch(int direction)
{
  winch.write(90-WIND_SPEED*direction);
  delay(WIND_TIME);
  winch.write(90);
}
void closeLatch()
{
  latch.write(35);
}
void openLatch()
{
  latch.write(5);
}

void loadNewCandy()
{
  feeder.write(180);
  delay(2000);
  feeder.write(90);
}