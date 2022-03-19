void getSolar() {
  int val = analogRead(SOLAR);
  Serial.print(val);
  double volt = val / 1023.0 * 5; // scale based on arduino 5V
  Serial.print(" (solar) : ");
  Serial.println(volt);

  int fval = analogRead(FILTER);
  Serial.print(fval);
  double fvolt = fval / 1023.0 * 5;
  Serial.print(" (filter) : ");
  Serial.println(fvolt);
}
