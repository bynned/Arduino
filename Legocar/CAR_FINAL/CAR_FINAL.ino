#include <Wire.h>
#include <LiquidCrystal.h>
#include <LIDARLite.h>
#define CMPS14_address 0x60
#define SDA 20
#define SCL 21
#include <math.h>
#define Motor_L_dir_pin       7
#define Motor_R_dir_pin       8
#define Motor_L_pwm_pin       9
#define Motor_R_pwm_pin       10
LIDARLite myLidarLite;

byte raw;
int motorDir, vehicle_direction, vehicle_turningDir;
    int pwm_R;
    int pwm_L;
    
LiquidCrystal lcd(37, 36, 35, 34, 33, 32);

//Values for reading the serial monitor
String message = "";
String command = "";
int value = 0;

//Encoder for pulses
#define ENC_IN_RIGHT_B 23
#define ENC_IN_RIGHT_A 3
#define ENC_IN_LEFT_B 24
#define ENC_IN_LEFT_A 2

//Distance
volatile int right_wheel_pulse_count = 0;
volatile int left_wheel_pulse_count = 0;
int stepsCounterL = 0;
int stepsCounterR = 0;
double totalDistR, totalDistL;
double distPerPulse = 0.1; //cm
double distPulsePer4 = distPerPulse * 4;

void setup()
{
  Wire.begin();
    lcd.begin(20, 4);
    Serial.begin(115200);
    
  pinMode(ENC_IN_LEFT_A, INPUT);
  pinMode(ENC_IN_LEFT_B, INPUT);
  pinMode(ENC_IN_RIGHT_A, INPUT);
  pinMode(ENC_IN_RIGHT_B, INPUT);

  attachInterrupt(digitalPinToInterrupt(ENC_IN_RIGHT_A), right_wheel_pulse, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_IN_LEFT_A), left_wheel_pulse, RISING);
  myLidarLite.begin(0, true);
  myLidarLite.configure(0);
}

void loop()                    
{          
  Wire.beginTransmission(CMPS14_address);
  Wire.write(1);
  Wire.endTransmission(false);
  Wire.requestFrom(CMPS14_address, 1, true);
  if (Wire.available() >= 1)
    raw = Wire.read();

  vehicle_direction = map(raw, 0, 255, 0, 360);

  digitalWrite(Motor_R_dir_pin, 1);
  digitalWrite(Motor_L_dir_pin, 1);

  analogWrite(Motor_L_pwm_pin, pwm_L);
  analogWrite(Motor_R_pwm_pin, pwm_R);

    while (Serial.available() > 0) {
    message = Serial.readStringUntil('\n');
    command = message.substring(0, message.indexOf(":"));
    value = message.substring(message.indexOf(":") + 1).toInt();

    lcd.setCursor(0, 0);
    lcd.print(message);
    lcd.setCursor(0, 1);
    lcd.print(command);
    lcd.setCursor(0, 2);
    lcd.print(value);

    lcd.setCursor(0, 3);
    lcd.print(right_wheel_pulse_count);
    }

    if(command == "Move"){
      Move(value);
      command = "";
    }
    else if(command == "Turn" && value > 0){
      TurnRight(value);
      command = "";
    }
    else if(command == "Turn" && value < 0){
      TurnLeft(value);
      command = "";
    }
    
   pwm_L = 0;
   pwm_R = 0;

   digitalWrite(Motor_R_dir_pin,1);
   digitalWrite(Motor_L_dir_pin,1);

   analogWrite(Motor_L_pwm_pin,pwm_L);
   analogWrite(Motor_R_pwm_pin,pwm_R);
}

void Move(int moveValue) {
    int positiveMoveValue = abs(moveValue);
   if(moveValue > 0)
      motorDir = 1;
      else
      motorDir = 0;
    
   do{
   pwm_L = 100;
   pwm_R = 100;
   digitalWrite(Motor_R_dir_pin,motorDir);
   digitalWrite(Motor_L_dir_pin,motorDir);

   analogWrite(Motor_L_pwm_pin,pwm_L);
   analogWrite(Motor_R_pwm_pin,pwm_R);
   } while(right_wheel_pulse_count * distPerPulse < positiveMoveValue);
   
   pwm_L = 0;
   pwm_R = 0;
   right_wheel_pulse_count = 0;
   left_wheel_pulse_count = 0;
   readValues();
}

void TurnLeft(int turnValue) {
  int positiveTurnValue = abs(turnValue);
  do {
    Wire.beginTransmission(CMPS14_address);
    Wire.write(1);
    Wire.endTransmission(false);
    Wire.requestFrom(CMPS14_address, 1, true);
    if (Wire.available() >= 1)
      raw = Wire.read();

    vehicle_turningDir = map(raw, 0, 255, 0, 360);
    pwm_L = 200;
    pwm_R = 0;
    digitalWrite(Motor_R_dir_pin, 1);
    digitalWrite(Motor_L_dir_pin, 1);
    analogWrite(Motor_L_pwm_pin, pwm_L);
    analogWrite(Motor_R_pwm_pin, pwm_R);
  } while (vehicle_turningDir > vehicle_direction - turnValue);
  pwm_L = 0;
  pwm_R = 0;
  right_wheel_pulse_count = 0;
  left_wheel_pulse_count = 0;
  readValues();
}

void TurnRight(int turnValue) {
  int positiveTurnValue = abs(turnValue);
  do {
    Wire.beginTransmission(CMPS14_address);
    Wire.write(1);
    Wire.endTransmission(false);
    Wire.requestFrom(CMPS14_address, 1, true);
    if (Wire.available() >= 1)
      raw = Wire.read();

    vehicle_turningDir = map(raw, 0, 255, 0, 360);
    pwm_L = 0;
    pwm_R = 200;
    digitalWrite(Motor_R_dir_pin, 1);
    digitalWrite(Motor_L_dir_pin, 1);
    //Serial.println(vehicle_turningDir);
    //Serial.println(vehicle_direction);
    analogWrite(Motor_L_pwm_pin, pwm_L);
    analogWrite(Motor_R_pwm_pin, pwm_R);
  } while (vehicle_turningDir < vehicle_direction + turnValue);
  pwm_L = 0;
  pwm_R = 0;
  left_wheel_pulse_count = 0;
  right_wheel_pulse_count = 0;
  readValues();
}
void readValues() {
  Wire.beginTransmission(CMPS14_address);
  Wire.write(1);
  Wire.endTransmission(false);
  Wire.requestFrom(CMPS14_address, 1, true);
  if(Wire.available() >= 1)
    raw = Wire.read();


   
  vehicle_direction = map(raw, 0, 255, 0, 360);

  int lidarValue = myLidarLite.distance();
  char resp[80];
  int n = sprintf(resp, "Lid=%d Com=%d", lidarValue, vehicle_direction);
  Serial.println(resp);
  delay(100);
}

void right_wheel_pulse() {
  right_wheel_pulse_count++;  
}
void left_wheel_pulse() {
  left_wheel_pulse_count++;
}