#!/bin/bash
gcc -std=gnu99 -o sig2 /root/simple_timer.c
gcc -std=gnu99 -o simple_listener /root/simple_listener.c
/root/simple_listener &
target_PID="$(pgrep -n simple_listener)"
/sig2 $target_PID

