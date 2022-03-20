const int solar = A2; // solar panel
const int filter = A3; // solar panel with UV filter
unsigned long timeCurrent;
int del = 1000;

void setup(){
  pinMode(solar, INPUT); 
  pinMode(filter, INPUT); 
  timeCurrent = millis();
  Serial.begin(9600);

}

void loop(){
  if (millis()-timeCurrent>del) { // Repeat the thing inside every del millis
    timeCurrent = millis(); // reset the time
    Serial.print("Time = ");
    Serial.println(timeCurrent);
    
    int val = analogRead(solar);
    Serial.print(val);
    double volt = val / 1023.0 * 5; // scale based on arduino 5V
    Serial.print(" (solar) : ");
    Serial.println(volt);

    int fval = analogRead(filter);
    Serial.print(fval);
    double fvolt = fval / 1023.0 * 5;
    Serial.print(" (filter) : ");
    Serial.println(fvolt);
  }
    
    
  
}
