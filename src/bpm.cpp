// #include<ESP8266WiFi.h>
// #define USE_ARDUINO_INTERRUPTS false
// #include <PulseSensorPlayground.h>


// const int PULSE_INPUT = A0;
// const int THRESHOLD = 550;   // Adjust this number to avoid noise when idle


// byte samplesUntilReport;
// const byte SAMPLES_PER_SERIAL_SAMPLE = 10;

// /*
//    All the PulseSensor Playground functions.
// */
// PulseSensorPlayground pulseSensor;

// void setup() {
//   /*
//      Use 115200 baud because that's what the Processing Sketch expects to read,
//      and because that speed provides about 11 bytes per millisecond.

//      If we used a slower baud rate, we'd likely write bytes faster than
//      they can be transmitted, which would mess up the timing
//      of readSensor() calls, which would make the pulse measurement
//      not work properly.
//   */
//   Serial.begin(9600);

//   pulseSensor.setSerial(Serial);
//   pulseSensor.setThreshold(THRESHOLD);

//   // Skip the first SAMPLES_PER_SERIAL_SAMPLE in the loop().
//   samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;

//   // Now that everything is ready, start reading the PulseSensor signal.
//   if (!pulseSensor.begin()) {
//     /*
//        PulseSensor initialization failed,
//        likely because our Arduino platform interrupts
//        aren't supported yet.

//        If your Sketch hangs here, try changing USE_PS_INTERRUPT to false.
//     */
//     Serial.println("unsuccessful");
//   }
//   Serial.println("successful");
// }

// void loop() {

//   /*
//      See if a sample is ready from the PulseSensor.

//      If USE_INTERRUPTS is true, the PulseSensor Playground
//      will automatically read and process samples from
//      the PulseSensor.

//      If USE_INTERRUPTS is false, this call to sawNewSample()
//      will, if enough time has passed, read and process a
//      sample (analog voltage) from the PulseSensor.
//   */
//   if (pulseSensor.sawNewSample()) {
//     /*
//        Every so often, send the latest Sample.
//        We don't print every sample, because our baud rate
//        won't support that much I/O.
//     */
//     if (--samplesUntilReport == (byte) 0) {
//       samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;

//       //pulseSensor.outputSample();

//       /*
//          At about the beginning of every heartbeat,
//          report the heart rate and inter-beat-interval.
//       */
//       if (pulseSensor.sawStartOfBeat()) {
//         Serial.println("♥  A HeartBeat Happened ! "); // If test is "true", print a message "a heartbeat happened".
//         Serial.print("BPM: ");                        // Print phrase "BPM: " 
//         Serial.println(pulseSensor.getBeatsPerMinute());                        // Print the value inside of myBPM. 
//       }
//     }

//     /*******
//       Here is a good place to add code that could take up
//       to a millisecond or so to run.
//     *******/
//   }

//   /******
//      Don't add code here, because it could slow the sampling
//      from the PulseSensor.
//   ******/
// }
