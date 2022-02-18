// TO DO: calibrate temperature (?)
// Buzzer, strobe

// millis() is a sort of relative time function
// https://forum.arduino.cc/t/internal-clock-function/265591/4


#include <SparkFun_MS5803_I2C.h> // Click here to get the library: http://librarymanager/All#SparkFun_MS5803-14BA
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


unsigned long time;

const int cutdownPin = 8; // put pin here
int minPressure = 14; // From Michael (?)

const int heatingPin = 6; // put pin here
const int tempSensor = A3; // temperature sensor
int minTemp = -10; // in C
int maxTemp = 10;

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

  pinMode(cutdownPin, OUTPUT);
  pinMode(heatingPin, OUTPUT);
  pinMode(tempSensor, INPUT);
  pinMode(hallChipPin, INPUT);

  time = millis();
}

void loop() {
  // Makes sure that it was exactly 1s since the last iteration
  if (millis()-time>=1000){
    updateTime();

    getPressure();
    getTemperature();
    // temperature_c = -20;
    
    hallChip();

    checkCutdown();
    heatingPad();

    Serial.println(" ");//padding between outputs
  }
}

void updateTime() {
  time = millis();
  Serial.print("Current relative time (millis) = ");
  Serial.println(time);
}

void getPressure(){

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

void getTemperature(){
  temperatureVal = analogRead(tempSensor);
//  Serial.print("Temperature voltage= ");
//  Serial.println(temperatureVal);
  

  // t actual = -133 + 25.3 ln val
  temperature_c = -133 + 25.3 * log(temperatureVal);
  Serial.print("Temperature (C) = ");
  Serial.println(temperature_c);
}

void hallChip(){
  hallChipVal = analogRead(hallChipPin);
  Serial.print("Hall Chip on/off: ");
//Serial.println(hallChip);
  if (hallChipVal>1000){
    Serial.println("ON");
  }
  else{
    Serial.println("OFF");
  }
}

void heatingPad() {
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

void checkCutdown() {
  if (pressure_corrected < minPressure) {
    digitalWrite(cutdownPin, HIGH);
    // Serial.print("yayay");
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
