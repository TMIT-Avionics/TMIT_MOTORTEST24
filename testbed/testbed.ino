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
typedef enum {SAFE,ARMED,LAUNCHED} STATE;
STATE currentState = SAFE;

// Object Instantiations
SoftwareSerial RYLR(RX_RYLR, TX_RYLR);
AltSoftSerial RS232;

// Variable Definitions
File logFile;
String response;

// Function Definitions
String parseRYLR(String input) 
{
  int start = input.indexOf(',') + 1;
  start = input.indexOf(',', start) + 1;
  int end = input.indexOf(',', start);
  return input.substring(start, end);  
}

void getTareValue() {

}

void checkInput(String receive)
{
  if (receive == "SAFE" && currentState == SAFE)
  {
    Serial.println("COMMUNICATION ESTABLISHED.");
    message = "SAFE";
    packet = String("AT+SEND=0,")+String(message.length())+","+message;
  }
  else if (receive == "ARMON" && currentState == SAFE)
  {
    Serial.println("CURRENT STATE: ARMED");
    currentState = ARMED;
    message = "ARMED";
    packet = String("AT+SEND=0,")+String(message.length())+","+message;
    RYLR.println(packet);
    return;
  }
  else if (receive == "SAFE" && currentState == ARMED)
  {
    Serial.println("CURRENT STATE: SAFE");
    currentState = SAFE;
    message = "SAFE";
    packet = String("AT+SEND=0,")+String(message.length())+","+message;
    RYLR.println(packet);
    return;
  }
  else if (receive == "LAUNCH" && currentState == ARMED)
  {
    Serial.println("CURRENT STATE: LAUNCHED");
    currentState = LAUNCHED;
    message = "LAUNCHED";
    packet = String("AT+SEND=0,")+String(message.length())+","+message;
    RYLR.println(packet);
    return;
  }
  else
  {
    Serial.println("INVALID INPUT PROVIDED");
    message = "ERROR";
    packet = String("AT+SEND=0,")+String(message.length())+","+message;
    RYLR.println(packet);
    return;
  }
}

void performOperation(STATE currentState) 
{

}

void setup() 
{
  Serial.begin(9600);
  RYLR.begin(57600);
  RS232.begin(9600);
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



