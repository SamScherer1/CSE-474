#!/bin/bash
# Note: This bash script includes commands that works on most Unix-based
# machine. But it doesn't mean that it works on the Phytec board. You should
# use the arm compile to compile the user-space C code (w/ gnu99 flag)
# and use scp to send the compiled executable to the Phytec board and then 
# run the last 2 commands in this script to make it work on the Phytec board.
# Also note that "pgrep" doesn't work on the Phytec board either, 
# so you need to use following command to get the process ID of the process:
#
# top | grep "./signal_receive_test"
#
# This tells you the process ID that you can send your signal to.

gcc -std=gnu99 -o sig2 signal_send_test.c
target_PID="$(pgrep -f signal_receive_test)"
./sig2 $target_PID
