#include <Arduino.h>
#include <WiFi.h>
#include <AsyncUDP.h>
#include "sACN.h"

// define SSDI and SSID_PASS
#include "secret.h"

#define LED_R 4
#define LED_G 16
#define LED_B 17

#define DMX_RX 27


// you should probably change this I dunno
static const char* SACN_CID = "bd89e380-c838-4cb5-8bd0-81edb8d74336";
// this is what q light controller plus broadcasted at, and it works with wled
static const ip_addr_t ip = IPADDR4_INIT_BYTES(239, 255, 0, 2);

static AsyncUDP udp;
static e131_packet_t packet;
static uint8_t seq = 0;

void dmxCB(const uint8_t* buffer) {
    //for(int i=0; i<3; ++i) {
    //    Serial.printf("slot %d: val %d\r\n", i+1, buffer[i]);
    //}
    analogWrite(LED_R, buffer[0]/5); // too bright
    analogWrite(LED_G, buffer[1]/5);
    analogWrite(LED_B, buffer[2]/5);

    // TODO: if data doesn't change, only send three of the same packet
    // then send a new one oevery 800-1000ms
    // first byte is start code?
    memcpy(packet.property_values+1, buffer, 15); // TODO: length
    udp.writeTo(packet.raw, sizeof(packet), &ip, ACN_SDT_MULTICAST_PORT);
    packet.sequence_number = ++seq;
}


void setup () {
    initPacket(&packet, SACN_CID, "esp32 dmx relay");
    packet.universe = htons(1);


    Serial.begin(115200);
    //Serial2.begin(baud-rate, protocol, RX pin, TX pin);
    Serial2.begin(250000, SERIAL_8N1, DMX_RX, 0);
    Serial.println("hi");

    pinMode(LED_R, OUTPUT);
    pinMode(LED_G, OUTPUT);
    pinMode(LED_B, OUTPUT);
    analogWrite(LED_R, 255);
    analogWrite(LED_G, 255);
    analogWrite(LED_B, 255);

    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, SSID_PASS);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed");
        while (1) {
            delay(1000);
        }
    }
    Serial.println("Connected!");
    analogWrite(LED_R, 0);
    analogWrite(LED_G, 0);
    analogWrite(LED_B, 0);
}


// like 10ms between dmx packets, and each byte is like 44μs, so timeout of 1ms is fine
#define TIMEOUT 5
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
