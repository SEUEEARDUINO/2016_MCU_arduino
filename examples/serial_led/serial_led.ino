
int led= 12;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input pin:

  while(Serial.available()){
      if (Serial.read() == 'l'){
             digitalWrite(led, LOW);
              delay(5000);
              digitalWrite(led, HIGH);


        }
        delay(10);  // delay in between reads for stability
  }


}
