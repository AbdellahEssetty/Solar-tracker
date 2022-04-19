#include <Arduino.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include <Wire.h>

//Working functions.

int measureLight(Servo& servo, const int& angle, const int& lightPin);
void displayTest(const int& light, bool right = true);
int getPerfectAngle(Servo& servo, int& angle, const int& lightPin);
void getVector(const int& tL, const int& tR, const int& bL, const int& bR, int& angleX, int& angleZ);


// LCD display
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Phothoresistors pin and values.
// const int lightPinX = A4;
// const int lightPinZ = A5;

const int topLeftPin = A4;
const int topRightPin  = A5;
const int botLeftPin = A6;
const int botRightPin = A7;

int lightValX = 0;
int lightValZ = 0;

// Servo Motors
Servo servoX;
Servo servoZ;

const int servoXPin = A0;
const int servoZPin = A1;

int angleX = 90;
int angleZ = 90;

void setup() {
  // put your setup code here, to run once:

  pinMode(topLeftPin, INPUT);
  pinMode(topRightPin, INPUT);
  pinMode(botLeftPin, INPUT);
  pinMode(botLeftPin, INPUT);

  servoX.attach(servoXPin);
  servoZ.attach(servoZPin);

  servoX.write(90);
  servoZ.write(90);

  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.print("Helios total eng");

  delay(3000);
}

void loop() {
  // put your main code here, to run repeatedly:


  getVector(topLeftPin, topRightPin, botLeftPin, botRightPin, angleX, angleZ);
  servoX.write(angleX);
  servoZ.write(angleZ);


  lcd.setCursor(0, 1);
  lcd.print(millis()  / 1000);
  lcd.print(" J");


  delay(100);
}

//Transform the data from the photoresistors into a vector indicating the direction of the movement to track.
void getVector(const int& tL, const int& tR, const int& bL, const int& bR, int& angleX, int& angleZ)
{
  int topLeft = analogRead(tL);
  topLeft = map(topLeft, 0, 1023, -10, 10);
  int topRight = analogRead(tR);
  topRight = map(topRight, 0, 1023, -10, 10);
  int botLeft = analogRead(bL);
  botLeft = map(botLeft, 0, 1023, -10, 10);
  int botRight = analogRead(bR);
  botRight = map(botRight, 0, 1023, -10, 10);

  int x = (topRight - topLeft) + (botRight - botLeft);
  int z = (topRight + topLeft) - (botRight + botLeft);

  x = map(x, -20, 20, -5, 5);
  z = -map(z, -20, 20, -5, 5);

  if( (abs(angleX + x)% 180) <= 180 &&  (abs(angleX + x)% 180) >= 0) 
    angleX = ( (abs(angleX + x))% 180);
  if( ((abs(angleZ + z)) % 180) <= 180 && ( (abs(angleZ + z)) % 180) >= 0)
    angleZ = ( (abs(angleZ + z)) % 180);

  Serial.print(x);
  Serial.print('\t');
  Serial.println(angleX);

  Serial.print(z);
  Serial.print('\t');
  Serial.println(angleZ);

  Serial.println('\t');


  delay(100);

  // Serial.print(topLeft);
  // Serial.print('\t');
  // Serial.println(topRight);


  // Serial.print(botLeft);
  // Serial.print('\t');
  // Serial.println(botRight);

  // Serial.println('\n');
}

//Get the angle where the most light comes from.
//It does work on one axis. 
int getPerfectAngle(Servo& servo, int& angle, const int& lightPin)
{
  int direction = measureLight(servo, angle, lightPin);

  while(direction != 0)
  {

    if(direction == 1)
    { 
      Serial.println("Turning right");
      angle += 15;
    }
    else 
    {
      Serial.println("Turning left");
      angle -= 15;
    }
    direction = measureLight(servo, angle, lightPin);
    delay(1000);
  }
  return angle;
}

//measure the light coming to photonresistor.
// return the side with the most light, -1 for left, 1 for right, 0 for perfect positioning (0 => no changes needed).
int measureLight(Servo& servo, const int& angle, const int& lightPin)
{

  int curr = analogRead(lightPin);
  int left = 0;
  int right = 0;
  Serial.print("Current light\t");
  Serial.println(curr);

  delay(1000);
  //Turn right and measure the light.
  servo.write(angle - 5);
  right = analogRead(lightPin);
  displayTest(right, true);

  delay(1000);
  //Turn left and measure light.
  servo.write(angle + 5);
  left = analogRead(lightPin);
  displayTest(left, false);

  delay(1000);
  if(left > curr + 1)
    return -1;
  else if(right > curr + 1)
    return 1;
  
  return 0;

}

//Display result of the test. Just for testing.
void displayTest(const int& light, bool right)
{
  const char* s = right ? "right\t" : "left\t";
  Serial.print(s);
  Serial.println(light);
  return;
}