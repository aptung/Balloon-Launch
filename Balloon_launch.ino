// TO DO: Strobe (?)

// millis() is a sort of relative time function
// https://forum.arduino.cc/t/internal-clock-function/265591/4


#include <SparkFun_MS5803_I2C.h> // Click here to get the library: http://librarymanager/All#SparkFun_MS5803-14BA
#include <MQ131.h>
#include <math.h>

// Begin class with selected address
// available addresses (selected by jumper on board)
// default is ADDRESS_HIGH

//  ADDRESS_HIGH = 0x76
//  ADDRESS_LOW  = 0x77

MS5803 sensor(ADDRESS_HIGH);

//Create variables to store results
float temperature_c;
double pressure_abs, pressure_relative, altitude_delta, pressure_baseline;
double pressure_corrected;
int temperatureVal;
int hallChipVal;

// Create Variable to store altitude in (m) for calculations;
double base_altitude = 21; // Altitude of Menlo Park in (m)

// These are unsigned to increase the range
// Each of these is updated at different times for each component
unsigned long timePressure;
unsigned long timeTemperature;
unsigned long timeHall;
unsigned long timeHeating;
unsigned long timeBuzzer;
unsigned long timeCutdown;
unsigned long timeOzone;

bool cutdown = false;
int minPressure = 14; // From Michael (?)
int minTemp = -10; // in C
int maxTemp = 10;

float R0; // "base resistance for circuit" (from Emory)

int counter = 0; // For buzzer alternation purposes

const int cutdownPin = 8; // put pin to control cutdown here
const int heatingPin = 6; // put pin to control heating sensor here
const int tempSensorPin = A3; // put pin to read in temperature sensor
const int buzzerPin = 9; // put pin to control buzzer
const int hallChipPin = A1;


void setup() {
  // Start your preferred I2C object
  // Wire.begin();
  //Initialize Serial Monitor
  Serial.begin(9600);
  //Retrieve calibration constants for conversion math.
  sensor.reset();
  sensor.begin();
  
  pressure_baseline = sensor.getPressure(ADC_4096);

  // Initiate sensor
  // Heater control on pin 2
  // Sensor analog read on pin A0
  // Model low concentration
  // Load resistance RL of 1MOhms (1000000 Ohms)
  MQ131.begin(2, A0, LOW_CONCENTRATION, 1000000); // From Emory
  MQ131.calibrate();
  Serial.println("Calibration done!");
  Serial.print("R0 = ");
  Serial.print(MQ131.getR0());
  Serial.println("Ohms");
  Serial.print("Time to heat = ");
  Serial.print(MQ131.getTimeToRead()); // time until heated
  Serial.println("s");
  Serial.println("Sampling...");

  pinMode(cutdownPin, OUTPUT);
  pinMode(heatingPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(tempSensorPin, INPUT);
  pinMode(hallChipPin, INPUT);

  timePressure = millis();
  timeTemperature = millis();
  timeHall = millis();
  timeHeating = millis();
  timeBuzzer = millis();
  timeCutdown = millis();
}

void loop() {
  if (millis()-timePressure>1000){
    timePressure = millis();
    updatePressure();
    printPressure();
  }
  if (millis()-timeTemperature>1000){
    timeTemperature = millis();
    updateTemperature();
    printTemperature();
    // temperature_c = -20;
  }

  if (millis()-timeHall>250){
    timeHall = millis();
    readHallChip();
    // printPressure();
    printHallChip();
  }

  if (millis()-timeOzone>10000){
    readOzone();
  }

  if (cutdown) {
    if (millis()-timeBuzzer>1000){
      timeBuzzer = millis();
      if (counter==0){
        counter=1;
        buzzerOn();
      }
     else if (counter==1){
        counter=0;
        buzzerOff();
     }
   }
  }

  if (millis()-timeCutdown>5000){
    timeCutdown = millis();
    checkCutdown();
  }

  if (millis()-timeHeating>5000){
    timeHeating = millis();
    adjustHeatingPad();
  }
  
}

//void updateTime() {
//  time = millis();
//  Serial.print("Current relative time (millis) = ");
//  Serial.println(time);
//}

void updatePressure(){

  // To measure to higher degrees of precision use the following sensor settings:
  // ADC_256
  // ADC_512
  // ADC_1024
  // ADC_2048
  // ADC_4096
  
  // Pressure is in mbar.
  pressure_abs = sensor.getPressure(ADC_4096);
  pressure_corrected = (pressure_abs+9.96392)/1.00249; // Correction based on calibration testing
  
//  // Convert abs pressure with the help of altitude into relative pressure
//  // This is used in Weather stations.
//  pressure_relative = sealevel(pressure_abs, base_altitude);
//
//  // Taking our baseline pressure at the beginning we can find an approximate
//  // change in altitude based on the differences in pressure.
//  altitude_delta = altitude(pressure_abs , pressure_baseline);
}

void printPressure(){
//  Serial.print("Pressure raw (mbar)= ");
//  Serial.println(pressure_abs);

  Serial.print("Pressure corrected (mbar)= ");
  Serial.println(pressure_corrected);

//  Serial.print("Pressure relative (mbar)= ");
//  Serial.println(pressure_relative);
//
//  Serial.print("Altitude change (m) = ");
//  Serial.println(altitude_delta);
}

void updateTemperature(){
  temperatureVal = analogRead(tempSensorPin);
//  Serial.print("Temperature voltage= ");
//  Serial.println(temperatureVal);
  

  // t actual = -133 + 25.3 ln val
  temperature_c = -133 + 25.3 * log(temperatureVal);
}

void printTemperature(){
  Serial.print("Temperature (C) = ");
  Serial.println(temperature_c);
}

void readHallChip(){
  hallChipVal = analogRead(hallChipPin);
}

void printHallChip(){
  Serial.print("Hall Chip on/off: ");
//Serial.println(hallChip);
  if (hallChipVal>1000){
    Serial.println("ON");
  }
  else{
    Serial.println("OFF");
  }
}

void readOzone(){
  MQ131.sample();
  Serial.print("Concentration O3 ppm: ");
  Serial.print(MQ131.getO3(PPM)); // concentration in ppm
  Serial.print(" , ");
  Serial.print("Concentration O3 ppb: ");
  Serial.print(MQ131.getO3(PPB)); // concentration in ppb
  Serial.print(" , ");
  Serial.print("Concentration O3 mg/m3: ");
  Serial.print(MQ131.getO3(MG_M3)); // concentration in mg/m3
  Serial.print(" , ");
  Serial.print("Concentration O3 ug/m3: ");
  Serial.println(MQ131.getO3(UG_M3)); // concentration in ug/m3
}

void adjustHeatingPad() {
  Serial.print("Heating pad on/off: ");
  if (temperature_c > maxTemp){
    digitalWrite(heatingPin, LOW);
    Serial.println("OFF");
  }
  if (temperature_c < minTemp){
    digitalWrite(heatingPin, HIGH);
    Serial.println("ON");
  }
}

void buzzerOn() { tone(buzzerPin, 3000); } // optimal frequency as determined by testing that made grace (and everyone else) want to die

void buzzerOff() { noTone(buzzerPin); }

void checkCutdown() {
  if (pressure_corrected < minPressure) {
    if (timePressure > 5400000){ // Extra check to make sure it doesn't cut down before 90 min
      digitalWrite(cutdownPin, HIGH);
      Serial.print("omg cutdown");
      cutdown = true;
    }
  }
}



// Thanks to Mike Grusin for letting me borrow the functions below from
// the BMP180 example code.

double sealevel(double P, double A)
// Given a pressure P (mbar) taken at a specific altitude (meters),
// return the equivalent pressure (mbar) at sea level.
// This produces pressure readings that can be used for weather measurements.
{
  return (P / pow(1 - (A / 44330.0), 5.255));
}


double altitude(double P, double P0)
// Given a pressure measurement P (mbar) and the pressure at a baseline P0 (mbar),
// return altitude (meters) above baseline.
{
  return (44330.0 * (1 - pow(P / P0, 1 / 5.255)));
}
