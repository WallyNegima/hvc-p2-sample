#!/bin/bash
gcc -o exe-serial serialraw.c -I/usr/local/include -L/usr/local/lib -lwiringPi
./exe-serial
