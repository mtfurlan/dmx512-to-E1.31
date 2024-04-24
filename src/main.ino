// TODO: steal https://github.com/KauyonKais/artemis-lighting/blob/master/old_code/oldLight/ArtemisLight/ArtemisLight.ino

#include <Arduino.h>

#define CHANNELS_WE_CARE_ABOUT 15

#define LED_R 18
#define LED_G 19
#define LED_B 20

void dmxCB(const uint8_t* buffer) {
  for(int i=0; i<3; ++i) {
      Serial.printf("slot %d: val %d\n", i+1, buffer[i]);
  }
  analogWrite(LED_R, 255-buffer[0]);
  analogWrite(LED_G, 255-buffer[1]);
  analogWrite(LED_B, 255-buffer[2]);
}


void setup () {
  Serial.begin(115200);
  Serial2.begin(250000);
  Serial.println("hi");

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  analogWrite(LED_R, 255);
  analogWrite(LED_G, 255);
  analogWrite(LED_B, 255);
}


// like 10ms between dmx packets, and each byte is like 44μs, so timeout of 1ms is fine
#define TIMEOUT 1
void processDMX() {
    // we can pretend a dmx packet is 513 serial bytes
    // framing error, start code, then slot 1 to 512 (511 total)
    // not really sure what start code does, not really sure I care
    static uint64_t startedMissingData = 0;
    static uint16_t i = 0;
    static uint8_t buf[513];
    if (!Serial2.available() && i != 0) {
        if(startedMissingData == 0) {
            startedMissingData = millis();
        } else if(millis() - startedMissingData > TIMEOUT) {
            Serial.printf("reset at %d\n", i);
            i=0;
        }
    } else {
        startedMissingData = 0;
        while (Serial2.available()) {
            buf[++i] = Serial2.read();
            //Serial.printf("buf[%d] = 0x%02X\n", i-1, buf[i-1]);
            if(i >= 513) {
                dmxCB(buf+2);
                i = 0;
            }
        }
        //Serial.printf("i is %d\n", i);
    }
}
void loop() {
    processDMX();
}
