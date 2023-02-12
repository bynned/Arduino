#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //LCD pins
const int sensorPin = A0; //Initializing temperature sensor
void setup() {
  // LCD Setups.
  lcd.begin(16, 2);
  lcd.print("Room Temperature: ");
  Serial.begin(9600);
}

void loop() {
  
  // Declaring array for temperature values
  double tempValues[30]; 
  double sum = 0;

  for(int i = 0; i < sizeof(tempValues) / sizeof(double); i++){
    int sensorVal = analogRead(sensorPin);
    //------- Calculations for temperature---------
    float voltage = (sensorVal/1024.0) * 5.0;
    float temperature = (voltage - .5) * 100;
    //---------------------------------------------
    tempValues[i] = temperature;
    delay(200);
  }

  // Find the sum of array
  for(int i = 0; i < 30; i++){
    sum = sum + tempValues[i];
  }

  // Calculate average
  float averageTemperature = sum / 30;
  Serial.println(averageTemperature);
  
  lcd.setCursor(6, 1);
  lcd.print(averageTemperature);


}
