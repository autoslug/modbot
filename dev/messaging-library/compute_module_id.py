"""Defined list of valid message ids and helper function to
check and process messages into actions"""

from enum import Enum


class MessageProcessResult (Enum):
    """state of robot after message has been fully processed"""
    PAYLOAD_ID_NOT_FOUND = 0


result_array = {}


def messaging_process_payload(payload, id) -> MessageProcessResult:
    """
    process_payload: reads parameter of specified length
    and performs actions based on the id

    @param payload string
    @param id id of payload

    @returns success/failure to process payload,
    next state of main state machine
    """
    print(payload)
    print(id)
    return MessageProcessResult.PAYLOAD_ID_NOT_FOUND
