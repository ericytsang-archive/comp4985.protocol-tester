/* following constants are used to parse control messages. */

/* total number of sections in the packet */
#define PACKET_SECTIONS 3

/* sequence that the parts of the packet come in */
#define PACKET_SEQ_TYPE    0
#define PACKET_SEQ_LENGTH  1    // determines length of the payload
#define PACKET_SEQ_PAYLOAD 2

/* length of each packet section */
#define PACKET_LEN_TYPE    1
#define PACKET_LEN_LENGTH  4
