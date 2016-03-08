all: robot

robot: robot.o
	g++ robot.o  -lm -lSDL -o robot

robot.o: robot.cpp
	g++ -O2 -std=c++0x -c robot.cpp

rpiPWM1.o: rpiPWM1.cpp
	g++ -c rpiPWM1.cpp

clean:
	rm *.o robot

install: all
	systemctl stop robot.service
	cp robot /usr/local/bin/robot
	systemctl start robot.service
