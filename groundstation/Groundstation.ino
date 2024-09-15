#include <SoftwareSerial.h>
#include <SD.h>

// GPIO definitions
#define RX_RYLR       7
#define TX_RYLR       6
#define ARM_SW        2
#define LCH_SW        3

// State Machine Definition
typedef enum {
  SAFE,
  ARMED,
  LAUNCHED,
  FAILURE
} STATE;
STATE currentState = SAFE;

// Object Instantiations
SoftwareSerial RYLR(RX_RYLR, TX_RYLR);

// Variable Definitions
File logFile;
String message, response;

String parseRYLR(String input) {
  int start = input.indexOf(',') + 1;
  start = input.indexOf(',', start) + 1;
  int end = input.indexOf(',', start);
  String parsed = input.substring(start, end);
  parsed.trim();
  return parsed;  
}

void sendState(String data) {
  Serial.println("TRANSMIT: " + data);
  message = "AT+SEND=0,"+ String(data.length()) + "," + data + "\r\n";
  RYLR.print(message);
  delay(10);
}

void checkTestbed() {
  if (RYLR.available()) {
    response = RYLR.readStringUntil('\n');
    response = parseRYLR(response);
    if (response.length() > 2) {
      Serial.println(response);
    }
  }
}

void logData() {
  //SD card store
  logFile = SD.open("loadcell.txt", FILE_WRITE);
  logFile.println(response);
  logFile.close();
}

void checkInput() {
  switch (currentState) {
    case SAFE:
      if (digitalRead(ARM_SW) == HIGH) {
        currentState = ARMED;
        sendState("ARM");
        delay(1000);
      }
      break;
    case ARMED:
      if (digitalRead(ARM_SW) == LOW) {
        currentState = SAFE;
        sendState("DISARM");
        delay(1000);
      }
      else if(digitalRead(LCH_SW) == HIGH) {
        currentState = LAUNCHED;
        delay(1000);
        sendState("LAUNCH");
      }
      break;
    case LAUNCHED:
      while(1) {
        checkTestbed();
        logData();
      }
      break;
    default:
      break;
  }

}

void setup() {
  Serial.begin(9600);

  pinMode(ARM_SW, INPUT);
  pinMode(LCH_SW, INPUT);

  //RYLR setup
  RYLR.begin(57600);

  //SD Card setup
  Serial.println("\nSerial Comm. Initialised.");
  if (!SD.begin()) {
    Serial.println("SD card initialisation failed.");
    return;
  }
  logFile = SD.open("loadcell.txt", FILE_WRITE);
  if (!logFile) {
    Serial.println("Couldn't open log file");
  } 
  else {
    Serial.println("Logging to SD card...");
  }

  Serial.println("GROUNDSTATION SET UP COMPLETE.");
}

void loop() {
  checkInput();
  checkTestbed();
}