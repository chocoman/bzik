#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include<signal.h>
#include<unistd.h>
#include <raspicam/raspicam_cv.h>
#include <unistd.h>
#include "raspicam/raspicam_cv.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/background_segm.hpp"


using namespace std; 
using namespace cv;

int width;
int height;
int i, j, k, l;

float realphi;
float PHI_STEP =2*3.14/360*5;
Vec3b target;



int timestamp;

string int2string(int i){
        string stringi;
        ostringstream convert;
        convert << i;
        stringi = convert.str();
        return stringi;
}
int log(string message){
	int delay =(clock()-timestamp)*1000/CLOCKS_PER_SEC;
	cout << delay << "ms\t" <<message << "\n";
	timestamp=clock();
}
int square(int a){
	return a*a;
}
int colorDistance(Vec3b a, Vec3b b) {
	return (square(a.val[0]-b.val[0]) + square(a.val[1]-b.val[1]) + square(a.val[2]-b.val[2]));
}


int setSpeedRadial(float phi, float r){
	if (phi>realphi){
		if (phi>realphi+PHI_STEP) realphi += PHI_STEP;
		else realphi = phi;
	}
	else{
		if(phi<realphi-PHI_STEP) realphi -= PHI_STEP;
		else realphi = phi;
	}

	ofstream stream;
	stream.open ("/home/pi/robot/direction");
	stream << "radial\n"<<realphi<<" "<<r<<"\n";
	stream.close();
	return 0;
}
void sig_handler(int signo)
{
	if (signo == SIGINT || signo == SIGSTOP || signo == SIGTERM){
	}
}
int targetpos(Mat image, Point* position){
	int proximity = 10000000; 
	int dist;
	position[0] = Point(-1,-1);//hack
	for (int i=0; i<image.rows;i++){
		for(int j=0; j<image.cols;j++){
			dist = colorDistance(image.at<Vec3b>(i,j),target);
			if (dist<proximity){
				proximity = dist;
				position[0] = Point(j,i);
			}
		}
	}
	return proximity;
}
bool analyze(Mat* images, int nimages) {
        Mat diff(height,width,CV_8UC1);
	Mat suspects(height,width,CV_8UC4);
	Mat mask;
	Mat dist;
	double maximum;
	Point maxLoc;
	BackgroundSubtractorMOG2 pMOG2;
	for (i=0; i<nimages; i++) {
		pMOG2.operator()(images[i],mask);
	}
	imwrite("fg.png",mask);
	imwrite("image.png",images[nimages-1]);
	int erosion_size = 7;
	Mat element = getStructuringElement( MORPH_ELLIPSE,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );
	dilate(mask,mask,element);
	erode(mask,mask,element);
	distanceTransform(mask, dist, CV_DIST_L2, 3);
	imwrite("dist.png",dist);
	minMaxLoc(dist,NULL,&maximum,NULL,&maxLoc);
	log("target size: "+int2string(maximum));
	if (maximum>10){
		target = images[nimages-1].at<Vec3b>(maxLoc);
		return true;
	}
	else return false;
	
}
void my_handler(int s){
	cout << "stop" << endl;
	setSpeedRadial(0,0);
	exit(1);

}
int main ( int argc,char **argv ) {
	   struct sigaction sigIntHandler;

 	  sigIntHandler.sa_handler = my_handler;
	   sigemptyset(&sigIntHandler.sa_mask);
 	  sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);
	
	width = 192;
	height = 108;
		
		Mat* images;
		int nimages = 14;	
		images = new Mat[nimages];
		
		Mat image;
		Point position;
		int proximity;
		float phi,r;
		
		int lostCounter = 0;
		int waitTime = 100;
		
	timestamp = clock();
	raspicam::RaspiCam_Cv Camera;
	Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 );
	Camera.set(CV_CAP_PROP_FRAME_WIDTH, width);
	Camera.set(CV_CAP_PROP_FRAME_HEIGHT, height);
	if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
	log("warming up 3 s");
	usleep(3*1000000);
	while (true){
		log("Finding target");	
		for(i=0;i<nimages;i++){
			Camera.grab();
			Camera.retrieve ( images[i] );
			usleep(0.1*1000000);
		}
		if (!analyze(images, nimages)) continue;
		log("target: "+int2string(target.val[0])+" "+int2string(target.val[1])+" "+int2string(target.val[2]));
		cout<<endl;
		while(true){
			Camera.grab();
			Camera.retrieve ( image );
			proximity = targetpos(image, &position);
			if (proximity>30){
				lostCounter++;
				setSpeedRadial(phi,1-((double)lostCounter)/waitTime);
				if (lostCounter == waitTime) break;
			}
			else {
				lostCounter = 0;
				phi = 3.14*(position.x-width/2)/width;
				cout << "x:\t" << position.x <<" y:\t" << position.y <<" p:\t" << proximity <<" phi:\t"<<phi<<"     \r";
cout<<flush;
				setSpeedRadial(phi,1);
			}
		}
	}
	Camera.release();
	log("camera released");
}
