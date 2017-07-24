#!/bin/bash
g++ -o exe-camera camera.cpp -I/usr/local/include -L/usr/local/lib -lwiringPi -std=gnu++0x -lfluent -lsqlite3 
