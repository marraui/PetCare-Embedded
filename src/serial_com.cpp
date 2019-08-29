// #define TINY_GSM_MODEM_A7
// #include <ESP8266WiFi.h>
// #include <SoftwareSerial.h>
// #include <TinyGsmClient.h>
// SoftwareSerial mySerial(D5, D6); // RX, TX

// void setup()
// {
//   // Open serial communications to computer
//   Serial.begin(9600);
//   Serial.println("Hello World!");

//   mySerial.begin(9600); // Default for the board
//   //mySerial.println("Connect! - ");
//   //Clear out any waiting serial data
//   while (mySerial.available())
//   {
//     mySerial.read();
//   }
// }

// void loop()
// {
//   /*
//    * This loop just takes whatever comes in from the console and sends it to the board
//    */
//   if (Serial.available())
//   {
//     mySerial.write(Serial.read());
//   }
//   if (mySerial.available())
//   {
//     Serial.write(mySerial.read());
//   }
// }

