#define TINY_GSM_MODEM_A7

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

#define HOST_NAME "esp8266-01"

#include <FS.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#define USE_ARDUINO_INTERRUPTS false // Don't use interrupts for PulseSensor
#include <PulseSensorPlayground.h>
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(D5, D6); // RX, TX
 
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <RemoteDebug.h> //https://github.com/JoaoLopesF/RemoteDebug

RemoteDebug Debug;

boolean mqttConnect();
void mqttCallback(char* topic, uint8_t* payload, unsigned int len);
boolean gsmConnect();
boolean wifiConnect(boolean loadSuccessful);
boolean loadConfig();
boolean saveConfig();
void saveConfigCallback();
void changeMux(int A);
boolean sendMessage(float bpm, float tmp);
void setup_mDNS();
void setup_remoteDebug();
void setup_arduinoOTA();

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// Range to attempt to autobaud
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 38400

// Add a reception delay - may be needed for a fast processor at a slow baud rate
//#define TINY_GSM_YIELD() { delay(2); }

#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN ""

#define JSON_DOCUMENT_SIZE 1024

#define AP_NAME "PetCare - AP"

#define MUX_A D2
#define ANALOG_INPUT A0

// Pulse sensor constants
#define THRESHOLD 550
byte samplesUntilReport;
const byte SAMPLES_PER_SERIAL_SAMPLE = 10;

// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[]  = "internet.comcel.com.co";
const char gprsUser[] = "web comcel";
const char gprsPass[] = "comcel";
const char wifiSSID[]  = "SEMARD";
const char wifiPass[] = "SEMARD123";

// MQTT details
const char* broker = "test.mosquitto.org";

const char* topicLed = "GsmClientTest/led";
const char* topicInit = "GsmClientTest/init";
const char* topicLedStatus = "GsmClientTest/ledStatus";

// Config parameters
// Minutes
int hibernationLength = 1;
char petName[40] = "Doggo";
char email[40] = "email@example.com";

bool shouldSaveConfig = false;

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif
TinyGsmClient gsmClient(modem);
WiFiClient wifiClient;
PubSubClient mqtt;
PulseSensorPlayground pulseSensor;

long lastReconnectAttempt = 0;

void setup() {
  // Set console baud rate
  SerialMon.begin(9600);
  delay(10);

  pulseSensor.setSerial(SerialMon);
  pulseSensor.analogInput(ANALOG_INPUT);
  pulseSensor.setThreshold(THRESHOLD);

  pinMode(MUX_A, OUTPUT);
  
  if (!pulseSensor.begin()) {
    SerialMon.println("Pulse sensor initialization unsuccessful");
  }

  boolean loadSuccessful = loadConfig();
  if (wifiConnect(!loadSuccessful)) {
    mqtt.setClient(wifiClient);
    
  } else {
    if (gsmConnect()) {
      mqtt.setClient(gsmClient);
    }
  }
  
  SerialMon.print("IP address: ");
  SerialMon.println(WiFi.localIP());
  // MQTT Broker setup
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(mqttCallback);
}

enum messageState {
  NOT_SENT,
  SENT,
  RECEIVED
};
enum messageState currState = NOT_SENT;
void loop() {
  if (currState == RECEIVED && hibernationLength) {
    SerialMon.print("Going to sleep for ");
    SerialMon.print(hibernationLength);
    SerialMon.println(" minutes");
    ESP.deepSleep(hibernationLength * 1000000 * 60);
  }
  else if (WiFi.status() != WL_CONNECTED) wifiConnect(false);

  if (!mqtt.connected()) {
    // Reconnect every 10 seconds
    unsigned long t = millis();
    if (t - lastReconnectAttempt > 10000L) {
      lastReconnectAttempt = t;
      if (mqttConnect()) {
        lastReconnectAttempt = 0;
      }
    }
    delay(100);
    return;
  }

  changeMux(LOW);
  if (pulseSensor.sawNewSample()) {
    /*
    Every so often, send the latest Sample.
    We don't print every sample, because our baud rate
    won't support that much I/O.
    */
    if (--samplesUntilReport == (byte) 0) {
      samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;

      if (pulseSensor.sawStartOfBeat()) {
        float bpm = pulseSensor.getBeatsPerMinute();

        SerialMon.println("♥  A HeartBeat Happened ! "); // If test is "true", print a message "a heartbeat happened".
        SerialMon.print("BPM: ");                          // Print the value inside of myBPM.
        SerialMon.println(bpm);
        changeMux(HIGH);

        float tmp = (analogRead(ANALOG_INPUT) * 3.3 / 1024.0 - 0.5) * 100;
        SerialMon.print("Temperature: ");
        SerialMon.println(tmp);

        sendMessage(bpm, tmp);
        if (currState == NOT_SENT) currState = SENT;
      }
    }
  }


  mqtt.loop();
}

void mqttCallback(char* topic, uint8_t* payload, unsigned int len) {
  SerialMon.print("Message arrived [");
  SerialMon.print(topic);
  SerialMon.print("]: ");
  SerialMon.write(payload, len);
  SerialMon.println();
  currState = RECEIVED;
  SerialMon.print("value of currstate: ");
  SerialMon.println(currState);
}

boolean gsmConnect() {
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  modem.restart();
  // modem.init();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem: ");
  SerialMon.println(modemInfo);

#if TINY_GSM_USE_GPRS
  // Unlock your SIM card with a PIN if needed
  if ( GSM_PIN && modem.getSimStatus() != 3 ) {
    SerialMon.print("Unlocking SIM... ");
    if(modem.simUnlock(GSM_PIN)) {
      SerialMon.println("success");
    } else {
      SerialMon.println("fail");
    }
  }
#endif

#if defined TINY_GSM_USE_WIFI && defined TINY_GSM_MODEM_HAS_WIFI
  SerialMon.print(F("Setting SSID/password..."));
  if (!modem.networkConnect(wifiSSID, wifiPass)) {
    SerialMon.println(" fail");
    delay(10000);
    return false;
  }
  SerialMon.println(" success");
#endif

#if TINY_GSM_USE_GPRS && defined TINY_GSM_MODEM_XBEE
  // The XBee must run the gprsConnect function BEFORE waiting for network!
  modem.gprsConnect(apn, gprsUser, gprsPass);
#endif

  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    delay(10000);
    return false;
  }
  SerialMon.println(" success");

  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected");
  }

#if TINY_GSM_USE_GPRS && defined TINY_GSM_MODEM_HAS_GPRS
    SerialMon.print(F("Connecting to "));
    SerialMon.print(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
      SerialMon.println(" fail");
      delay(10000);
      return false;
    }
    SerialMon.println(" success");
#endif
  // SerialMon.print("Enabling GPS... ");
  // if (!modem.enableGPS()) {
  //   SerialMon.println("fail");
  //   return false;
  // }
  // SerialMon.println("success");

  // SerialMon.print("Enabling AGPS... ");
  // if (!modem.enableAGPS()) {
  //   SerialMon.println("fail");
  // }

  // SerialMon.println("success");
  return true;
}

boolean wifiConnect(boolean forceConfigPortal) {
  SerialMon.print("Connecting to Wi-Fi...");
  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  WiFiManagerParameter customHibernationLength(
    "hibernationLength",
    "Hibernation length",
    String(hibernationLength).c_str(),
    5
  );
  WiFiManagerParameter customPetName("name", "Pet name", petName, 40);
  WiFiManagerParameter customEmail("email", "e-mail", email, 40);

  wifiManager.addParameter(&customHibernationLength);
  wifiManager.addParameter(&customPetName);
  wifiManager.addParameter(&customEmail);


  boolean connectionSuccessful = false;
  if (forceConfigPortal) {
    SerialMon.println("Starting forced config portal");
    connectionSuccessful = wifiManager.startConfigPortal(AP_NAME);
  }
  else connectionSuccessful = wifiManager.autoConnect(AP_NAME);

  if(!connectionSuccessful) {
    SerialMon.println(" fail");
    return false;
  }
  SerialMon.println(" success");

  SerialMon.print("raw hibernation value: ");
  SerialMon.println(customHibernationLength.getValue());
  hibernationLength = String(customHibernationLength.getValue()).toInt();
  SerialMon.println("parsed value: ");
  SerialMon.println(hibernationLength);
  strcpy(petName, customPetName.getValue());
  strcpy(email, customEmail.getValue());

  SerialMon.print("petName: ");
  SerialMon.println(petName);

  SerialMon.println("Updated config parameters");

  if (shouldSaveConfig) {
    saveConfig();
  }
  return true;
}

boolean mqttConnect() {
  SerialMon.print("Connecting to ");
  SerialMon.print(broker);

  // Connect to MQTT Broker
  boolean status = mqtt.connect("GsmClientTest");

  // Or, if you want to authenticate MQTT:
  //boolean status = mqtt.connect("GsmClientName", "mqtt_user", "mqtt_pass");

  if (status == false) {
    SerialMon.println(" fail");
    return false;
  }
  SerialMon.println(" success");
  if (mqtt.subscribe(email)) {
    SerialMon.print("Subscribed to: ");
    SerialMon.println(email);
  }
  return mqtt.connected();
}

boolean sendMessage(float bpm, float tmp) {
  DynamicJsonDocument jsonDocument(200);
  jsonDocument["name"] = petName;
  jsonDocument["typeOfPet"] = 1;
  jsonDocument["heartRate"] = bpm;
  jsonDocument["temperature"] = tmp;
  char json[200];
  serializeJson(jsonDocument, json);
  mqtt.publish(email, json);
  return mqtt.connected();
}

boolean loadConfig() {
  if (!SPIFFS.begin()) {
    SerialMon.println("Failed to mount file system: couldn't initialize SPIFFS");
    return false;
  }
  SerialMon.println("Mounted file system");

  if (!SPIFFS.exists("/config.json")) {
    SerialMon.println("Failed to mount the file system: config.json doesn't exist");
    return false;
  }

  // File exists reading and loading
  SerialMon.println("Reading config file");
  File configFile = SPIFFS.open("/config.json", "r");

  if (!configFile) {
    SerialMon.println("Failed to mount the file system: couldn't open config.json");
    return false;
  }

  SerialMon.println("Opened config file");
  size_t size = configFile.size();
  
  // Allocate buffer to store contents of the file
  std::unique_ptr<char[]> buffer(new char[size]);

  configFile.readBytes(buffer.get(), size);
  DynamicJsonDocument jsonDocument(JSON_DOCUMENT_SIZE);
  DeserializationError error = deserializeJson(jsonDocument, buffer.get());
  JsonObject json = jsonDocument.as<JsonObject>();
  serializeJsonPretty(jsonDocument, SerialMon);

  if (error) {
    SerialMon.println("Failed to load json config");
    configFile.close();
    return false;
  }

  SerialMon.println("Parsed json");
  strcpy(petName, json["petName"]);
  strcpy(email, json["email"]);
  configFile.close();
  
  return true;
}

boolean saveConfig() {
  SerialMon.println("Saving config");
  DynamicJsonDocument jsonDocument(JSON_DOCUMENT_SIZE);
  jsonDocument["email"] = email;
  jsonDocument["petName"] = petName;
  File configFile = SPIFFS.open("/config.json", "w");

  if (!configFile) {
    SerialMon.println("Failed to open config file for writing");
    return false;
  }

  serializeJsonPretty(jsonDocument, SerialMon);
  serializeJson(jsonDocument, configFile);
  configFile.close();
  SerialMon.println("config saved");
  return true; 
}

void saveConfigCallback() {
  SerialMon.println("Should save config");
  shouldSaveConfig = true;
}

void changeMux(int A) {
  digitalWrite(MUX_A, A);
}

void setup_mDNS()
{
  if (MDNS.begin(HOST_NAME))
  {
    SerialMon.print("* MDNS responder started. Hostname -> ");
    SerialMon.println(HOST_NAME);
  }
  MDNS.addService("telnet", "tcp", 23);
}

void setup_arduinoOTA()
{
  ArduinoOTA.onStart([]() {
    SerialMon.println("OTA: Start");
  });
  ArduinoOTA.onEnd([]() {
    SerialMon.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    SerialMon.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    SerialMon.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      SerialMon.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      SerialMon.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      SerialMon.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      SerialMon.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      SerialMon.println("End Failed");
  });
  SerialMon.println("OTA about to begin");
  ArduinoOTA.begin();
}

void setup_remoteDebug()
{
    // Initialize RemoteDebug

    Debug.begin(HOST_NAME); // Initialize the WiFi server

    Debug.setResetCmdEnabled(true); // Enable the reset command

    Debug.showProfiler(true); // Profiler (Good to measure times, to optimize codes)
    Debug.showColors(true);   // Colors
}
