 /*
  EBYTE E32-900T30D Transceiver Example
  Arduino UNO
  -----------------
  Connections:
  E32      | Arduino UNO
  -----------------
  M0       | D7
  M1       | D6
  AUX      | D5
  TX       | D2 (SoftwareSerial RX)
  RX       | D3 (SoftwareSerial TX)
  VCC      | 5V
  GND      | GND
*/

#include <SoftwareSerial.h>

#define M0 7
#define M1 6
#define AUX 5

#define RX_PIN 2   // Arduino receives on D2 from module TX
#define TX_PIN 3   // Arduino transmits on D3 to module RX

SoftwareSerial E32(RX_PIN, TX_PIN); // RX, TX

void waitAuxHigh() {
  while (digitalRead(AUX) == LOW) {
    delay(1);
  }
  delay(20); // small buffer
}

void setup() {
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  pinMode(AUX, INPUT);

  digitalWrite(M0, LOW);
  digitalWrite(M1, LOW); // Mode 0 = Normal (transmit/receive)

  Serial.begin(9600);
  E32.begin(9600);

  Serial.println("E32-900T30D Transceiver Ready!");
}

void loop() {
  // ----------- SEND -----------
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    Serial.print("Sending: ");
    Serial.println(msg);

    waitAuxHigh();
    E32.print(msg);   // Send message via LoRa
  }

  // ----------- RECEIVE -----------
  if (E32.available()) {
    String incoming = E32.readString();
    Serial.print("Received: ");
    Serial.println(incoming);
  }
}
