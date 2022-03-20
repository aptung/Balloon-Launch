const int buzzer = 9; //buzzer to arduino pin 9


void setup(){
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
  Serial.begin(9600);

}

void loop(){
    tone(buzzer, 3000); // optimal frequency as determined by testing that made grace (and everyone else) want to die
    delay(1000);        // ...for 1 sec
    noTone(buzzer);     // Stop sound...
    delay(20000);        // ...for 20sec
  }
  
}
