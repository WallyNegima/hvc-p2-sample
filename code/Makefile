CC := g++
CXXFLAGS := -I/usr/local/include -L/usr/local/lib -lwiringPi -std=gnu++0x  -lfluent
exe-camera: main.cpp
	g++ -I/usr/local/include -L/usr/local/lib -lwiringPi -std=gnu++0x  -lfluent -o exe-camera main.cpp
faceRegister: faceRegister.cpp
	g++ -I/usr/local/include -L/usr/local/lib -lwiringPi -std=gnu++0x  -lfluent -lsqlite3 -o faceRegister faceRegister.cpp
test: test.cpp 
	g++ -I/usr/local/include -L/usr/local/lib -lwiringPi -std=gnu++0x  -lfluent  -o libTester test.cpp
clean:
	$(RM) exe-camera faceRegister
