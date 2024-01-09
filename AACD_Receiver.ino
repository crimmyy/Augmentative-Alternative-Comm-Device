/********************************************************************
Pin Map

Pin Number | Type   | Connection

2          | INPUT  | NRF24L01 Module RF24 CE

3          | INPUT  | NRF24L01 Module RF24 CSN (SS)

4          | EMPTY  | 

5          | INPUT  | Reset Button

6          | OUTPUT | NeoPixel 

7          | OUTPUT | E-ink Module BUSY

8          | OUTPUT | E-ink Module RST

9          | OUTPUT | E-ink Module DC

10         | OUTPUT | E-ink Module CS (SS)

11         | SHARED | NRF24L01 Module MOSI & E-ink Module DIN (MOSI)

12         | OUTPUT | NRF24L01 Module MISO 

13         | SHARED | NRF24L01 Module SCK & E-ink Module CLK (SCK)
********************************************************************/

#include <SPI.h>

#include <nRF24L01.h>

#include <RF24.h>

#include "epd2in9_V2.h"

#include "epdpaint.h"

#include <Adafruit_NeoPixel.h>

// Define Constants
#define COLORED 0
#define UNCOLORED 1
#define PIN_NEO_PIXEL 6
#define NUM_PIXELS 8

                                        // NRF24L01 Setup
                                        RF24 radio(2, 3);
const byte address[6] = "60223";

// Button Setup
const int resetButtonPin = 5;

// e-ink Display Setup
Epd epd;
unsigned char image[1100];
Paint paint(image, 0, 0);
bool isDisplayOn = false;
bool lastResetButtonState = HIGH;
bool messageReceived = false;

// NeoPixel Strip Setup
Adafruit_NeoPixel NeoPixel(NUM_PIXELS, PIN_NEO_PIXEL, NEO_GRB + NEO_KHZ800);

void setup() {
  // Radio Start
  Serial.begin(9600);

  // Initialize Radio
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  // Initialize Button Pin
  pinMode(resetButtonPin, INPUT_PULLUP);

  // Initialize e-ink Display
  if (epd.Init() != 0) {
    Serial.println("e-Paper init failed");
    return;
  }
  epd.ClearFrameMemory(0xFF);
  epd.DisplayFrame();

  // Initialize NeoPixel Strip
  NeoPixel.begin();
  NeoPixel.setBrightness(10);
  setNeoPixelColor(33, 138, 255);
}



void loop() {
  // Check for Messages
  if (radio.available()) {
    char receivedMessage;
    radio.read(&receivedMessage, sizeof(receivedMessage));
    Serial.print("Received message: ");
    Serial.println(receivedMessage);
    displayMessage(receivedMessage);
    messageReceived = true;
  }

  // Check Reset Button Change
  bool currentResetButtonState = digitalRead(resetButtonPin);

  if (currentResetButtonState != lastResetButtonState && currentResetButtonState == LOW) {
    if (isDisplayOn) {
      clearDisplay();
      messageReceived = false;
      setNeoPixelColor(33, 138, 255);
    }
  }

  lastResetButtonState = currentResetButtonState;
  delay(50);
}

//Displaying Message Logic Helper Function

void displayMessage(char buttonNumber) {
  isDisplayOn = true;

  //NeoPixel Change
  NeoPixel.setBrightness(255);
  setNeoPixelColor(0, 255, 0);

  //Text Display Initialization
  paint.SetWidth(40);
  paint.SetHeight(220);
  paint.SetRotate(ROTATE_270);
  paint.Clear(UNCOLORED);

  //Message List
  const char* line1;
  const char* line2;

  switch (buttonNumber) {
    case '1':
      line1 = "Can I have";
      line2 = "more time?";
      break;

    case '2':
      line1 = "I'm feeling";
      line2 = "nervous/shy.";
      break;

    case '3':
      line1 = "I'm not comfortable";
      line2 = "with that.";
      break;

    case '4':
      line1 = "I'm having";
      line2 = "trouble focusing.";
      break;

    case '5':
      line1 = "I need";
      line2 = "some space.";
      break;

    default:
      line1 = "";
      line2 = "";
      break;
  }

  //e-ink Draw Logic
  paint.DrawStringAt(0, 8, line1, &Font16, COLORED);
  paint.DrawStringAt(0, 24, line2, &Font16, COLORED);
  epd.ClearFrameMemory(0xFF);
  epd.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrame();
}

//e-ink Sleep Logic
void clearDisplay() {
  isDisplayOn = false;
  epd.ClearFrameMemory(0xFF);
  epd.DisplayFrame();
  NeoPixel.setBrightness(10);
  setNeoPixelColor(33, 138, 255);
}

//NeoPixel Color Logic Helper Function
void setNeoPixelColor(uint8_t r, uint8_t g, uint8_t b) {

  for (int i = 0; i < NUM_PIXELS; i++) {
    NeoPixel.setPixelColor(i, NeoPixel.Color(r, g, b));
  }
  NeoPixel.show();
}