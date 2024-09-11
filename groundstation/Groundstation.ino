#include <SoftwareSerial.h>
#include <SD.h>

#define RYLR_RXD 7
#define RYLR_TXD 6
const String RYLR_ADD = "50"; //set to testbed RYLR address

#define ARM_SWITCH 2
#define LAUNCH_SWITCH 3

SoftwareSerial RYLR(RYLR_RXD, RYLR_TXD);

File logFile;

typedef enum {SAFE, ARMED, LAUNCHED} STATUS;
STATUS CURRENT_STATE = SAFE;
String message, packet, receive;

void stateInput()
{
  switch(CURRENT_STATE)
  {
    case SAFE:
      if (digitalRead(ARM_SWITCH) == HIGH && digitalRead(LAUNCH_SWITCH) == LOW)
      {
        CURRENT_STATE = ARMED;
        delay(500); //debouncing delay
        message = "ARMON";
        packet = String("AT+SEND=")+RYLR_ADD+","+String(message.length())+","+message;
        Serial.println("CURRENT STATE = ARMED");
        RYLR.println(packet);
        delay(100); // for transmission of data
      }
      break;
    case ARMED:
      if (digitalRead(ARM_SWITCH) == LOW)
      {
        CURRENT_STATE = SAFE;
        delay(500); //debouncing delay
        message = "SAFE";
        packet = String("AT+SEND=")+RYLR_ADD+","+String(message.length())+","+message;
        Serial.println("CURRENT STATE = SAFE");
        RYLR.println(packet);
        delay(100); // for transmission of data
      }
      else if (digitalRead(LAUNCH_SWITCH) == HIGH)
      {
        CURRENT_STATE = LAUNCHED;
        delay(500); //debouncing delay
        message = "LAUNCH";
        packet = String("AT+SEND=")+RYLR_ADD+","+String(message.length())+","+message;
        Serial.println("CURRENT STATE = LAUNCHED");
        RYLR.println(packet);
        delay(100); // for transmission of data
      }
      break;

    case LAUNCHED:
      break;
  }
}

String parseRYLR(String input) 
{
  int start = input.indexOf(',') + 1;
  start = input.indexOf(',', start) + 1;
  int end = input.indexOf(',', start);
  return input.substring(start, end);  
}

void receiveData()
{
  if(Serial.available())
  {
    receive = Serial.readString();
    receive = parseRYLR(receive);

    if(receive.charAt(0) >= '0' && receive.charAt(0) <= '9')
      Serial.println("LOAD CELL READING: "+receive+" KG");
    else
      Serial.println("TESTBED CURRENT STATE: "+receive);
  }
}

void setup() 
{
  pinMode(ARM_SWITCH, INPUT);     // connect external pulldown resistor
  pinMode(LAUNCH_SWITCH, INPUT);  // connect external pulldown resistor

  Serial.begin(9600);
  RYLR.begin(57600);
  
  if(CURRENT_STATE == SAFE)
  {
    message = "SAFE";
    packet = String("AT+SEND=")+RYLR_ADD+","+String(message.length())+","+message;
    Serial.println("CURRENT STATE = SAFE");
    RYLR.println(packet);
  }
}

void loop() 
{
  stateInput();
  receiveData();
}
