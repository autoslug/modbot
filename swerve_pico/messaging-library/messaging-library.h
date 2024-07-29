#include "swerve-module-id.h"

#define PAYLOAD_MAX_LEN 127
#define HEAD_ID 0xCC
#define TAIL_ID 0xB9

enum messageParseResult
{
    MESSAGE_BUFFER_OVERFLOW = -2,
    MESSAGE_PARSE_FAILURE,
    MESSAGE_PARSE_NONE,
    MESSAGE_PARSE_SUCCESS
};

enum messageReadState
{
    MESSAGE_ERROR = -1,
    MESSAGE_HEAD,
    MESSAGE_LENGTH,
    MESSAGE_PAYLOAD,
    MESSAGE_TAIL,
    MESSAGE_CHECKSUM,
    MESSAGE_END
};

/**
 * BSDChecksum: Calculates checksum for messaging, using BSDChecksum algorithm. The max length is 128 bytes. The first byte is the message ID, the next 127 bytes is the payload
 *
 * @param payloadString string of payload and id, 128 bytes max
 * @param payloadLen length of payload with id
 *
 * @return calculated checksum
 */
char MessagingBSDChecksum(char *payloadString, unsigned char payloadLen);
/**
 * ReadBuffer: Uses state machine to take the buffer parameter and processes the message into the result string;
 * Returns true/false based on if conversion was successful. Changes value of robotState if message is valid.
 *
 * \nPacket Structure:
 * HEAD LENGTH PAYLOAD TAIL CHECKSUM END
 *   1     1   (1)<128   1      1    \r\n
 *
 * Head: 1 byte 0xCC
 * Length: 1 byte, length of payload in bytes
 * Payload: Variable length < 128 bytes, always proceeded by a 1 byte ID
 * Tail: 1 byte 0xB9
 * Checksum: 1 byte checksum calculated over payload using BSDChecksum algorithm
 * End: 2 bytes \r\n
 *
 * @param bufferString string that will be processed
 * @param resultString string that the result is stored in
 * @param bufferLength length of buffer function parses through
 * @param robotState pointer to variable containing state of robot that is modified
 *  when a message is fully processed
 *
 * @return true or false based on success of function
 */
char MessagingReadBuffer(char *bufferString, char *resultString, int bufferLength, char *robotState);
/**
 * WriteMessage: converts parameter payload into a valid message string and writes it to parameter message.
 * Payload corresponds to PAYLOAD in the packet structure: [HEAD, LENGTH, PAYLOAD, TAIL, CHECKSUM, END]
 *
 * @param payload string that will be converted to a valid message format
 * @param message string that the message will be written to
 *
 * @return None
 */
void MessagingWriteMessage(unsigned char payload[], char message[]);
