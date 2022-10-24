/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x32 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/
#include <Arduino.h>


const uint8_t APIN = 3;
const uint8_t BPIN = 2;

String myAddress;
int measureMode;
const float myLathe = 0.004233333;
int posCounter = 30000;
int oldPos;
// Interrupt service Routines
void ARDUINO_ISR_ATTR moveCounter() {
    posCounter += 1;
}


void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");
  myAddress = connectWifi();
  initDisplay();
  // Set up the Encoder interrupts
  pinMode(APIN, INPUT_PULLUP);
  attachInterrupt(APIN, moveCounter, FALLING);
  pinMode(BPIN, INPUT_PULLUP);
  

}

void loop() {
  
  webServer();
  if ( posCounter != oldPos ) {
    String mode;
    if (measureMode) mode = " (Dia)";
      else mode = " (mm)";
    String position = String(posCounter * myLathe,3);
    // Serial.println(position);
    putText(1,1,0,0,myAddress+mode);
    putText(0,1,0,10,position);
    putText(0,1,0,20,String(posCounter));
    oldPos = posCounter;
  }
  delay(500);
}
