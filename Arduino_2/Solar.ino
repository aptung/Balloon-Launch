void getSolar() {
  int val = analogRead(SOLAR);
  // Serial.print(val);
  double volt = val / 1023.0 * 5; // scale based on arduino 5V
  Serial.print("(");
  Serial.print(millis());
  Serial.print(")");
  Serial.print("Without filter: ");
  Serial.println(volt);

  
  int fval = analogRead(FILTER);
  // Serial.print(fval);
  double fvolt = fval / 1023.0 * 5;
  Serial.print("(");
  Serial.print(millis());
  Serial.print(")");
  Serial.print("With filter: ");
  Serial.println(fvolt);
}
