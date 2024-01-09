/********************************************************************
Pin Map

Pin Number | Type   | Connection

3          | INPUT  | Button #1

4          | INPUT  | Button #5

5          | INPUT  | Button #4

6          | INPUT  | Button #3

7          | OUTPUT | NRF24L01 Module RF24 CE

8          | OUTPUT | NRF24L01 Module RF24 CSN

9          | INPUT  | Button #2

10         | EMPTY  | 

11         | OUTPUT | NRF24L01 Module MOSI

12         | OUTPUT | NRF24L01 Module MISO

13         | OUTPUT | NRF24L01 Module SCK
********************************************************************/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Define Constants
#define COLORED 0
#define UNCOLORED 1

// NRF24L01 Setup
RF24 radio(7, 8);
const byte address[6] = "60223";

// Button Setup
const int buttonPins[] = {3, 9, 6, 5, 4};
bool lastButtonStates[5] = {HIGH, HIGH, HIGH, HIGH, HIGH};

void setup() {
   // Initialize Radio
   radio.begin();
   radio.openWritingPipe(address);
   radio.setPALevel(RF24_PA_MIN);
   radio.stopListening();

   // Button Pin Logic
   for (int i = 0; i < 5; i++) {
       pinMode(buttonPins[i], INPUT_PULLUP);

   }
}

void loop() {

   // Check Each Button for Change

   for (int i = 0; i < 5; i++) {
       bool currentButtonState = digitalRead(buttonPins[i]);
       if (currentButtonState != lastButtonStates[i] && currentButtonState == LOW) {
           char messageToSend = '1' + i;
           radio.write(&messageToSend, sizeof(messageToSend)); //Radio Transmission
       }
       lastButtonStates[i] = currentButtonState;
   }
   delay(50);
}