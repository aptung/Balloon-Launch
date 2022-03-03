// TO DO: Strobe (?)
// Integrate GPS chip, change Serial commands to Serial1, integrate Michelle's indvidual experiment, Grace's individual experiment



#include <SparkFun_MS5803_I2C.h> // Click here to get the library: http://librarymanager/All#SparkFun_MS5803-14BA
#include <MQ131.h>
#include <math.h>
#include <Wire.h> //Needed for I2C to GNSS

#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS
SFE_UBLOX_GNSS myGNSS;

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
unsigned long timeGPS;

bool cutdown = false;
int minPressure = 14; // From Michael (?)
int minTemp = -10; // in C
int maxTemp = 10;

float R0; // "base resistance for circuit" (from Emory)

long latitude, longitude, altitude;
byte SIV;

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
  Serial1.begin(9600);
  //Retrieve calibration constants for conversion math.
  sensor.reset();
  sensor.begin();

  Wire.begin();

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial1.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR
  
  pressure_baseline = sensor.getPressure(ADC_4096);

  // Initiate sensor
  // Heater control on pin 2
  // Sensor analog read on pin A0
  // Model low concentration
  // Load resistance RL of 1MOhms (1000000 Ohms)
  Serial1.println("Calibrating...");
  MQ131.begin(2, A0, LOW_CONCENTRATION, 1000000); // From Emory
  MQ131.calibrate();
  Serial1.println("Calibration done!");

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
  timeOzone = millis();
  timeGPS = millis();
}

void loop() {
  if (millis()-timePressure>1000){
    Serial1.print("Time=");
    Serial1.println(millis());
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

  if (millis()-timeGPS>1000){
    timeGPS = millis();
    updateGPS();
    printGPS();
  }

  if (millis()-timeHall>100){
    timeHall = millis();
    readHallChip();
    // printPressure();
    printHallChip();
  }

  if (millis()-timeOzone>60000){
    timeOzone = millis();
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
//  Serial1.print("Current relative time (millis) = ");
//  Serial1.println(time);
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
//  Serial1.print("Pressure raw (mbar)= ");
//  Serial1.println(pressure_abs);

  Serial1.print("Pressure corrected (mbar)= ");
  Serial1.println(pressure_corrected);

//  Serial1.print("Pressure relative (mbar)= ");
//  Serial1.println(pressure_relative);
//
//  Serial1.print("Altitude change (m) = ");
//  Serial1.println(altitude_delta);
}

void updateTemperature(){
  temperatureVal = analogRead(tempSensorPin);
//  Serial1.print("Temperature voltage= ");
//  Serial1.println(temperatureVal);
  

  // t actual = -133 + 25.3 ln val
  temperature_c = -133 + 25.3 * log(temperatureVal);
}

void printTemperature(){
  Serial1.print("Temperature (C) = ");
  Serial1.println(temperature_c);
}

void updateGPS(){
  latitude = myGNSS.getLatitude();
  longitude = myGNSS.getLongitude();
  altitude = myGNSS.getAltitude();
  SIV = myGNSS.getSIV();
}

void printGPS(){
  Serial1.print(F("Lat: "));
  Serial1.print(latitude);
  
  Serial1.print(F(" Long: "));
  Serial1.print(longitude);
  
  Serial1.print(F(" (degrees * 10^-7)"));
  Serial1.print(F(" Alt: "));
  Serial1.print(altitude);
  Serial1.print(F(" (mm)"));

  Serial1.print(F(" SIV: "));
  Serial1.print(SIV);

  Serial1.println();
}

void readHallChip(){
  hallChipVal = analogRead(hallChipPin);
}

void printHallChip(){
  Serial1.print("Hall Chip on/off: ");
//Serial1.println(hallChip);
  if (hallChipVal>1000){
    Serial1.println("ON");
  }
  else{
    Serial1.println("OFF");
  }
}

void readOzone(){
  MQ131.sample();
  Serial1.print("Concentration O3 ppm: ");
  Serial1.println(MQ131.getO3(PPM)); // concentration in ppm
//  Serial1.print(" , ");
//  Serial1.print("Concentration O3 ppb: ");
//  Serial1.print(MQ131.getO3(PPB)); // concentration in ppb
//  Serial1.print(" , ");
//  Serial1.print("Concentration O3 mg/m3: ");
//  Serial1.print(MQ131.getO3(MG_M3)); // concentration in mg/m3
//  Serial1.print(" , ");
//  Serial1.print("Concentration O3 ug/m3: ");
//  Serial1.println(MQ131.getO3(UG_M3)); // concentration in ug/m3
}

void adjustHeatingPad() {
  Serial1.print("Heating pad on/off: ");
  if (temperature_c > maxTemp){
    digitalWrite(heatingPin, LOW);
    Serial1.println("OFF");
  }
  if (temperature_c < minTemp){
    digitalWrite(heatingPin, HIGH);
    Serial1.println("ON");
  }
}

void buzzerOn() { tone(buzzerPin, 3000); } // optimal frequency as determined by testing that made grace (and everyone else) want to die

void buzzerOff() { noTone(buzzerPin); }

void checkCutdown() {
  if (pressure_corrected < minPressure) {
    if (timePressure > 5400000){ // Extra check to make sure it doesn't cut down before 90 min
      digitalWrite(cutdownPin, HIGH);
      Serial1.print("omg cutdown");
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


double altitudeCalculator(double P, double P0)
// Given a pressure measurement P (mbar) and the pressure at a baseline P0 (mbar),
// return altitude (meters) above baseline.
{
  return (44330.0 * (1 - pow(P / P0, 1 / 5.255)));
}
