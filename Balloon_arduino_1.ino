#include <SparkFun_MS5803_I2C.h> // Library for pressure sensor, download here: http://librarymanager/All#SparkFun_MS5803-14BA
#include <math.h> // Doing math in arduino
#include <Wire.h> // Needed for I2C to GNSS


//****************** For GPS **********************//
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> // Library for GPS, download here: http://librarymanager/All#SparkFun_u-blox_GNSS
SFE_UBLOX_GNSS myGNSS; // Initialize GNSS object


//**************** For pressure sensor ********************//
MS5803 sensor(ADDRESS_HIGH);


// Create variables to store results (these are updated periodically)
float temperature_c;
double pressure_abs, pressure_relative, pressure_corrected;
int temperatureVal; // Stores the raw voltage from the temperature sensor
int hallChipVal; // Stores the raw voltage from the Hall chip


// These are unsigned to increase the range
// Each of these is updated at different times for each component
unsigned long timePressure, timeTemperature, timeHall, timeCutdown, timeGPS;

int minPressure = 14; // Pressure that triggers cutdown

// Variables for storing GPS data
long latitude, longitude, altitude;
byte SIV;


// Pin labels
const int cutdownPin = 8;
const int tempSensorPin = A3;
const int hallChipPin = A1;


void setup() {
  
  Serial.begin(9600); // Initialize Serial Monitor
  sensor.begin(); // Start pressure sensor
  Wire.begin(); // For I2C

  //****************** For GPS **********************//
  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }
  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR  


  // Initialize pins
  pinMode(cutdownPin, OUTPUT);
  pinMode(tempSensorPin, INPUT);
  pinMode(hallChipPin, INPUT);


  // Start timers
  unsigned long roundedTime = millis()-(millis()%1000)+1000; // Used to make the timers start at an integer number of seconds
  timePressure = roundedTime;
  timeTemperature = roundedTime;
  timeHall = roundedTime;
  timeCutdown = roundedTime;
  timeGPS = roundedTime;
}

void loop() {
  if (millis()-timePressure>1000){
    timePressure = millis();
    getPressure();
  }
  
  if (millis()-timeTemperature>1000){
    timeTemperature = millis();
    getTemperature();
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
  
  // Pressure is in mbar
  pressure_abs = sensor.getPressure(ADC_2048);

  Serial.print("(");
  Serial.print(millis());
  Serial.print(")");
  
  Serial.print("Pressure raw (mbar)= ");
  Serial.println(pressure_abs);
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

void getGPS(){
  latitude = myGNSS.getLatitude();
  longitude = myGNSS.getLongitude();
  altitude = myGNSS.getAltitude();
  SIV = myGNSS.getSIV();

  Serial.print("(");
  Serial.print(millis());
  Serial.print(")");
  
  Serial.print(F("Lat: "));
  Serial.print(latitude);
  
  Serial.print(F(" Long: "));
  Serial.print(longitude);
  
  Serial.print(F(" (degrees * 10^-7)"));
  Serial.print(F(" Alt: "));
  Serial.print(altitude);
  Serial.print(F(" (mm)"));

  Serial.print(F(" SIV: "));
  Serial.print(SIV);

  Serial.println();
}

void getHallChip(){
  hallChipVal = analogRead(hallChipPin); // Read in voltage from Hall chip circuit
  Serial.print("(");
  Serial.print(millis());
  Serial.print(")");
  Serial.print("Hall Chip on/off: ");
  if (hallChipVal>1000){
    Serial.println("ON");
  }
  else{
    Serial.println("OFF");
  }
}

void checkCutdown() {
  if (pressure_corrected < minPressure) {
    if (timePressure > 5400000){ // Extra check to make sure it doesn't cut down before 90 min
      digitalWrite(cutdownPin, HIGH);
      Serial.print("omg cutdown");
    }
  }
}
