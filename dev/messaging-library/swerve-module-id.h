/**
 * Defined list of valid message ids and helper function to check and process messages into actions
 */
#ifndef _MESSAGING_IDS
    #define _MESSAGING_IDS

    #define MOVE_IN_DIRECTION 0x80

    #define resultArraySize 6
    
    enum messageProcessResult {
        PAYLOAD_ID_NOT_FOUND,
        STATE_READ_RESULT_ARRAY
    };

    float resultArray[resultArraySize] = {0};
    char flippedBinaryArray[sizeof(float) * resultArraySize] = {0};
    /**
     * ProcessPayload: reads parameter of specified length and performs actions based on the id
     * 
     * @param payload pointer to beginning of payload
     * @param id id of payload
     * @param len length of payload
     * 
     * @returns success/failure to process payload, next state of main state machine
     */
    char MessagingProcessPayload(char* payload, unsigned char id, unsigned char len){
        switch(id){
            case MOVE_IN_DIRECTION:
                // use https://www.h-schmidt.net/FloatConverter/IEEE754.html to convert between float and binary/hex
                // store buffer array of uint8 (or char) to n floats
                // bytes are stored in the wrong direction, flip and rewrite
                
                for(char i=0;i<len;i++)
                    flippedBinaryArray[i]=payload[len-1-i];

                for(char i=0;i<resultArraySize;i++) // write to floats (in reversed direction)
                    *(resultArray+(i)) = *(((float*) flippedBinaryArray)+(resultArraySize-1-i));

                return STATE_READ_RESULT_ARRAY;
            default:
                return PAYLOAD_ID_NOT_FOUND;
        };
    }
#endif