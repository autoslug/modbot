# How to Use!
0. Connect the user's device to the raspi's network.
1. Get the `raspi` folder on the raspi.
2. Get the `user` folder on a user's device.
3. On the user's device, run `user.py` *first*.
4. On the raspi, run `raspi.sh` with appropriate arguments.
    - First argument is the user's connected IP address
    - Second argument is the command to run when the heartbeat fails
    - `./raspi.sh IP FAILCMD`
5. Enjoy!

## Example
- `./raspi.sh 10.42.0.3 ./failtest.sh`
- `./raspi.sh "10.42.0.3" "touch grass"`