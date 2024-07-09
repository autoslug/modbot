#include "swerve-module-id.h"

#define PAYLOAD_MAX_LEN 127
#define HEAD_ID 0xCC
#define TAIL_ID 0xB9

enum messageParseResult {
    MESSAGE_BUFFER_OVERFLOW = -2,
    MESSAGE_PARSE_FAILURE,
    MESSAGE_PARSE_NONE,
    MESSAGE_PARSE_SUCCESS
};

enum messageReadState {
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
char MessagingBSDChecksum(char* payloadString, unsigned char payloadLen) {
    unsigned char checksum = 0;
    for(short i = 0; i < payloadLen; i++) {
        checksum = (checksum >> 1) | (checksum << 7);
        checksum += payloadString[i];
    }
    return checksum;
}

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
char MessagingReadBuffer(char* bufferString, char* resultString, int bufferLength, char* robotState) { // turn result to struct later
    short idx = 0;
    char state = MESSAGE_HEAD;
    char currentChar = bufferString[idx];
    char payloadLen = 0;
    char payloadIdx = 0;
    char messageID = 0;
    char calculatedChecksum = 0;
    char messageParseResult = MESSAGE_PARSE_NONE;
    char robotResultState = 0;

    while (idx < bufferLength) {
        if(state == MESSAGE_HEAD) {
            if(currentChar == (char)HEAD_ID) {
                state = MESSAGE_LENGTH;
            }
        }else if(state == MESSAGE_LENGTH) {
            payloadLen = currentChar;

            // expected payload range check
            if(payloadLen < 2) {
                state = MESSAGE_ERROR;
            }

            payloadIdx = 0;
            char* payload = bufferString + idx + 1;
            calculatedChecksum = MessagingBSDChecksum(payload, payloadLen);
            state = MESSAGE_PAYLOAD;

        }else if(state == MESSAGE_PAYLOAD) {

            // assign/store id
            if(payloadIdx == 0) {
                messageID = currentChar;
            }

            // actual payload length check
            if(payloadIdx < payloadLen - 1) {
                if(currentChar == (char)TAIL_ID) {
                    state = MESSAGE_ERROR;
                }else {
                    payloadIdx++;
                }
            }else {
                if(currentChar == (char)TAIL_ID) {
                    state = MESSAGE_ERROR;
                }else {
                    robotResultState = MessagingProcessPayload(bufferString + idx - payloadLen + 2, messageID, payloadLen - 1);
                    state = MESSAGE_TAIL;
                }
            }

        }else if(state == MESSAGE_TAIL) {
            if(currentChar == (char)TAIL_ID) {
                state = MESSAGE_CHECKSUM;
            }else{
                state = MESSAGE_ERROR;
            }

        }else if(state == MESSAGE_CHECKSUM) {
            if(calculatedChecksum != currentChar) {
                state = MESSAGE_ERROR;
            }else {
                state = MESSAGE_END;
            }

        }else if(state == MESSAGE_END){
            char nextChar = bufferString[idx + 1];
            if(currentChar == '\r' && nextChar == '\n') {
                messageParseResult = MESSAGE_PARSE_SUCCESS;
                state = MESSAGE_HEAD;
                *robotState = robotResultState;
                break;
            }else {
                state = MESSAGE_ERROR;
            }

        }else {
            messageParseResult = MESSAGE_PARSE_FAILURE;
            break;
        }

        idx++;
        currentChar = bufferString[idx];
    }
    
    if(state != MESSAGE_HEAD && state != MESSAGE_ERROR) {
        messageParseResult = MESSAGE_BUFFER_OVERFLOW;
    }

    return messageParseResult;
}

/**
 * WriteMessage: converts parameter payload into a valid message string and writes it to parameter message. 
 * Payload corresponds to PAYLOAD in the packet structure: [HEAD, LENGTH, PAYLOAD, TAIL, CHECKSUM, END]
 * 
 * @param payload string that will be converted to a valid message format 
 * @param message string that the message will be written to
 * 
 * @return None
 */
void MessagingWriteMessage(unsigned  char payload[], char message[]){
    char payloadLen = 0;
    for(;payload[payloadLen] != '\0'; payloadLen++){
        message[payloadLen + 2] = payload[payloadLen];
    }
    message[0] = (char)HEAD_ID;
    message[1] = payloadLen;
    message[payloadLen + 2] = (char)TAIL_ID;
    message[payloadLen + 3] = MessagingBSDChecksum(payload, payloadLen);
    message[payloadLen + 4] = '\r';
    message[payloadLen + 5] = '\n';
    message[payloadLen + 6] = '\0';
}
