#ifndef SACN_H
#define SACN_H
// based on
// * https://tsp.esta.org/tsp/documents/docs/ANSI_E1-31-2018.pdf
//   * page 6 has table 4-1 the packet
// * https://github.com/forkineye/ESPAsyncE131/blob/master/ESPAsyncE131.h
// * https://github.com/tigoe/sACNSource/blob/main/src/sACNSource.cpp

#include <stdint.h>
#include <arpa/inet.h>


#define ACN_SDT_MULTICAST_PORT 5568

// E1.31 Packet Structure
typedef union {
    struct {
        // Root Layer
        uint16_t preamble_size; // static
        uint16_t postamble_size; // static
        uint8_t  acn_id[12]; // static
        uint16_t root_flength; // static if size doesn't change
        uint32_t root_vector; // static
        uint8_t  cid[16];

        // Frame Layer
        uint16_t frame_flength;
        uint32_t frame_vector; // static
        char     source_name[64]; // utf-8, null terminated
        uint8_t  priority;
        uint16_t synchronization_addresses; // universe where synchronizaation packets exist
        uint8_t  sequence_number;
        uint8_t  options;
        uint16_t universe;

        // DMP Layer
        uint16_t dmp_flength;
        uint8_t  dmp_vector; // staic
        uint8_t  type; // static
        uint16_t first_address; // static
        uint16_t address_increment; // static
        uint16_t property_value_count; //"Indicates 1+ the number of slots in packet", range 1 to 513
        uint8_t  property_values[513]; // I guess this size could be 1-513
                                       // the first byte is start code, which shoudl probably be 0?
    } __attribute__((packed));

    uint8_t raw[638];
} e131_packet_t;

/**
 * init static data in E1.31 packet
 *
 * cid is Sender's unique ID, use a uuidv4 generator
 * name is the Source Name, UTF-8, 64 chars max
 **/
void initPacket(e131_packet_t* p, const char* cid, const char* name);
#endif // SACN_H
