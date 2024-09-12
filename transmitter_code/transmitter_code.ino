
///////////////////////////////////////////////////////////////////////////////
/*
WIRELESS HOME SECURITY USING NRF24L01

Developed by:
- Daniel George Ashok
- Parthiv Raj
- Rishab Prejesh

// Transmitter Code for Wireless Home Security System

*/

//////////////////////////////////////////////////////////////////////////////
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>

#define BUTTON_PIN 2

RF24 radio(9, 10);  // CE, CSN
RF24Network network(radio);
RF24Mesh mesh(radio, network);

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);
  mesh.begin();  // Start mesh, dynamically assign an address
}

void loop() {
  mesh.update();
  mesh.DHCP();  // Manage dynamic node addressing

  if (digitalRead(BUTTON_PIN) == LOW) {
    unsigned long buttonState = millis(); // Send timestamp or any other data
    // Write data to the final device (Node ID 2 in this example)
    if (mesh.write(2, &buttonState, 1, sizeof(buttonState))) {
      Serial.println("Message sent!");
    } else {
      Serial.println("Message failed.");
    }
  }
  delay(1000); // Delay to prevent spamming
}
