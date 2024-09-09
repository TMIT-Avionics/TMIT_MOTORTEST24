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


// Object Instantiations
SoftwareSerial RYLR(RX_RYLR, TX_RYLR);
AltSoftSerial RS232;

// Variable Definitions
File logFile;
String response;

// Function Definitions
String parseRYLR(String input) {

}

void getTareValue() {

}

void checkInput(String receive) {

}

void performOperation(STATE current) {

}

void setup() {
  getTareValue();
}

void loop() {
  if (RYLR.available()) {
    response = RYLR.readStringUntil('\n');
    RYLR.flush();
    Serial.println("MESSAGE FROM GROUND: " + response);
    response = parseRYLR(response);
    
    checkInput(response);
    performOperation();
  }
}



