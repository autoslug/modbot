from flask import Flask
from datetime import datetime, timedelta

###
# This should be run on the user's machine.
# You can check `localhost:80/status` to see if the raspi has reported recently.
###

app = Flask(__name__)
last_heartbeat_time = datetime.now()

# Set the maximum allowed time between heartbeats in seconds
MAX_HEARTBEAT_INTERVAL = 15

@app.route('/', methods=['GET'])
def log_heartbeat():
    global last_heartbeat_time

    # Update the last heartbeat time
    last_heartbeat_time = datetime.now()

    print("Received heartbeat!")
    return "OK", 200

@app.route('/status', methods=['GET'])
def check_status():
    global last_heartbeat_time

    # Check if the heartbeat is received within the allowed timeframe
    if datetime.now() - last_heartbeat_time > timedelta(seconds=MAX_HEARTBEAT_INTERVAL):
        return "Error: Heartbeat not received within the allowed timeframe", 500
    else:
        return "OK", 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80)
