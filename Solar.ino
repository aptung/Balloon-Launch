const int solar = A11; // solar panel
const int filter = A7; // solar panel with UV filter

void setup(){
  pinMode(solar, INPUT); 
  pinMode(filter, INPUT); 
  Serial.begin(9600);

}

void loop(){
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
    
    delay(1000);
    
  
}
