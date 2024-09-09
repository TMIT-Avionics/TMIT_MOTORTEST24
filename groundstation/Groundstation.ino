#include <SoftwareSerial.h>
#include <SD.h>

#define RXD 7
#define TXD 6

SoftwareSerial RYLR(RXD, TXD);

String message, response;
File logFile;
static int logCount = 0;

void setup() {
  Serial.begin(9600);

  //RYLR setup
  RYLR.begin(57600);
  Serial.println("\nSENDING AT TRANSMIT.");

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
    Serial.println("Logging to SD card...");
  }

}

void loop() {
  if (RYLR.available()) {
    //RYLR get response
    response = RYLR.readStringUntil('\n');
    RYLR.flush();
    Serial.println("RESPONSE: " + response);

    //SD card store
    logFile = SD.open("loadcell.txt", FILE_WRITE);
    logFile.println(response);
    logFile.close();
  }
}

