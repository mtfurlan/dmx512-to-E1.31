#include "sACN.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define VECTOR_ROOT_E131_DATA 0x00000004
#define VECTOR_E131_DATA_PACKET 0x00000002
#define VECTOR_DMP_SET_PROPERTY 0x02

// E1.17 ACN Packet Identifier
const uint8_t ACN_ID[12] = { 0x41, 0x53, 0x43, 0x2d, 0x45, 0x31, 0x2e, 0x31, 0x37, 0x00, 0x00, 0x00 };

// https://github.com/tigoe/sACNSource/blob/main/src/sACNSource.cpp#L87
void doCIDShit(uint8_t* output, const char* uuid)
{
    //TODO: I don't trust this, check that it can't overrun
    // add 16-byte UUID (bytes 22 - 37)
    int i = 0;    // loop iteration counter
                  // iterate in steps of two chars, so you can convert
                  // each two-char substring to an unsigned long:
    for (int c = 0; c < strlen(uuid); c += 2) {
        char temp[] = {uuid[c], uuid[c+1], 0};  // get the temp string
        uint8_t b = strtoul(temp, NULL, 16);     // convert it
        output[i] = b;               // add the byte to the array
        i++;                                  // increment the counter
    }
}
void initPacket(e131_packet_t* p, const char* cid, const char* name)
{
    // TODO: we have byte order problems with all the uint16_t
    p->preamble_size = 0x0010;
    p->postamble_size = 0;
    memcpy(p->acn_id, ACN_ID, 12);
    // flags and length
    p->root_flength = (0x7 << 12) | (sizeof(e131_packet_t) - 16);
    p->root_vector = VECTOR_ROOT_E131_DATA;
    doCIDShit(p->cid, cid);

    p->frame_flength = (0x7 << 12) | (sizeof(e131_packet_t) - 38);
    p->frame_vector = VECTOR_E131_DATA_PACKET;
    snprintf(p->source_name, 64, name);
    p->priority = 100;

    p->dmp_flength = (0x7 << 12) | (sizeof(e131_packet_t) - 115);
    p->dmp_vector = VECTOR_DMP_SET_PROPERTY;
    p->type = 0xA1;
    p->first_address = 0;
    p->address_increment = 1;
    p->property_value_count = 513;
}
