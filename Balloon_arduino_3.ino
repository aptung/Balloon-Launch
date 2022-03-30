#include <math.h> // For doing math in arduino


// Create variables to store results (these are updated periodically)
double temperature_c; // Actual temperature, in C
int temperatureVal; // For storing the voltage that was read in

// These are unsigned to increase the range
// Each of these is updated at different times for each component, so that the timing
// for each component is independent
unsigned long timeTemperature, timeBuzzer;

// Pin labels
const int heatingPin = 6;
const int tempSensorPin = A3;
const int buzzerPin = 9;

// Thresholds for adjusting the heating pad, in C
int minTemp = -10;
int maxTemp = 10;

// Used for timing the buzzer's on/off
int cycle = 0;


void setup() {
  Serial.begin(9600); // Initialize Serial Monitor

  // Initialize pins
  pinMode(heatingPin, OUTPUT);
  pinMode(tempSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  // Start timers
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

void getTemperature(){
  temperatureVal = analogRead(tempSensorPin);
  
  // Conversion from voltage level to temperature (C)
  temperature_c = -133 + 25.3 * log(temperatureVal);

  Serial.print("(");
  Serial.print(millis());
  Serial.print(")");

  Serial.print("Temperature (C) = ");
  Serial.println(temperature_c);
}

void adjustHeating() {
  // Note that maxTemp =/= minTemp to prevent the heating pad 
  // from switching on and off quickly when the temp is near the threshold
  if (temperature_c > maxTemp) {
    digitalWrite(heatingPin, LOW);
  }
  if (temperature_c < minTemp) {
    digitalWrite(heatingPin, HIGH);
  }
}

void buzzer(){
  // "cycle" controls when the buzzer is on/off, namely every 10th time this function is called
  if (cycle == 10) {
    cycle = 0;
    tone(buzzerPin, 3000); // "optimal" (i.e. most annoying) frequency as determined by testing
  }
  else if (cycle==0){
    noTone(buzzerPin);
    cycle++;
  }
  else{
    cycle++;
  }
}
