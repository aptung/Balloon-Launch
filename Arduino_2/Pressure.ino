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
