This repository contains 2 files used in the final design and the lab report.
Old code used to test certain parts of the project is included in TestingFiles

readBluetooth.m     Is a matlab file we used to read the proximity sensor data from
                    the bluetooth module.
bluetooth.c         Is userspace code responsible for transmitting, recieving, and 
                    moving the robot around based on recieved signals.
EE474labreport5.pdf Is our lab report for the lab


Within TestingFiles:

bluetooth1.c        Is a modified copy of bluetooth.c used to test which program
                    was running on boot.
connect_uart1_uart4.c 
                    Is a program used to verify the functionality of our UART 
                    ports.
simple_listener.c   Is a program created during lab4 with important code we used
                    in lab 5.
simple_timer.c      Is a program created during lab4 to be used with 
                    simple_listener.c with important code we used in lab 5.