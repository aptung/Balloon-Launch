#include <math.h>

const int heatingPin = 6;
const int tempSensorPin = A3;
const int buzzerPin = 9;

unsigned long timeTemperature;
unsigned long timeBuzzer;

double temperature_c;
int temperature_val;

int minTemp = -10; // in C
int maxTemp = 10;

int cycle = 0;


void setup() {
  Serial.begin(9600);

  pinMode(heatingPin, OUTPUT);
  pinMode(tempSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  timeTemperature = millis();
  timeBuzzer = millis();
}

void loop() {
  if (millis()-timeTemperature>1000){
    timeTemperature = millis();
    getTemperature();
    adjustHeating();
  }
  
  if (millis()-timeBuzzer>1000){
    timeBuzzer = millis();
    buzzer();
  }
}

void getTemperature() {
  temperature_val = analogRead(tempSensorPin);
//  Serial.print("Temperature voltage= ");
//  Serial.println(temperatureVal);
  
  // t actual = -133 + 25.3 ln val
  temperature_c = -133 + 25.3 * log(temperature_val);

  Serial.print("(");
  Serial.print(millis());
  Serial.print(")");

  Serial.print("Temperature (C) = ");
  Serial.println(temperature_c);
}

void adjustHeating() {
  if (temperature_c > maxTemp) {
    digitalWrite(heatingPin, LOW);
  }
  if (temperature_c < minTemp) {
    digitalWrite(heatingPin, HIGH);
  }
}

void buzzer(){
  if (cycle == 10) {
    cycle = 0;
    tone(buzzerPin, 3000); // optimal frequency as determined by testing that made grace (and everyone else) want to die
  }
  else if (cycle==0){
    noTone(buzzerPin);
    cycle++;
  }
  else{
    cycle++;
  }
}
