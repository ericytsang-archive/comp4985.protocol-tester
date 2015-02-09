/**
 * constants here are used to parse control messages.
 *
 * @sourceFile Packet.h
 *
 * @program    ProtocolTester.exe
 *
 * @date       2015-02-09
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 */

/**
 * total number of sections in the packet
 */
#define PACKET_SECTIONS 3

////////////////////////////////////////////////////
// sequence that the parts of the packet come in  //
////////////////////////////////////////////////////
/**
 * type identifier of the packet is the the first part of the packet
 */
#define PACKET_SEQ_TYPE    0
/**
 * contains the length of the payload data. this part of the packet follows
 *   right after the type of the packet
 */
#define PACKET_SEQ_LENGTH  1
/**
 * the payload of the packet. the length of this section is indicated by the
 *   previous section of the packet.
 */
#define PACKET_SEQ_PAYLOAD 2

///////////////////////////////////
// length of each packet section //
///////////////////////////////////
/**
 * type identifier of a packet is a character; it is always just one byte long.
 */
#define PACKET_LEN_TYPE    1
/**
 * length of the payload length part of the packet. this is always just an
 *   integer; it is 4 bytes long.
 */
#define PACKET_LEN_LENGTH  4
