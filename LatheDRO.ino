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


const uint8_t APIN = 7;
const uint8_t BPIN = 8;
const uint8_t ZPIN = 6;

String myAddress;
int measureMode = 0;
const float myLathe = 0.004233333;
int posCounter = 30000;
int oldPos;
float diameter = 0.0;
int buttonState = 1;   
const int initCount = 32000;
int overrun=0;


// Interrupt service Routines
void ARDUINO_ISR_ATTR moveCounter() {
  overrun++;
  // Try to disregard noise. Interrupt is triggered on falling edge, so see if the actual value is low
  if (!digitalRead(APIN)){
    if (digitalRead(BPIN))
      posCounter += 1;
    else
      posCounter -= 1;
  }
  overrun--;
}


void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");
  myAddress = connectWifi();
  initDisplay();
  initialiseWebUI();
  // Set up the Encoder interrupts
  pinMode(APIN, INPUT_PULLUP);
  attachInterrupt(APIN, moveCounter, FALLING);
  pinMode(BPIN, INPUT_PULLUP);
  pinMode(ZPIN, INPUT_PULLUP);

}

void loop() {
  float currentPos;
  webloopHandler();
  int curbuttonState = digitalRead(ZPIN);
  if ( curbuttonState != buttonState ){
    Serial.print("Button Pressed ");
    Serial.println(curbuttonState);
    buttonState = curbuttonState;
    posCounter = initCount; // Put it half way so it never goes negative
  }
  if (overrun >1) {
    putText(0,1,20,20,"Err");
    Serial.println("Overrun"); 
  }
  String mode;
  
  if ( posCounter != oldPos || (millis() % 1000) == 0) {
    if (measureMode) {
    mode = " (Dia)";
    currentPos = diameter + ((posCounter - initCount)*2 * myLathe) ; // posCount -initCount = radius, therefore *2 for diameter
  } else {
    mode = " (mm)";
    currentPos = ((posCounter - initCount) * myLathe);
  }
    String position = String(currentPos,3);
    // Serial.println(position);
    putText(1,1,0,0,myAddress+mode);
    putText(0,2,0,10,position);
    // putText(0,1,0,20,String(posCounter));
    oldPos = posCounter;
  }
  delay(50);
}
