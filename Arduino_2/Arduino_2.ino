#include <SparkFun_MS5803_I2C.h> // Click here to get the library: http://librarymanager/All#SparkFun_MS5803-14BA
#include <MQ131.h>
#include <math.h>
#include <Wire.h>


//************************ Michelle Variables Start ****************************//
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"
// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#define DHTPIN 4     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND and leave the pin 3 EMPTY (if your sensor has 4 pins)
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);
//************************ Michelle Variables End ****************************//

//************************ Raul CO2 Variables Start ****************************//
/************************Hardware Related Macros************************************/
#define         MG_PIN                       (A0)     //define which analog input channel you are going to use
#define         BOOL_PIN                     (2)
#define         DC_GAIN                      (8.5)   //define the DC gain of amplifier

/***********************Software Related Macros************************************/
#define         READ_SAMPLE_INTERVAL         (10)    //define how many samples you are going to take in normal operation
#define         READ_SAMPLE_TIMES            (100)     //define the time interval(in milisecond) between each samples in
                                                     //normal operation

/**********************Application Related Macros**********************************/
//These two values differ from sensor to sensor. user should derermine this value.
#define         ZERO_POINT_VOLTAGE           (0.38) //define the output of the sensor in volts when the concentration of CO2 is 400PPM 0.38
#define         REACTION_VOLTAGE             (0.030) //define the voltage drop of the sensor when move the sensor from air into 1000ppm CO2

/*****************************Globals***********************************************/
float           CO2Curve[3]  =  {2.602,ZERO_POINT_VOLTAGE,(REACTION_VOLTAGE/(2.602-3))};
                                                     //two points are taken from the curve.
                                                     //with these two points, a line is formed which is
                                                     //"approximately equivalent" to the original curve.
                                                     //data format:{ x, y, slope}; point1: (lg400, 0.324), point2: (lg4000, 0.280)
                                                     //slope = ( reaction voltage ) / (log400 –log1000)
//************************ Raul Variables End ****************************//

//************************ Pressure Sensor Variables Start ****************************//
// For pressure sensor
// Begin class with selected address
// available addresses (selected by jumper on board)
// default is ADDRESS_HIGH

//  ADDRESS_HIGH = 0x76
//  ADDRESS_LOW  = 0x77

MS5803 sensor(ADDRESS_HIGH);

//Create variables to store results
double pressure_abs, pressure_relative, pressure_corrected;

// Create Variable to store altitude in (m) for calculations;
double base_altitude = 21; // Altitude of Menlo Park in (m)
//************************ Pressure Sensor Variables End ****************************//

//************************ Other Variables Start ****************************//
//Create variables to store results
float temperature_c;

// These are unsigned to increase the range
// Each of these is updated at different times for each component
unsigned long timePressure;
unsigned long timeGeiger;
unsigned long timeHumidity;
unsigned long timeSolar;
unsigned long timeCO2;

// In ms
#define PRESS_DEL 5000
#define SOL_DEL 5000
#define HUM_DEL 30000
#define GEI_DEL 30000
#define CO2_DEL 10000
//************************ Other Variables End ****************************//

//************************ Michael Variables Start ****************************//
///　Digital I/O PIN Settings　///
//VCC 5V (Red)
//GND (Blue)
////////////////////////////////
#define SIGN_PIN 2
#define NOISE_PIN 5
#define alpha 53.032// cpm = uSv x alpha

int index = 0; //Number of loops
char msg[256] = ""; //Message buffer for serial output

int signCount = 0; //Counter for Radiation Pulse
int noiseCount = 0; //Counter for Noise Pulse

int sON = 0; //Lock flag for Radiation Pulse
int nON = 0; //Lock flag for Noise Puls

double cpm = 0; //Count rate [cpm] of current
double cpmHistory[200]; //History of count rates
int cpmIndex = 0; //Position of current count rate on cpmHistory[]
int cpmIndexPrev = 0; //Flag to prevent duplicative counting

//Timing Settings for Loop Interval
int prevTime = 0;
int currTime = 0;

int totalSec = 0; //Elapsed time of measurement [sec]
int totalHour = 0; //Elapsed time of measurement [hour]

//Time settings for CPM calcuaration
int cpmTimeMSec = 0;
int cpmTimeSec = 0;
int cpmTimeMin = 0;

//String buffers of float values for serial output
char cpmBuff[20];
char uSvBuff[20];
char uSvdBuff[20];
//************************ Michael Variables End ****************************//

//************************ Grace Variables Start ****************************//
const int SOLAR = A3; // solar panel
const int FILTER = A2; // solar panel with UV filter
unsigned long timeCurrent;
int del = 1000;
//************************ Grace Variables End ****************************//

void setup() {

  
  //Initialize Serial Monitor
  Serial.begin(9600);

  //***********************************//
  
  // Michelle Humidity Setup Start 
  dht.begin();
  // Michelle Setup End 
  
  //***********************************//
  
  //Retrieve calibration constants for conversion math.
  sensor.reset();
  sensor.begin(); // For pressure sensor

  Wire.begin(); // for I2C communication

  //***********************************//
  
  // Grace Solar UV Setup Start 
  pinMode(SOLAR, INPUT); 
  pinMode(FILTER, INPUT); 
  timeCurrent = millis(); 
  // Grace Setup End
  
  //***********************************//
  
  // Michael Geiger Setup Start 
  //PIN setting for Radiation Pulse
  pinMode(SIGN_PIN, INPUT);
  digitalWrite(SIGN_PIN, HIGH);

  //PIN setting for Noise Pulse
  pinMode(NOISE_PIN, INPUT);
  digitalWrite(NOISE_PIN, HIGH);

  //CSV-formatting for serial output (substitute , for _)
  Serial.println("hour[h]_sec[s]_count_cpm_uSv/h_uSv/hError");

  //Initialize cpmHistory[]
  for (int i = 0; i < 200; i++ )
  {
    cpmHistory[i] = 0;
  }
  // Michael Setup End 

  //***********************************//

  // Raul CO2 Setup Start
  pinMode(BOOL_PIN, INPUT);                        //set pin to input
  digitalWrite(BOOL_PIN, HIGH);                    //turn on pullup resistors
  // Raul End

  //***********************************//
  
  // Time Setup
  unsigned long roundedTime = millis()-(millis()%1000)+1000; // So that they start at an integer number of seconds
  timePressure = roundedTime;
  timeSolar = roundedTime;
  timeHumidity = roundedTime;
  timeGeiger = roundedTime;
  timeCO2 = roundedTime;
}

void loop() {
  
  if (millis()-timePressure>PRESS_DEL){
    timePressure = millis();
    getPressure();
  }

  if (millis()-timeSolar > SOL_DEL) {
    timeSolar = millis();
    getSolar();
  }
  
  if (millis()-timeHumidity> HUM_DEL){
    timeHumidity = millis();
    getHumidity();
  }

  if (millis()-timeCO2> CO2_DEL){
    timeCO2 = millis();
    getCO2();
  }

//  if (millis()-timeGeiger> GEI_DEL){
//    timeGeiger = millis();
//    getGeiger();
//  }
  
  
}
