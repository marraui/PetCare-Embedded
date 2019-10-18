
// #define TINY_GSM_MODEM_A6
// // #define DUMP_AT_COMMANDS

// #include <TinyGsmClient.h>
// const char apn[]  = "internet.comcel.com.co";
// const char user[] = "web comcel";
// const char pass[] = "comcel";


// #include <SoftwareSerial.h>
// SoftwareSerial SerialAT(D5, D6); // RX, TX

// #ifdef DUMP_AT_COMMANDS
//   #include <StreamDebugger.h>
//   StreamDebugger debugger(SerialAT, Serial);
//   TinyGsm modem(debugger);
// #else
// TinyGsm modem(SerialAT);
// #endif
// TinyGsmClient client(modem);

// long lastReconnectAttempt = 0;

// #include <vector>
// #include <string>

// std::vector<std::string> splitString(const std::string &str, const std::string &delim) {
//   std::vector<std::string> tokens;
//   size_t prev = 0, pos = 0;
//   do
//   {
//     pos = str.find(delim, prev);
//     if (pos == std::string::npos)
//       pos = str.length();
//     std::string token = str.substr(prev, pos - prev);
//     tokens.push_back(token);
//     prev = pos + delim.length();
//   } while (pos < str.length() && prev < str.length());
//   return tokens;
// }

// String getCoordinates(String rawInput) {
//   std::string rawInputStdString(rawInput.c_str());
//   if (rawInputStdString == "") {
//     return "Not available";
//   }
//   std::vector<std::string> stringSplit = splitString(rawInputStdString, ",");
//   if (stringSplit[0] != "$GPGGA" || stringSplit[2] == "" || stringSplit[3] == "" || stringSplit[4] == "" || stringSplit[5] == ""){
//     return "Not available";
//   }

//   std::string result = "";
//   result += stringSplit[3] == "N" ? stringSplit[2] : "-" + stringSplit[2];

//   result += ", ";

//   result += stringSplit[5] == "E" ? stringSplit[4] : "-" + stringSplit[4];
//   String r = "";
//   r = String(result.c_str());
//   return r;
// }

// void setup() {
//   // pinMode(D0, OUTPUT);
//   // digitalWrite(D0,HIGH);

//   Serial.begin(9600);
//   delay(10);
//   SerialAT.begin(9600);
//   delay(3000);
//   Serial.println("Initializing modem...");
//   modem.restart();

//   Serial.print("Waiting for network...");
//   if (!modem.waitForNetwork()) {
//     Serial.println(" fail");
//     while (true);
//   }
//   Serial.println(" OK");

//   Serial.print("Connecting to ");
//   Serial.print(apn);
//   if (!modem.gprsConnect(apn, user, pass)) {
//     Serial.println(" fail");
//     while (true);
//   }
//   Serial.println(" OK");

//   Serial.print("Enabling GPS... ");
//   if (!modem.enableGPS()) {
//     Serial.println("fail");
//     while (true);
//   }
//   Serial.println("success");

//   // Serial.print("Enabling AGPS... ");
//   // if (!modem.enableAGPS()) {
//   //   Serial.println("fail");
//   //   while (true);
//   // }

//   // Serial.println("success");

//   SerialAT.println("at+cifsr");
//   delay(3000);
//   SerialAT.println("at+cifsr");

// }
// void loop() {
//   String gpsRaw = modem.getGPSRaw();
//   Serial.println("GPS Raw:");
//   Serial.println(gpsRaw);
//   Serial.println("Coordinates");
//   Serial.println(getCoordinates(gpsRaw));
// }
