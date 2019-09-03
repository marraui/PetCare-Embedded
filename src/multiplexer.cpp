// /*
//  ESP8266 4051 Multiplexer by Brian Lough
 
//  An example showing how to use a 4051 multiplexer with an esp8266
//  to connect up to 8 analog sensors.
//  Wiring:
//  Wemos ->  4051
//  ---------------
//  D2    ->   S0 (A)
//  G    ->   S1 (B)
//  G    ->   S2 (C)
//  A0    ->   Common
//  3.3v  ->   VCC
//  G     ->   GND
//  G     ->   Inhibit
//  G     ->   VEE  
 
//  4051 Option pins are then wired to whatever Analog sensors required
//  One thing to note: say for example if you only require 2 analog sensors,
//  You can just wire up S0(A) and connect S1 & S2 to GND and you will be 
//  able to switch between option 1 and option 2 pins.
//  Same goes for up to 4 pins (just use S0 & S1)
// */
// #include <ESP8266WiFi.h>

// #define MUX_A D2 

// #define ANALOG_INPUT A0


// void setup() {
//   //Deifne output pins for Mux
//   Serial.begin(9600);
//   pinMode(MUX_A, OUTPUT);   
// }

// void changeMux(int c, int b, int a) {
//   digitalWrite(MUX_A, a);
// }

// void loop() {
//   float value;
  
//   changeMux(LOW, LOW, LOW);
//   value = analogRead(ANALOG_INPUT); //Value of the sensor connected Option 0 pin of Mux
//   Serial.print("Value 0: ");
//   Serial.println(value);
  
//   changeMux(LOW, LOW, HIGH);
//   value = analogRead(ANALOG_INPUT); //Value of the sensor connected Option 1 pin of Mux
//   Serial.print("Value 1: ");
//   Serial.println((value * 3.3 / 1024.0 - 0.5) * 100);
  
// }
