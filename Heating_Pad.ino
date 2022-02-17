#include <math.h>

// CODE FOR HEATING PAD
const int heatingPin = 6; // put pin here
const int tempSensor = A3; // temperature sensor
int minTemp = -10; // in C
int maxTemp = 10;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);

  // Set heating pin out
  pinMode(heatingPin, OUTPUT); 

  pinMode(tempSensor, INPUT);

}

void loop() {
  // Read temperature from the sensor 
  int val = analogRead(tempSensor); // digital read gives # from 0 - 1023
  Serial.print(val);
  Serial.print(": ");
  
  // t actual = -133 + 25.3 ln val
  double temperature_c = -133 + 25.3 * log(val);
  Serial.println(temperature_c);
  
  // CODE FOR HEATING PAD
  if (temperature_c > maxTemp) {
    digitalWrite(heatingPin, LOW);
  }
  if (temperature_c < minTemp) {
    digitalWrite(heatingPin, HIGH);
    Serial.print("hello");
  } // END CODE FOR HEATING PAD

  delay(1000);
}
