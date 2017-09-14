CC := g++
CXXFLAGS := -I/usr/local/include -L/usr/local/lib -lwiringPi -std=gnu++0x  -lfluent
exe-camera: main.cpp
	g++ -I/usr/local/include -L/usr/local/lib -lwiringPi -std=gnu++0x  -lfluent -o exe-camera main.cpp
faceRegister: faceRegister.cpp
	g++ -I/usr/local/include -L/usr/local/lib -lwiringPi -std=gnu++0x  -lfluent -o faceRegister faceRegister.cpp
clean:
	$(RM) exe-camera faceRegister
