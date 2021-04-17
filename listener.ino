#include <SPI.h>
// listener.ino
// Listener listens for transmissions from Talker
// Listener is also a Sparkfun SAMD 21 Pro RF with a MiroLED
// Listener displays the code sent by Talker reflecting
// the state of Talker's toggle switches
// Later, Listener may have its own toggle switches to be
// able to transmit messages back to Talker, but for now
// it doesn't

// removed comments that came with example code

// always displays current(-ish) status of Talker's 
// toggle switches. later may make it flash when
// status changes

#include <RH_RF95.h>
#include <Wire.h>
#include <SFE_MicroOLED.h> 

#define DC_JUMPER 1
#define PIN_RESET 9 
#define NUM_PINS 5

MicroOLED oled(PIN_RESET, DC_JUMPER); 

RH_RF95 rf95(12, 6);

int LED = 13; 

int packetCounter = 0; 
long timeSinceLastPacket = 0; 
float frequency = 923.5;	// must match Talker

void setup()
{
  delay(100);   
  Wire.begin();

  oled.begin();
  oled.clear(ALL); 
  
  pinMode(LED, OUTPUT);

  if (rf95.init() == false){
    digitalWrite(LED,HIGH);
    delay(500);
    while (1);
  }
  else{
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);
  }
  rf95.setFrequency(frequency); 
}

void loop()
{
  if (rf95.available()){
    uint8_t buf[6];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)){
      digitalWrite(LED, HIGH); 
      timeSinceLastPacket = millis();
      oled.clear(PAGE);
      oled.clear(ALL);
      oled.setFontType(1);
      oled.setCursor(1,1);
      for (int i = 0; i < NUM_PINS; i++) {
        oled.print(buf[i]);
      }
      oled.display();

	// later can do this only if have marked
	// message as read
      uint8_t toSend[] = "Hello Back!"; 
      rf95.send(toSend, sizeof(toSend));
      rf95.waitPacketSent();
      digitalWrite(LED, LOW); 
    }
  }
}
