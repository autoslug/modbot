#include "messaging-library.h"


char MessagingBSDChecksum(char *payloadString, unsigned char payloadLen)
{
    unsigned char checksum = 0;
    for (short i = 0; i < payloadLen; i++)
    {
        checksum = (checksum >> 1) | (checksum << 7);
        checksum += payloadString[i];
    }
    return checksum;
}


char MessagingReadBuffer(char *bufferString, char *resultString, int bufferLength, char *robotState)
{ // turn result to struct later
    short idx = 0;
    char state = MESSAGE_HEAD;
    char currentChar = bufferString[idx];
    char payloadLen = 0;
    char payloadIdx = 0;
    char messageID = 0;
    char calculatedChecksum = 0;
    char messageParseResult = MESSAGE_PARSE_NONE;
    char robotResultState = 0;

    while (idx < bufferLength)
    {
        if (state == MESSAGE_HEAD)
        {
            if (currentChar == (char)HEAD_ID)
            {
                state = MESSAGE_LENGTH;
            }
        }
        else if (state == MESSAGE_LENGTH)
        {
            payloadLen = currentChar;

            // expected payload range check
            if (payloadLen < 2)
            {
                state = MESSAGE_ERROR;
            }

            payloadIdx = 0;
            char *payload = bufferString + idx + 1;
            calculatedChecksum = MessagingBSDChecksum(payload, payloadLen);
            state = MESSAGE_PAYLOAD;
        }
        else if (state == MESSAGE_PAYLOAD)
        {

            // assign/store id
            if (payloadIdx == 0)
            {
                messageID = currentChar;
            }

            // actual payload length check
            if (payloadIdx < payloadLen - 1)
            {
                if (currentChar == (char)TAIL_ID)
                {
                    state = MESSAGE_ERROR;
                }
                else
                {
                    payloadIdx++;
                }
            }
            else
            {
                if (currentChar == (char)TAIL_ID)
                {
                    state = MESSAGE_ERROR;
                }
                else
                {
                    robotResultState = MessagingProcessPayload(bufferString + idx - payloadLen + 2, messageID, payloadLen - 1);
                    state = MESSAGE_TAIL;
                }
            }
        }
        else if (state == MESSAGE_TAIL)
        {
            if (currentChar == (char)TAIL_ID)
            {
                state = MESSAGE_CHECKSUM;
            }
            else
            {
                state = MESSAGE_ERROR;
            }
        }
        else if (state == MESSAGE_CHECKSUM)
        {
            if (calculatedChecksum != currentChar)
            {
                state = MESSAGE_ERROR;
            }
            else
            {
                state = MESSAGE_END;
            }
        }
        else if (state == MESSAGE_END)
        {
            char nextChar = bufferString[idx + 1];
            if (currentChar == '\r' && nextChar == '\n')
            {
                messageParseResult = MESSAGE_PARSE_SUCCESS;
                state = MESSAGE_HEAD;
                *robotState = robotResultState;
                break;
            }
            else
            {
                state = MESSAGE_ERROR;
            }
        }
        else
        {
            messageParseResult = MESSAGE_PARSE_FAILURE;
            break;
        }

        idx++;
        currentChar = bufferString[idx];
    }

    if (state != MESSAGE_HEAD && state != MESSAGE_ERROR)
    {
        messageParseResult = MESSAGE_BUFFER_OVERFLOW;
    }

    return messageParseResult;
}

void MessagingWriteMessage(unsigned char payload[], char message[])
{
    char payloadLen = 0;
    for (; payload[payloadLen] != '\0'; payloadLen++)
    {
        message[payloadLen + 2] = payload[payloadLen];
    }
    message[0] = (char)HEAD_ID;
    message[1] = payloadLen;
    message[payloadLen + 2] = (char)TAIL_ID;
    message[payloadLen + 3] = MessagingBSDChecksum((char*)payload, payloadLen);
    message[payloadLen + 4] = '\r';
    message[payloadLen + 5] = '\n';
    message[payloadLen + 6] = '\0';
}
