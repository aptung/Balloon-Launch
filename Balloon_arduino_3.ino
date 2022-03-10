#include <math.h>

// CODE FOR HEATING PAD, BUZZER
const int heatingPin = 6; // put pin here
const int tempSensor = A3; // temperature sensor
const int buzzer = 9; //buzzer to arduino pin 9
int minTemp = -10; // in C
int maxTemp = 10;
int cycle = 0;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);

  // Set heating pin out
  pinMode(heatingPin, OUTPUT); 

  pinMode(tempSensor, INPUT);

  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output

}

void loop() {
  // Read temperature from the sensor 
  int val = analogRead(tempSensor); // digital read gives # from 0 - 1023
  Serial.print(val);
  Serial.print(": ");
  
  // t actual = -133 + 25.3 ln val
  //double temperature_c = -133 + 25.3 * log(val);
  double temperature_c = -30;
  Serial.println(temperature_c);
  
  // CODE FOR HEATING PAD
  if (temperature_c > maxTemp) {
    digitalWrite(heatingPin, LOW);
  }
  if (temperature_c < minTemp) {
    digitalWrite(heatingPin, HIGH);
    Serial.print("hello");
  } // END CODE FOR HEATING PAD

  if (cycle == 10) {
    cycle = 0;
    tone(buzzer, 3000); // optimal frequency as determined by testing that made grace (and everyone else) want to die
    delay(1000);        // ...for 1 sec
    noTone(buzzer);     // Stop sound...
  }
  cycle++;
  delay(1000);
}
