#!/bin/bash

###
# This should be run on the raspi to send heartbeat to the user's computer
###


# Set the remote server URL to first argument
REMOTE_SERVER_URL="$1"
# Command to run if heartbeat fails to send
CMD_ON_FAIL="$2"
# Seconds to wait before sending heartbeat
HEARTBEAT_INTERVAL=0.5
# Flag to track if CMD_ON_FAIL has been run
CMD_RAN=0

# Function to send heartbeat
send_heartbeat() {
    curl --max-time 1 -s -o /dev/null -w "%{http_code}" "$REMOTE_SERVER_URL"
}

# Main loop for sending heartbeat every HEARTBEAT_INTERVAL seconds
# If heartbeat fails to send, run CMD_ON_FAIL once, then retry sending heartbeat
while true; do
    if [ "$(send_heartbeat)" == "200" ]; then
        echo "Heartbeat sent successfully"
        CMD_RAN=0
    else
        if [ "$CMD_RAN" -eq 0 ]; then
            echo "Failed to send heartbeat. Running command..."
            $CMD_ON_FAIL
            CMD_RAN=1
        else 
            echo "Failed to send heartbeat. Command already ran."
        fi
    fi

    sleep $HEARTBEAT_INTERVAL
done
