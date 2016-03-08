#include <SDL/SDL.h>
#include <stdlib.h>
#include <cmath>
#include <csignal>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

#define PI 3.14159265359
#define MAX_X 32768

using namespace std;

ofstream *pin22;
ofstream *pin2;
ofstream *pin9;
ofstream *pin11;
ofstream *pin_handbrake;

int pulsator(float q, int i) {
        if (q<0) return -pulsator(-q,i);
        int numerator = (int)floor(abs(q*64));
        int reversed = ((i&1)  << 5) |
                       ((i&2)  << 3) |
                       ((i&4)  << 1) |
                       ((i&8)  >> 1) |
                       ((i&16) >> 3) |
                       ((i&32) >> 5);
        if (reversed < numerator) return 1;
        else return 0;
}

void set_dir(float left, float right, int i) {
	int leftpulse = pulsator(left,i);
	int rightpulse = pulsator(right,i);
	*pin22 << "0";
	*pin2 << "0";
	*pin11 << "0";
	*pin9 << "0";
	switch(leftpulse)
	{
	case 0:
		break;
	case 1:
		*pin22 << "1"; break;
	case -1:
		*pin2 << "1"; break;
	}

	switch(rightpulse)
	{
	case 0:
		break;
	case 1:
		*pin11 << "1"; break;
	case -1:
		*pin9 << "1"; break;
	}
	pin22 -> flush();
	pin2 -> flush();
	pin9 -> flush();
	pin11 -> flush();
};
int set_speed_radial(float phi, float r,int i){
	float left, right;
	int spd = r;
	while (phi<0) phi += 2*PI;
	phi = fmod(phi, 2*PI);
	if(phi < PI/4) {
		right = 1 - (phi/(PI/4));
		set_dir(r,r*right,i);
	} else if(phi < PI/2){
		right = -(phi - PI/4)/(PI/4);
		set_dir(r,r*right,i);
	} else if(phi < 3*PI/4){
		left = 1 - (phi - PI/2)/(PI/4);
		set_dir(r*left,-r,i);
	} else if(phi < PI){
		left = -(phi - 3*PI/4)/(PI/4);
		set_dir(r*left,-r,i);
	} else if(phi < 5*PI/4){
		right = -(1-(phi-PI)/(PI/4));
		set_dir(-r,r*right,i);
	} else if(phi < 3*PI/2){
		right = (phi - 5*PI/4)/(PI/4);
		set_dir(-r,r*right,i);
	} else if(phi < 7*PI/4){
		left = -(1-(phi-3*PI/2)/(PI/4));
		set_dir(r*left,r,i);
	} else {
		left = (phi-7*PI/4)/(PI/4);
		set_dir(r*left,r,i);
	}
}
void cleanup(int signum)
{
	*pin_handbrake << "0";
	pin_handbrake->close();
	ofstream gpiunexport("/sys/class/gpio/unexport");
	gpiunexport << std::unitbuf;
	*pin22 << "0";
	pin22->close();
	*pin2 << "0";
	pin2->close();
	*pin9 << "0";
	pin9->close();
	*pin11 << "0";
	pin11->close();
	ofstream pindir("/sys/class/gpio/gpio9/direction");
	pindir << "in";
	pindir.close(); pindir.open("/sys/class/gpio/gpio11/direction");
	pindir << "in";
	pindir.close(); pindir.open("/sys/class/gpio/gpio22/direction");
	pindir << "in";
	pindir.close(); pindir.open("/sys/class/gpio/gpio2/direction");
	pindir << "in";
	pindir.close(); pindir.open("/sys/class/gpio/gpio10/direction");
	pindir << "in";
	pindir.close();
	gpiunexport << "22";
	gpiunexport.flush();
	gpiunexport << "2";
	gpiunexport.flush();
	gpiunexport << "9";
	gpiunexport.flush();
	gpiunexport << "11";
	gpiunexport.flush();
	gpiunexport << "10";
	gpiunexport.flush();
	gpiunexport.close();
	this_thread::sleep_for(chrono::milliseconds(900));
	exit(0);
}

int joystickloop(){
	int rc = SDL_Init(SDL_INIT_JOYSTICK);
	if (rc<0) {
		printf("Error");
		return -1;
	};
	
	cout << "Searching for joysticks\n";
	if (SDL_NumJoysticks() == 0)
	{
		cout << "Joystick not found";
		return -1;
	} 
	
	SDL_Joystick *js = SDL_JoystickOpen(0);
	cout << "Found joystick, starting robot\n";

	int ok = 1;
	double x;
	double y;
	double r;
	double phi;
	int i = 0;
	clock_t start = clock();
	string mode;
	while (ok){
		i++;
		SDL_JoystickUpdate();
		x = SDL_JoystickGetAxis(js,0);
		y = SDL_JoystickGetAxis(js,1);
		int servo = (MAX_X+SDL_JoystickGetAxis(js,2))*120/(2*MAX_X)+110;
		if (abs(x)>abs(y)) r=abs(x); else r=abs(y);
		r = r/MAX_X;
		if (x==0 && y==0) phi =1;
		else phi = atan2(-x,y)+PI; //forward 0; Right pi/2; left 3*pi/2
		if (i % 30 ==0) cout << " phi:" << phi <<  " R:" << r << " x:" << x << " y:" << y << "\n";
		set_speed_radial(phi,r,i);
		FILE *fservo = fopen("/home/pi/robot/servopos","w");
                fprintf (fservo, "%d", servo*10);
                fclose(fservo);
		if (i % 10 == 0) {
			std::ifstream infile("/home/pi/robot/direction");
			if (infile != NULL){
				infile >> mode;
				if (mode != "joystick") return 0;
			}
				
		}

	        this_thread::sleep_for(chrono::milliseconds(5));
	}
}
int fileloop() {
	float left, right,max,phi,r;
	std::string mode;
	int i;
	cout << "initializing loop. Using the file /home/pi/robot/direction\n";
	while(true){
		i++;
		std::ifstream infile("/home/pi/robot/direction");
		if (infile == NULL){
			cout << "no file found. Trying joystick";
			joystickloop();
			"Joystick failed. Retrying in 1";
			this_thread::sleep_for(chrono::milliseconds(1000));
		}
		else {
			infile >> mode;
			if (mode == "wheels"){
				infile >> left >> right;
				infile.close();
				set_dir(left,right,i);
			}
			else if (mode == "radial") {
				infile >> phi >> r;
				infile.close();
				set_speed_radial(phi,r,i);
			}
			else if (mode == "joystick") {
				cout << "mode joystick.";
				joystickloop();
				"Joystick failed. Retrying in 1";
				this_thread::sleep_for(chrono::milliseconds(1000));
			}
			else {
				cout << "first line of the file direction must be either \"wheels\" or \"radial\" or \"joystick\"";
			}
		}
		this_thread::sleep_for(chrono::milliseconds(10));
	}
//	camera.grab();
//	cv::Mat img;
//	camera.retrieve( img );
//	cout << clock()-start << "\n" ;
}
int prepare() {
	ofstream gpiexport("/sys/class/gpio/export");
	gpiexport << "9";
	gpiexport.flush();
	gpiexport << "11";
	gpiexport.flush();
	gpiexport << "22";
	gpiexport.flush();
	gpiexport << "2";
	gpiexport.flush();
	gpiexport << "10";
	gpiexport.close();
	this_thread::sleep_for(chrono::milliseconds(900));
	ofstream pindir("/sys/class/gpio/gpio9/direction");
	pindir << "out";
	pindir.close(); pindir.open("/sys/class/gpio/gpio11/direction");
	pindir << "out";
	pindir.close(); pindir.open("/sys/class/gpio/gpio22/direction");
	pindir << "out";
	pindir.close(); pindir.open("/sys/class/gpio/gpio2/direction");
	pindir << "out";
	pindir.close(); pindir.open("/sys/class/gpio/gpio10/direction");
	pindir << "out";
	pindir.close();
	this_thread::sleep_for(chrono::milliseconds(900));
	pin22 = new ofstream("/sys/class/gpio/gpio22/value");
	pin2 = new ofstream("/sys/class/gpio/gpio2/value");
	pin9 = new ofstream("/sys/class/gpio/gpio9/value");
	pin11 = new ofstream("/sys/class/gpio/gpio11/value");
	pin_handbrake = new ofstream("/sys/class/gpio/gpio10/value");
	*pin_handbrake << "1";
        pin_handbrake -> flush();
}

int main(int ARGC, char *argv[]) {
	std::signal(SIGINT, cleanup);
	std::signal(SIGTERM, cleanup);
	cout << std::unitbuf;
	cout << "Starting program\n";
	prepare();
	fileloop();
}
