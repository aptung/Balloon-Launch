#include <SparkFun_MS5803_I2C.h> // Click here to get the library: http://librarymanager/All#SparkFun_MS5803-14BA
#include <MQ131.h>
#include <math.h>
#include <Wire.h>


// For Michelle's humidity sensor
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"
// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#define DHTPIN 3     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND and leave the pin 3 EMPTY (if your sensor has 4 pins)
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);


// For pressure sensor
// Begin class with selected address
// available addresses (selected by jumper on board)
// default is ADDRESS_HIGH

//  ADDRESS_HIGH = 0x76
//  ADDRESS_LOW  = 0x77

MS5803 sensor(ADDRESS_HIGH);



//Create variables to store results
float temperature_c;
double pressure_abs, pressure_relative, pressure_corrected;

// Create Variable to store altitude in (m) for calculations;
double base_altitude = 21; // Altitude of Menlo Park in (m)

// These are unsigned to increase the range
// Each of these is updated at different times for each component
unsigned long timePressure;
unsigned long timeOzone;
unsigned long timeHumidity;

float R0; // "base resistance for circuit" (from Emory)

void setup() {
  
  // Start your preferred I2C object
  //Wire.begin();
  //Initialize Serial Monitor
  Serial.begin(9600);

  dht.begin(); // For Michelle's humidity sensor
  
  //Retrieve calibration constants for conversion math.
  // sensor.reset();
  sensor.begin(); // For pressure sensor

  Wire.begin();

  // For Emory's ozone
  // Initiate sensor
  // Heater control on pin 2
  // Sensor analog read on pin A0
  // Model low concentration
  // Load resistance RL of 1MOhms (1000000 Ohms)
  Serial.println("Calibrating...");
  MQ131.begin(2, A0, LOW_CONCENTRATION, 1000000); // From Emory
  MQ131.calibrate();
  Serial.println("Calibration done!");
  
  unsigned long roundedTime = millis()-(millis()%1000)+1000; // So that they start at an integer number of seconds
  timePressure = roundedTime;
  timeOzone = roundedTime;
  timeHumidity = roundedTime;
}

void loop() {
  if (millis()-timePressure>1000){
    timePressure = millis();
    getPressure();
  }
  
//  if (millis()-timeHumidity>30000){
//    // Reading temperature or humidity takes about 250 milliseconds!
//    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
//    float h = dht.readHumidity();
//    // Read temperature as Celsius (the default)
//    float t = dht.readTemperature();
//    // Read temperature as Fahrenheit (isFahrenheit = true)
//    float f = dht.readTemperature(true);
//
//    // Compute heat index in Fahrenheit (the default)
//    float hif = dht.computeHeatIndex(f, h);
//    // Compute heat index in Celsius (isFahreheit = false)
//    float hic = dht.computeHeatIndex(t, h, false);
//  
//    Serial.print(F("Humidity: "));
//    Serial.print(h);
//  }

  if (millis()-timeOzone>60000){
    timeOzone = millis();
    getOzone();
  }
  
}

void getPressure(){

  // To measure to higher degrees of precision use the following sensor settings:
  // ADC_256
  // ADC_512
  // ADC_1024
  // ADC_2048
  // ADC_4096
  
  // Pressure is in mbar.
  pressure_abs = sensor.getPressure(ADC_2048);
  pressure_corrected = (pressure_abs+9.96392)/1.00249; // Correction based on calibration testing

  Serial.print("(");
  Serial.print(millis());
  Serial.print(")");
  
//  Serial.print("Pressure raw (mbar)= ");
//  Serial.println(pressure_abs);

  Serial.print("Pressure corrected (mbar)= ");
  Serial.println(pressure_corrected);
}


void getOzone(){
  MQ131.sample();

  Serial.print("(");
  Serial.print(millis());
  Serial.print(")");
  
  Serial.print("Concentration O3 ppm: ");
  Serial.println(MQ131.getO3(PPM)); // concentration in ppm
//  Serial.print(" , ");
//  Serial.print("Concentration O3 ppb: ");
//  Serial.print(MQ131.getO3(PPB)); // concentration in ppb
//  Serial.print(" , ");
//  Serial.print("Concentration O3 mg/m3: ");
//  Serial.print(MQ131.getO3(MG_M3)); // concentration in mg/m3
//  Serial.print(" , ");
//  Serial.print("Concentration O3 ug/m3: ");
//  Serial.println(MQ131.getO3(UG_M3)); // concentration in ug/m3
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
