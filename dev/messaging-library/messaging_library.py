"""
messaging_library encodes and decodes messages that are sent over i2c between
raspberry pis and raspberry picos for modbot
"""

from enum import Enum
import compute_module_id

HEAD = 0xCC
TAIL = 0xB9

# using pep 8 and flake 8 linter for code styling
# DOES NOT have buffer overflow error due to how python handles strings


class MessageReadState (Enum):
    """enums that describe the state of the state machine
    in messaging_readbuffer"""
    MESSAGE_ERROR = -1
    MESSAGE_HEAD = 0
    MESSAGE_LENGTH = 1
    MESSAGE_PAYLOAD = 2
    MESSAGE_TAIL = 3
    MESSAGE_CHECKSUM = 4
    MESSAGE_END = 5


class MessageParseResult (Enum):
    """enums that describe the success state of messaging_readbuffer"""
    MESSAGE_PARSE_FAILURE = -1
    MESSAGE_PARSE_NONE = 0
    MESSAGE_PARSE_SUCCESS = 1


def messaging_bsd_checksum(payload_string: bytes) -> int:
    """

    messaging_checksum: Calculates checksum for messaging,
    using BSDChecksum algorithm. The max length is 128 bytes.
    The first byte is the message ID, the next 127 bytes is the payload

    Args:
    payloadString (bytes): string of payload and id, 128 bytes max

    Returns:
    int: calculated checksum

    """
    checksum = 0
    for idx in payload_string:
        checksum = (checksum >> 1) | (checksum << 7)
        # 8-bit rotation
        checksum = checksum & 0b11111111
        # mask int to only include lower 8 bits
        checksum = int(bin(idx), 2) + int(bin(checksum), 2)
        # add values bitwise
        checksum = checksum & 0b11111111
        # mask int to only include lower 8 bits
    return checksum


def messaging_readbuffer(buffer_string: bytes, result_string: bytes,
                         robot_state: Enum) -> bool:
    """

    messaging_readbuffer: Uses state machine to take the buffer parameter
    and processes the message into the result string;
    Returns true/false based on if conversion was successful.

    Packet Structure:
    HEAD LENGTH PAYLOAD TAIL CHECKSUM END
      1     1   (1)<128   1      1    \r\n

    Head: 1 byte 0xCC
    Length: 1 byte, length of payload in bytes
    Payload: Variable length < 128 bytes, always proceeded by a 1 byte ID
    Tail: 1 byte 0xB9
    Checksum: 1 byte checksum calculated over payload
    using BSDChecksum algorithm
    End: 2 bytes \r\n

    Args:
    buffer_string (bytes): string that will be processed
    result_string (bytes): string that the result is stored in
    robot_state (enum): variable containing state of robot that is modified
    when a message is fully processed

    Returns:
    enum: true false or none based on success of function
          or if buffer contains no message

          DOES NOT HAVE BUFFER OVERFLOW ERROR
    """
    state = MessageReadState.MESSAGE_HEAD
    payload_idx = 0
    payload_len = 0
    calculated_checksum = 0
    message_id = 0
    message_parse_result = MessageParseResult.MESSAGE_PARSE_NONE
    robot_result_state = 0

    # note: since I am writing both the library in c and python,
    # I wanted to make the logic the same between both, so it could be easier
    # to update/change the whole library in the future
    for idx, current_char in enumerate(buffer_string):
        if state == MessageReadState.MESSAGE_HEAD:
            if current_char == HEAD:
                state = MessageReadState.MESSAGE_LENGTH
        elif state == MessageReadState.MESSAGE_LENGTH:
            payload_len = int(current_char)

            # expected payload range check
            if payload_len < 2:
                state = MessageReadState.MESSAGE_ERROR

            payload_idx = 0
            payload_string = buffer_string[(idx + 1):(idx + 1 + payload_len)]
            calculated_checksum = messaging_bsd_checksum(payload_string)
            # calculated_checksum = hex(calculated_checksum).encode()
            state = MessageReadState.MESSAGE_PAYLOAD

        elif state == MessageReadState.MESSAGE_PAYLOAD:

            # assign/store id
            if payload_idx == 0:
                message_id = current_char

            if result_string:
                pass
            # actual payload range check
            # (this is implemented slightly differently
            # as python for loops work differently)
            if payload_idx < payload_len - 1:
                if current_char == TAIL:
                    state = MessageReadState.MESSAGE_ERROR
                else:
                    payload_idx += 1
            else:
                if current_char == TAIL:
                    state = MessageReadState.MESSAGE_ERROR
                else:
                    robot_result_state = compute_module_id.\
                        messaging_process_payload(payload_string[1:],
                                                  message_id)
                    state = MessageReadState.MESSAGE_TAIL

        elif state == MessageReadState.MESSAGE_TAIL:
            if current_char == TAIL:
                state = MessageReadState.MESSAGE_CHECKSUM
            else:
                state = MessageReadState.MESSAGE_ERROR

        elif state == MessageReadState.MESSAGE_CHECKSUM:
            if calculated_checksum != current_char:
                state = MessageReadState.MESSAGE_ERROR
            else:
                state = MessageReadState.MESSAGE_END

        elif state == MessageReadState.MESSAGE_END:
            next_char = buffer_string[idx + 1]
            if current_char == 0x0D and next_char == 0x0A:
                message_parse_result = MessageParseResult.MESSAGE_PARSE_SUCCESS
                state = MessageReadState.MESSAGE_HEAD
                robot_state = robot_result_state
                print(str(robot_state)[0:0])  # silence error
                break
            else:
                state = MessageReadState.MESSAGE_ERROR

        else:
            message_parse_result = MessageParseResult.MESSAGE_PARSE_FAILURE
            break

    return message_parse_result


def messaging_write_message(payload: bytes) -> bytes:
    """

    WriteMessage:
    converts parameter payload into a valid message
    string and writes to parameter message.
    Payload corresponds to PAYLOAD in the packet structure:
    [HEAD, LENGTH, PAYLOAD, TAIL, CHECKSUM, END]

    Args:
    payload (str): string that will be converted to a valid message format

    Returns:
    message (str): string that the message will be written to
    """
    payload_len = len(payload)
    payload_checksum = messaging_bsd_checksum(payload)
    converted_payload = []
    for char in payload:
        converted_payload.append(char)
    bytes_return = bytes([HEAD, payload_len] + converted_payload +
                         [TAIL, payload_checksum, 13, 10])
    return bytes_return
