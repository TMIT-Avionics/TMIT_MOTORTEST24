#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
#include <SD.h>

// GPIO definitions
#define RX_RYLR       2
#define TX_RYLR       3
#define RX_RS232      8
#define TX_RS232      9 
#define D4184A        4 // E-MATCH
#define D4184B        5 // IGNITER WIRE
const String RYLR_ADD = "56"; //set to groundstation RYLR address

// State Machine Definition
typedef enum {SAFE,ARMED,LAUNCHED} STATE;
STATE currentState = SAFE;

// Object Instantiations
SoftwareSerial RYLR(RX_RYLR, TX_RYLR);
AltSoftSerial RS232;

// Variable Definitions
File logFile;
String response, weight, message, packet;
float read, tareValue = 0;

// Function Definitions
String parseRYLR(String input) 
{
  int start = input.indexOf(',') + 1;
  start = input.indexOf(',', start) + 1;
  int end = input.indexOf(',', start);
  return input.substring(start, end);  
}

void getTareValue() 
{
  if(RS232.available())
  {
    tareValue = RS232.parseFloat();
    Serial.println("TARE VALUE UPDATED");
  }
}

void logData()
{
  // write SD Card code
}

void transmitData()
{
  if(RS232.available())
  {
    read = RS232.parseFloat();
    weight = String(read - tareValue);
    packet = String("AT+SEND=")+RYLR_ADD+","+String(weight.length())+","+weight;
    RYLR.println(packet);
  }
}

void checkInput(String receive)
{
  if (receive == "SAFE" && currentState == SAFE)
  {
    Serial.println("COMMUNICATION ESTABLISHED.");
    message = "SAFE";
    packet = String("AT+SEND=0,")+String(message.length())+","+message;
    RYLR.println(packet);
  }
  else if (receive == "ARMON" && currentState == SAFE)
  {
    Serial.println("CURRENT STATE: ARMED");
    currentState = ARMED;
    message = "ARMED";
    packet = String("AT+SEND=0,")+String(message.length())+","+message;
    RYLR.println(packet);
    getTareValue(); // TARE once ARMED state is reached
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
  switch(currentState)
  {
    case SAFE:
      break;
    case ARMED:
      transmitData();
      logData();
      break;
    case LAUNCHED:
      digitalWrite(D4184A, HIGH);
      digitalWrite(D4184B, HIGH);
      Serial.println("D4184s LATCHED");
      delay(2000);
      digitalWrite(D4184A, LOW);
      Serial.println("E-MATCH D4184 UNLATCHED");
      delay(1500);
      digitalWrite(D4184B, LOW);
      Serial.println("IGNITER WIRE D4184 UNLATCHED");
      
      while(true)
      {
        transmitData();
        logData();
      }
      break;
  }
}

void setup() 
{
  pinMode(D4184A, OUTPUT);
  pinMode(D4184B, OUTPUT);
  digitalWrite(D4184A, LOW);
  digitalWrite(D4184B, LOW);

  Serial.begin(9600);
  RYLR.begin(57600);
  RS232.begin(9600);
  getTareValue();
}

void loop() 
{
  if (RYLR.available()) 
  {
    response = RYLR.readStringUntil('\n');
    RYLR.flush();
    Serial.println("MESSAGE FROM GROUND: " + response);
    response = parseRYLR(response);
    
    checkInput(response);
    performOperation(currentState);
  }
}