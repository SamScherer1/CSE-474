This project contains multiple programs:

    "simple_listener.c" contains the main program to run on the BeagleBone, which involves controlling the motors and sensors to spin, and to look for and chase an object.

    "simple_timer.c" contains the timer that interrupts "simple_listener.c" and reads ADC inputs.
    
    "doBoth.sh" is a bash script that compiles and runs the above programs.
    
    "hBridge.c" is a program that demonstrates the use of the H-bridge for controlling the motors.
    
    "adcRead.c" is a program that reads and prints the current adc value when run.

See the lab 4 report for more details on the implementation of the programs, and for the systemd commands used to run the main program on the BeagleBone on startup.