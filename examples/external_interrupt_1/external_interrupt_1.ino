/*
 Input Pullup Serial

 This example demonstrates the use of pinMode(INPUT_PULLUP). It reads a
 digital input on pin 2 and prints the results to the serial monitor.

 The circuit:
 * Momentary switch attached from pin 2 to ground
 * Built-in LED on pin 13

 Unlike pinMode(INPUT), there is no pull-down resistor necessary. An internal
 20K-ohm resistor is pulled to 5V. This configuration causes the input to
 read HIGH when the switch is open, and LOW when it is closed.

 created 14 March 2012
 by Scott Fitzgerald

 http://www.arduino.cc/en/Tutorial/InputPullupSerial

 This example code is in the public domain

 */
#define pin 2
volatile bool sensorVal=0;
volatile int pressed=0;
void setup() {
  //start serial connection
  Serial.begin(9600);
  //configure pin2 as an input and enable the internal pull-up resistor
  pinMode(2, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  digitalWrite(13,LOW);
  attachInterrupt(digitalPinToInterrupt(pin),blink,FALLING);
}
void blink(){
    
    for(int i=0;i<100;i++)
    {
      delayMicroseconds(1000);
    }
    if(digitalRead(pin)==LOW){
      pressed+=1;
      Serial.println(pressed);
      sensorVal=!sensorVal;
      digitalWrite(13,sensorVal);
    }
     

  
  }
void loop() {
  //read the pushbutton value into a variable   
  //print out the value of the pushbutton
  while(Serial.available()){
    char a=Serial.read();
    if(a=='r'){
      noInterrupts();
      Serial.println(pressed);
      interrupts();
    }
  }


}



