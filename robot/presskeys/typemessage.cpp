#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <chrono>
#include <ctype.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>

#include "opencv2/core/core.hpp"

using namespace std; 
using namespace std::chrono;
class Program
{
	public:
		float MIN_SAFE_DISTANCE; //mm
		float KEY_WIDTH;
		float CRITICAL_INCL_LOW; //sin(alpha)
		float CRITICAL_INCL_HIGH; //sin(alpha)
		float TURNING_SPEED;
		float TURN_FORCE;
		float ANGLE_PRECISION;
		float DRIVING_SPEED; //mm/s
		float WARM_UP_TIME; //s
		float PRESS_TIME;
		float inclination, aim;
		int distance, dataTime;
		float prevAim, prevDistance;
		int previousTime = 0;
		map<char, cv::Point2f> keyPosition;
		
		Program();
		void loadParameters( );
		void ssleep( double durationSeconds );
		bool alignPosition( );
		bool alignedAngle( char letter );
		bool alignAngle( char letter );
		void forward( double distance );
		bool press( );
		bool update( );
		bool drawStatus( char letter);
		void setSpeedWheels( float left, float right );
		void writeMessage( char *message );
		int timems();
};

Program::Program( void ) {
	FILE *keyPos;
	loadParameters();
	keyPos = fopen("keyboard2.csv","r");
	float x,y;
	char c;
	while( fscanf(keyPos, "%f,%f,%c\n", &y, &x, &c )!= EOF ){
		keyPosition[c] = cv::Point2f(x,y);
	}
	fclose(keyPos);
}

void Program::loadParameters( void ){
	FILE *parametersFile;
	char dummy[100];
	parametersFile = fopen("typemessage.par","r");
	fscanf( parametersFile, "%f", &MIN_SAFE_DISTANCE );
	fgets(dummy, 100, parametersFile);
	fscanf( parametersFile, "%f", &KEY_WIDTH );
	fgets(dummy, 100, parametersFile);
	fscanf( parametersFile, "%f", &CRITICAL_INCL_LOW );
	fgets(dummy, 100, parametersFile);
	fscanf( parametersFile, "%f", &CRITICAL_INCL_HIGH );
	fgets(dummy, 100, parametersFile);
	fscanf( parametersFile, "%f", &TURNING_SPEED );
	fgets(dummy, 100, parametersFile);
	cout << TURNING_SPEED <<endl;
	fscanf( parametersFile, "%f", &TURN_FORCE );
	fgets(dummy, 100, parametersFile);
	fscanf( parametersFile, "%f", &ANGLE_PRECISION );
	fgets(dummy, 100, parametersFile);
	fscanf( parametersFile, "%f", &DRIVING_SPEED );
	fgets(dummy, 100, parametersFile);
	fscanf( parametersFile, "%f", &WARM_UP_TIME );
	fgets(dummy, 100, parametersFile);
	fscanf( parametersFile, "%f", &PRESS_TIME );
	fgets(dummy, 100, parametersFile);
	cout << PRESS_TIME <<endl;
	fclose( parametersFile );
}

int Program::timems( void ){
	milliseconds ms =  duration_cast< milliseconds >(
	                    system_clock::now().time_since_epoch() );
	return ms.count();
}

void Program::ssleep(double durationSeconds){
	usleep(1000000*durationSeconds);
}

bool Program::alignPosition( ){
	cout << "aligning position" << endl;
	//TODO
	ssleep( 0.5 );
	return true;
}

bool Program::alignedAngle( char letter ){
	if (!keyPosition.count(letter)){
		cout << "unknown character: " << letter << endl;
		return false;
	}
	double desired = keyPosition[letter].x;
	double difference = desired - aim;
	if ( abs(difference) < ANGLE_PRECISION ){
		return true;
	} else {
		return false;
	}
}

bool Program::alignAngle( char letter ){
	if (!keyPosition.count(letter)){
		cout << "unknown character: " << letter << endl;
		return false;
	}
	double desired = keyPosition[letter].x;
	double difference = desired - aim;
	cout << "aiming: desired angle " << desired << endl;
	cout << "        actual angle  " << aim << endl;
	double turningSpeedKeys = TURNING_SPEED * distance / KEY_WIDTH;
	double duration = abs(difference) / turningSpeedKeys;
	double endTime = timems() + 
	                 1000 *(WARM_UP_TIME + duration);
	if (difference < 0) {
		//turn left
		setSpeedWheels(-TURN_FORCE, TURN_FORCE);
	} else {
		//turn right
		setSpeedWheels( TURN_FORCE, -TURN_FORCE);
	}
	while ( timems() < endTime ){
		ssleep( 0.05 );
		if (update()) break;
	}
	setSpeedWheels(0,0);
	return true;
}

void Program::forward( double drivingDistance ){
	double duration = abs(drivingDistance) / DRIVING_SPEED;
	int endTime = timems() + 1000 * (duration + WARM_UP_TIME);
	if (drivingDistance > 0) {
		//drive forward
		setSpeedWheels(1, 1);
	} else {
		//drive backwards
		setSpeedWheels(-1,-1);
	}
	while ( timems() < endTime ){
		ssleep( 0.05 );
		if (update()) break;
	}
	setSpeedWheels(0,0);
}

bool Program::press( ){
	cout << "pressing" << endl;
	setSpeedWheels(1,1);
	ssleep( WARM_UP_TIME + PRESS_TIME );
	setSpeedWheels(-1,-1);
	ssleep( WARM_UP_TIME + PRESS_TIME );
	setSpeedWheels(0,0);
	return true;
}

bool Program::drawStatus( char letter){
	
	return true;
}

bool Program::update( ){
	int newDistance;
	float newAim;
	prevAim = aim;
	prevDistance = distance;
	FILE *positionFile;
	positionFile = fopen("position","r");
	fscanf(positionFile,"%f\n",&inclination);
	fscanf(positionFile,"%f\n",&newAim);
	fscanf(positionFile,"%d\n",&newDistance);
	fscanf(positionFile,"%d\n",&dataTime);
	if ( dataTime == previousTime ){
		return false;
	} else {
		previousTime = dataTime;
		prevAim = aim;
		prevDistance = distance;
		aim = newAim;
		distance = newDistance;
		return true;
	}
}

void Program::setSpeedWheels(float left, float right ){
	ofstream stream;
        stream.open ("/home/pi/robot/direction");
        stream << "wheels\n" << left << " " << right <<"\n";
        stream.close();
}

void Program::writeMessage( char *message ){
	while (message[0] != '\0' ){
		char activeLetter = toupper(message[0]);
		if (!keyPosition.count(activeLetter)){
			//omit unknown character
			message = message + 1;
			continue;
		}
		
		while(!update()){
			ssleep(0.05);
		}
		cout << "\na \t" << aim << "\t d \t" << distance << endl;
		cout << "letter " << activeLetter << endl;
		if (inclination < CRITICAL_INCL_LOW  ||
		    inclination > CRITICAL_INCL_HIGH ){
			
			if ( distance < MIN_SAFE_DISTANCE ) {
				// go back
				forward( distance-MIN_SAFE_DISTANCE );
				continue;
			}
			alignPosition( );
			continue;
		}
		
		
		if (!alignedAngle( activeLetter )){
			if ( distance < MIN_SAFE_DISTANCE ) {
				// go back
				forward( distance-MIN_SAFE_DISTANCE );
			}
			alignAngle ( activeLetter );
			continue;
		}

		if ( distance > MIN_SAFE_DISTANCE ) {
			cout << "forward: desired distance " << MIN_SAFE_DISTANCE << endl;
			cout << "         actual distance  " << distance << endl;
			forward( distance-MIN_SAFE_DISTANCE );
			continue;
		}
		
		if (press( )){
			message = message + 1;//forget first letter
		}
	}
}


void stopIntHandler(int s){
        cout << "stop" << endl;
	ofstream stream;
        stream.open ("/home/pi/robot/direction");
        stream << "wheels" << endl << "0 0" << endl;
        stream.close();
        exit(1);
}


int main ( int argc,char **argv ) {
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = stopIntHandler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
        sigaction(SIGINT, &sigIntHandler, NULL);
	
	Program program;	
	char *message;
	if (argc >= 2){
		message = argv[1];
	} else {
		cout << "provide a word to be written" <<endl;
		return 42;
	}
	
	program.writeMessage(message);
}
