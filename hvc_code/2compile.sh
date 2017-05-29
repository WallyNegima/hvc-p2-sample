#!/bin/bash
gcc -o exe-camera camera.c -I/usr/local/include -L/usr/local/lib -lwiringPi
./exe-camera
