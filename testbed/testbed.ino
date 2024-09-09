#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
#include <SD.h>

// GPIO definitions
#define RX_RYLR       2
#define TX_RYLR       3
#define RX_RS232      8
#define TX_RS232      9 
#define ARM_SWITCH    14
#define LAUNCH_SWITCH 15
#define D4184A        4
#define D4184B        5

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
AltSoftSerial RS232;

// Variable Definitions
File logFile;
float loadCellReading;
float tareValue = 0;
String weightString, response, currState;

String parseRYLR(String input) {
  int firstcomma = input.indexOf(',');
  int secondcomma = input.indexOf(',', firstcomma + 1);
  int dataStart = secondcomma + 1;
  int dataEnd   = input.indexOf(',', dataStart);
  String output = input.substring(dataStart, dataEnd);
  output.trim();
  return output;
}

void getTareValue() {
  if (RS232.available()) {
    tareValue = RS232.parseFloat();
  }
}

void getData() {
  loadCellReading = RS232.parseFloat();
  float weight = loadCellReading - tareValue;
  weightString = String(weight);
  Serial.println(weightString);
}

void logData() {
  logFile = SD.open("loadcell.txt", FILE_WRITE);
  logFile.println(weightString);
  logFile.close();
}

void sendData() {
  String transmit = "AT+SEND=0," + String(weightString.length()) + "," + weightString + "\r\n";
  RYLR.print(transmit);
  delay(10);
}

STATE getCurrentState() {
  return currentState;
}

void sendState(STATE current) {
  currState = String(current);
  String transmit = "AT+SEND=0," + String(currState.length()) + "," + currState + "\r\n";
  RYLR.print(transmit);
  delay(10);
}

void checkInput(String receive) {
  if (receive == "ARM" && currentState == SAFE) {
    Serial.println("CURRENT STATE: ARMED.");
    currentState = ARMED;
    sendState(getCurrentState());
    return;
  }
  else if (receive == "DISARM" && currentState == ARMED) {
    Serial.println("CURRENT STATE: SAFE. ");
    currentState = SAFE;
    sendState(getCurrentState());
    return;
  }
  else if (receive == "LAUNCH" && currentState == ARMED) {
    Serial.println("CURRENT STATE: LAUNCHED. ");
    currentState = LAUNCHED;
    sendState(getCurrentState());
    return;
  }
  else {
    //Serial.println("INVALID INPUT PROVIDED. ");
    return;
  }
}

void performOperation(STATE current) {
  switch(current) {
    case SAFE:
      break;
    case ARMED:
      break;
    case LAUNCHED:
      //Ignition
      digitalWrite(D4184A, HIGH);
      digitalWrite(D4184B, HIGH);

      //Data Acquisition and Telemetry
      while(1) {
        getData();
        logData();
        sendData();
      }

      break;
    case FAILURE:
      break;
    default:
      break;
  }
}

void setup() {
  Serial.begin(9600);

  //Load Cell setup
  RS232.begin(9600);
  Serial.println("\nLoad Cell Comm. Initialised.");

  //RYLR setup
  RYLR.begin(57600);
  Serial.println("SENDING AT TRANSMIT.");

  //SD Card setup
  Serial.println("Serial Comm. Initialised.");
  if (!SD.begin()) {
    Serial.println("SD card initialisation failed.");
    return;
  }
  logFile = SD.open("loadcell.txt", FILE_WRITE);
  if (!logFile) {
    Serial.println("Couldn't open log file");
  } 
  else {
    Serial.println("Logging to SD card...\n");
  }

  getTareValue();
}

void loop() {
  if (RYLR.available()) {
    response = RYLR.readStringUntil('\n');
    RYLR.flush();
    Serial.println("MESSAGE FROM GROUND: " + response);
    response = parseRYLR(response);
    
    checkInput(response);
    performOperation(getCurrentState());
  }
}



