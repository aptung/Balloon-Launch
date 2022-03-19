// Emory Ozone Sensor Code
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

/*
MQ-131 Calibration & Sampling
*/

#include <MQ131.h>

float R0; // base resistance for circuit

void setup() {
Serial.begin(9600);

// Initiate sensor
// Heater control on pin 2
// Sensor analog read on pin A0
// Model low concentration
// Load resistance RL of 1MOhms (1000000 Ohms)

MQ131.begin(2, A0, LOW_CONCENTRATION, 1000000);  

Serial.println("Calibration in progress...");
MQ131.calibrate();
 
Serial.println("Calibration done!");
Serial.print("R0 = ");
Serial.print(MQ131.getR0());
Serial.println("Ohms");
Serial.print("Time to heat = ");
Serial.print(MQ131.getTimeToRead()); // time until heated
Serial.println("s");
Serial.println("Sampling...");
}

// calibration complete
// beginning sampling

void loop() {

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

delay(60000);
}
