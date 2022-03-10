
#include <SparkFun_MS5803_I2C.h> // Click here to get the library: http://librarymanager/All#SparkFun_MS5803-14BA
#include <MQ131.h>
#include <math.h>
#include <Wire.h> //Needed for I2C to GNSS


// For GPS
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS
SFE_UBLOX_GNSS myGNSS;


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
int temperatureVal;
int hallChipVal;

// Create Variable to store altitude in (m) for calculations;
double base_altitude = 21; // Altitude of Menlo Park in (m)

// These are unsigned to increase the range
// Each of these is updated at different times for each component
unsigned long timePressure;
unsigned long timeTemperature;
unsigned long timeHall;
unsigned long timeCutdown;
unsigned long timeGPS;

bool cutdown = false;
int minPressure = 14; // From Michael (?)


long latitude, longitude, altitude;
byte SIV;


const int cutdownPin = 8; // put pin to control cutdown here
const int tempSensorPin = A3; // put pin to read in temperature sensor
const int hallChipPin = A1;


void setup() {
  
  // Start your preferred I2C object
  //Wire.begin();
  //Initialize Serial Monitor
  Serial1.begin(9600);
  Serial1.println("test");

  
  //Retrieve calibration constants for conversion math.
  // sensor.reset();
  sensor.begin(); // For pressure sensor

  Wire.begin();

  // For GPS
  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial1.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }
  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR  
  
  pinMode(cutdownPin, OUTPUT);
  pinMode(tempSensorPin, INPUT);
  pinMode(hallChipPin, INPUT);

  timePressure = millis();
  timeTemperature = millis();
  timeHall = millis();
  timeCutdown = millis();
  timeGPS = millis();
}

void loop() {
  if (millis()-timePressure>1000){
    Serial1.print("Time=");
    Serial1.println(millis());
    timePressure = millis();
    getPressure();
  }
  if (millis()-timeTemperature>1000){
    timeTemperature = millis();
    getTemperature();
    // temperature_c = -20;
  }

  if (millis()-timeGPS>1000){
    timeGPS = millis();
    getGPS();
  }

  if (millis()-timeHall>100){
    timeHall = millis();
    getHallChip();
  }

  if (millis()-timeCutdown>5000){
    timeCutdown = millis();
    checkCutdown();
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

  Serial1.print("Pressure raw (mbar)= ");
  Serial1.println(pressure_abs);

  Serial1.print("Pressure corrected (mbar)= ");
  Serial1.println(pressure_corrected);
}

void getTemperature(){
  temperatureVal = analogRead(tempSensorPin);
//  Serial1.print("Temperature voltage= ");
//  Serial1.println(temperatureVal);
  
  // t actual = -133 + 25.3 ln val
  temperature_c = -133 + 25.3 * log(temperatureVal);

  Serial1.print("Temperature (C) = ");
  Serial1.println(temperature_c);
}

void getGPS(){
  latitude = myGNSS.getLatitude();
  longitude = myGNSS.getLongitude();
  altitude = myGNSS.getAltitude();
  SIV = myGNSS.getSIV();
  
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

void getHallChip(){
  hallChipVal = analogRead(hallChipPin);
  Serial1.print("Hall Chip on/off: ");
  // Serial1.println(hallChipVal);
  if (hallChipVal>1000){
    Serial1.println("ON");
  }
  else{
    Serial1.println("OFF");
  }
}

void checkCutdown() {
  if (pressure_corrected < minPressure) {
    if (timePressure > 5400000){ // Extra check to make sure it doesn't cut down before 90 min
      digitalWrite(cutdownPin, HIGH);
      Serial1.print("omg cutdown");
      cutdown = true;
    }
  }
}
