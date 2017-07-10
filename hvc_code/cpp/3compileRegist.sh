#!/bin/bash
g++ -o registFace faceregister.cpp -I/usr/local/include -L/usr/local/lib -lwiringPi -std=gnu++0x -lfluent 
