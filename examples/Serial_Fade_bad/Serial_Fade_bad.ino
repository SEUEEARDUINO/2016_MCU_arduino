/*
  Fade

  This example shows the wrong method to read commands from
  serial port. The Serial.read() will block the execution.

*/

int led = 9;           // the pin that the LED is attached to
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

// the setup routine runs once when you press reset:
void setup() {
  // declare pin 9 to be an output:
  Serial.begin(9600);
  pinMode(led, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {

  while (Serial.available()) {
    //if you only send a '9'or '5' to the arduino, the code will not work as your expect.
    if (Serial.read()  == '0') {
      brightness = 0;
    }
    if (Serial.read()  == '5') { //if you only send one char to the arduino, this line will block.
      brightness = 125;
    }
    if (Serial.read() == '9') {
      brightness = 255;
    }
  }
  // set the brightness of pin led:
  analogWrite(led, brightness);
  // wait for 30 milliseconds to see the dimming effect
  delay(30);
}
