#include <SPI.h>
#include <RH_RF95.h> 
#include <Wire.h>
#include <SFE_MicroOLED.h>

// $Header$
// Code for SAMD 21 Pro RF we call Talker.
// Talker has 5 toggle switches. each toggle switch
// has one wire going to ground on a breadboard and
// the other going to a digital pin on the SAMD 21
// Pro RF.

// We only transmit if any of the switches have changed
// state.

// We run one wire from ground on the Talker to ground
// on the breadboard.

// In our setup, we have an additional toggle switch 
// protected by a missile switch cover to switch Talker
// on or off. The switch's leads
// go into the "switch" contacts on Talker's board.
// Don't let these touch the breadboard.
  
// You don't need to run a separate wire to power a switch,
// contrary to most internet examples. We use INPUT_PULLUP
// when configuring the PIN. This also affects how
// we interpret the values coming in, as noted below.

// We currently display the string we're sending to our
// MicroLED, but later we'll reconfigure to receive messages
// from Listener.

// We've commented out most of the example code's original
// comments, but they can be read from your Arduino's example
// codes.

#define DC_JUMPER 1
#define PIN_RESET 9
MicroOLED oled(PIN_RESET, DC_JUMPER);
#define NUM_PINS 5
RH_RF95 rf95(12, 6);
int LED = 13;
int pins[] = {9, 4, 3, 2, 5};	// this is the order we chose
int prev[NUM_PINS];
float frequency = 923.4;	// match on receiver 
void setup()
{
  delay(100);   
  Wire.begin();
  oled.begin();
  oled.clear(ALL); 
  oled.clear(PAGE);
  
  for (int i = 0; i < NUM_PINS; i++) {
    pinMode(pins[i], INPUT_PULLUP);
  } 
  pinMode(LED, OUTPUT);
  if (rf95.init() == false){
    digitalWrite(LED, HIGH);
    delay(1000);
    digitalWrite(LED, LOW);
    delay(1000);
    while (1);
  }
  else{
    digitalWrite(LED, HIGH);
    delay(1000);
    digitalWrite(LED, LOW);
    delay(500);
  }
  rf95.setFrequency(frequency);
  rf95.setTxPower(14, false);
  for (int i = 0; i < NUM_PINS; i++) {
	// we take the opposite because of how we've
	// wired the switches
    prev[i] = !digitalRead(pins[i]);
  }

}

void loop()
{ 
  int changed = 0;
  uint8_t toSend[NUM_PINS + 1];
  toSend[NUM_PINS] = '\0';
  for (int i = 0; i < NUM_PINS; i++) {
    toSend[i] = !digitalRead(pins[i]);
    if (prev[i] != toSend[i]) {
      changed = 1;
    }
  if (changed) {
	  rf95.send(toSend, sizeof(toSend));
	  rf95.waitPacketSent();
  }
    prev[i] = toSend[i];
  }
  // Now wait for a reply
  byte buf[RH_RF95_MAX_MESSAGE_LEN];
  byte len = sizeof(buf);
  if (rf95.waitAvailableTimeout(2000)) {
    digitalWrite(LED, HIGH);
    delay(1000);
    digitalWrite(LED, LOW);
    delay(500);
  }
  delay(500);
  oled.clear(ALL);
  oled.setFontType(1);
  oled.setCursor(1,1);
  for (int i = 0; i < NUM_PINS; i++) {
  	oled.print(buf[i]);
  }
  oled.display();
}
