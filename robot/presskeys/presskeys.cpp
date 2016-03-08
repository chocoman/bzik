#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <ctype.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <raspicam/raspicam_cv.h>
#include <unistd.h>
#include "raspicam/raspicam_cv.h"
#include "opencv2/core/core.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/background_segm.hpp"


using namespace std; 
using namespace cv;

int i, j, k, l;

int id;

float realphi = 0;
float PHI_STEP =2*3.14/360*5;

int timestamp;

int MAX_SIZE;
int MIN_SIZE;
int THRESHOLD_LIGHT;
int MIN_DISTANCE;
int MAX_DISTANCE;
int MAX_DIFFERENCE;
int MAX_DIFF_VERTI;
int WIDTH;
int HEIGHT;

int SYMBOL_SIZE=30;

map<char,Point2f> expectedPos;
char symbols[] = "ZXCVBNMASDFGHJKLQWERTYUIOP1234567890";
int symbolsSize = 36;

string int2string(int i){
        string stringi;
        ostringstream convert;
        convert << i;
        stringi = convert.str();
        return stringi;
}
string char2string(char c){
        string stringc;
        ostringstream convert;
        convert << c;
        stringc = convert.str();
        return stringc;
}

int log(string message){
	int delay =(clock()-timestamp)*1000/CLOCKS_PER_SEC;
	cout << delay << "ms\t" <<message << "\n";
	timestamp=clock();
}
int square(int a){
	return a*a;
}

int d2( Point a, Point b) {
	return (square(a.x-b.x) + square(a.y-b.y));
}

float d2( Point2f a, Point2f b) {
	return (square(a.x-b.x) + square(a.y-b.y));
}

bool horizontal(Point v){
	return (abs(v.x) > abs(v.y)*3);
}

bool vertical(Point v){
	return (abs(v.y) > abs(v.x)) && v.x*v.y > 0;
}
Point relative( Point a, Point b) {
	Point p = Point(a.x-b.x,a.y-b.y);
	if(horizontal(p)){
		if (a.x>=b.x) return Point(a.x-b.x,a.y-b.y);
		else return Point(b.x-a.x,b.y-a.y);
	} else {
		if (a.y>=b.y) return Point(a.x-b.x,a.y-b.y);
		else return Point(b.x-a.x,b.y-b.y);
	}
}

Point times(int k, Point a) {
	return Point(k*a.x,k*a.y);
}

Point divide(Point a, int k) {
	return Point(a.x/k, a.y/k);
}

Point average(Point a, Point b) {
	return Point((a.x+b.x)/2,(a.y+b.y)/2);
}

bool contains(vector<int> v, int x){
	for (int i = 0; i<v.size(); i++){
		if (x==v[i]) return true;
	}
	return false;
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

void my_handler(int s){
	cout << "stop" << endl;
	setSpeedRadial(0,0);
	exit(1);

}

Mat core( int erosion_size ){
	Mat element = getStructuringElement( MORPH_ELLIPSE,
		Size( 2*erosion_size + 1, 2*erosion_size+1 ),
		Point( erosion_size, erosion_size ) );
	return element;
}

vector<Point> findCenters( Mat image ){
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours( image, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
	bool smallEnough;
	vector<Point> accepted;
	Mat centers;
	centers = Mat::zeros( HEIGHT, WIDTH, CV_8UC1 );
	for(int i = 0; i < contours.size(); i++){
		smallEnough = true;
		for(int j = 0; j < contours[i].size(); j++){
			if (d2(contours[i][j],contours[i][0]) > square(2*MAX_SIZE)){
				smallEnough = false;
				break;
			}
		}
		if (!smallEnough) continue;
		Point center;
		int diameterSquared = 0;
		int distanceSquared;
		for(int j = 0; j < contours[i].size(); j++){
			for(int k = j+1; k < contours[i].size(); k++){
				distanceSquared = d2(contours[i][j], contours[i][k]);
				if (distanceSquared > diameterSquared){
					diameterSquared = distanceSquared;
					center.x = (contours[i][j].x + contours[i][k].x) / 2;
					center.y = (contours[i][j].y + contours[i][k].y) / 2;
				}
			}
		}
		
		int diameter = (int)sqrt(diameterSquared);
		if (diameter > MAX_SIZE || diameter < MIN_SIZE) continue;
		accepted.push_back(center);
		circle( centers, center, 2, Scalar( 255 ), -1, 8);
	}
	imwrite("centers.png", centers);
	log("centers drawn to centers.png");
	return accepted;
}


void DFSstep(int* visited, vector<Point> V, int v, Point direction){
	visited[v] = 1;
	for (int jumpLength = 1; jumpLength <= 3; jumpLength++){
	for (int i = 0; i < V.size(); i++){
		if (i==v) continue;
		Point newDirection = relative(V[v],V[i]);
		bool match = false;
		if (d2(newDirection, times(jumpLength,direction)) < square(jumpLength*MAX_DIFFERENCE)){
			match = true;
			newDirection = divide(newDirection,jumpLength);
		}
		if (!visited[i] && match){
			DFSstep(visited, V, i, newDirection);
		}
	}
	}
}

vector<int> DFS(vector<vector<int> > S, vector<Point> V){
	int visited[V.size()] = {};
	for (int i = 0; i< V.size(); i++){
		visited[i] = 0;
	}
	for (int i = 0; i < S.size(); i++){
		for (int j = 0; j < 2; j++){
			int v = S[i][j];
			DFSstep(visited, V, v, relative(V[S[i][0]],V[S[i][1]]));
		}
	}
	vector<int> ret;
	
	for (int i = 0; i < V.size(); i++){
		if(visited[i]){
			ret.push_back(i);
		}
	}
	return ret;
}

Point convertToElementar(Point a, Point b, Point2f c){
	//returns a*c.x+b*c.y
	return Point((int)(a.x*c.x+b.x*c.y),
	             (int)(a.y*c.x+b.y*c.y));
}

Point2f coordinates(Point a, Point b, Point c){
	//returns (k, m) such that ka+mb = c
	double ax = (double)a.x;
	double ay = (double)a.y;
	double bx = (double)b.x;
	double by = (double)b.y;
	double cx = (double)c.x;
	double cy = (double)c.y;
	double m = (cy-(cx*ay/ax))/(by-ay*bx/ax);
	double k = (cx-bx*m)/ax;
	cout <<"base: " << a << b <<"c: "<< c <<"(k,m)"<< Point2f(k,m)<<endl;
	return Point2f(k,m);
}
vector<Point> isolateKeys(vector<Point> centers, Point *vert, Point *hori){
	vector<vector<int> > hneighbors(centers.size(),vector<int>());
	vector<vector<int> > vneighbors(centers.size(),vector<int>());
	for (int i = 0; i < centers.size(); i++){
		for (int j = i+1; j < centers.size(); j++){
			Point v = relative(centers[i],centers[j]);
			if(horizontal(v)){
				if (d2(centers[i], centers[j]) > square(MAX_DISTANCE)) continue;
				hneighbors[i].push_back(j);
				hneighbors[j].push_back(i);
			}
			if(vertical(v)){
				if (d2(centers[i], centers[j]) > square(MAX_DISTANCE)) continue;
				vneighbors[i].push_back(j);
				vneighbors[j].push_back(i);
				
			}
		}
	}
	Point u,v;
	Point horizontalVector(0,0);
	int diff2;
	double score, bestScore, similar;
	bestScore = 0;
	for (int i = 0; i < centers.size(); i++){
		for (int j = 0; j < hneighbors[i].size(); j++){
			v = relative(centers[i],centers[hneighbors[i][j]]);
			similar = 0;
			for (int k = 0; k < centers.size(); k++){
				for (int l = 0; l < hneighbors[k].size(); l++){
					u = relative(centers[k],centers[hneighbors[k][l]]);
					diff2 = d2(v,u);
					similar+=exp(-diff2/MAX_DIFFERENCE);
				}
			}
			score = similar*similar/v.x; //heuristic
			if(score > bestScore){
				bestScore = score;
				horizontalVector = v;
			}
		}
	}
	*hori = horizontalVector;
	
	
	Mat connections;
	connections = Mat::zeros( HEIGHT, WIDTH, CV_8UC1 );
	vector<vector<int> > startingEdges;

	for (int k = 0; k < centers.size(); k++){
		for (int l = 0; l < hneighbors[k].size(); l++){
			u = relative(centers[k],centers[hneighbors[k][l]]);
			if (d2(horizontalVector,u) <= square(MAX_DIFFERENCE)){
				vector<int> edge;
				edge.push_back(k);
				edge.push_back(hneighbors[k][l]);
				startingEdges.push_back(edge);
				line(connections, centers[k], centers[hneighbors[k][l]], Scalar(255), 1, 8, 0);
			}
		}
	}
	vector<int> relevantI = DFS(startingEdges, centers);
	vector<Point> relevantP;
	for(int i = 0; i < relevantI.size(); i++){
		relevantP.push_back(centers[relevantI[i]]);
	}
	bool escape=true;
	while(escape){
		escape = false;
		for(int i = 0; i < relevantP.size() && !escape; i++){
			for(int j = i+1; j < relevantP.size() && !escape; j++){
				if(d2(relevantP[i], relevantP[j])<square(MIN_DISTANCE)){
					relevantP.push_back(average(relevantP[i], relevantP[j]));
					relevantP.erase(relevantP.begin()+j); //j>i
					relevantP.erase(relevantP.begin()+i);
					escape = true;
				}
			}
		}
	}

	
	Point verticalVector(0,0); //represents the vector between the keys Q and A
	diff2 = 0;
	similar = 0;
	score = 0;
	bestScore = 0;
	Point h = horizontalVector; //alias
	Point ub,ut;
	double bonus1, bonus2, bonus3;
	for (int i = 0; i < centers.size(); i++){
		if (!contains(relevantI,i)) continue;
		for (int j = 0; j < vneighbors[i].size(); j++){
			if (!contains(relevantI,i)) continue;
			line(connections, centers[i], centers[vneighbors[i][j]], Scalar(180), 1, 8, 0);
			v = relative(centers[i],centers[vneighbors[i][j]]);
			if(!vertical(v)) continue;
			similar = 0;
			for (int k = 0; k < centers.size(); k++){
				for (int l = 0; l < vneighbors[k].size(); l++){
					//FIXME nevoli spravny vektor.
					//vyhazet zbytecne dlouhe: z kazdeho vrcholu berme nejkratsi nahoru a nejkratsi dolu
					u = relative(centers[k],centers[vneighbors[k][l]]);
					    //u: if u is between Q and A.
					ub = Point(u.x-h.x*4/10,u.y-h.y*4/10);
					     //ub: if u is between A and Z
					ut = Point(u.x*3/2-h.x/13,u.y*3/2-h.x/13);
					     //ut: if u is between 1 and Q
					bonus1 = exp(-d2(v,u )/(double)square(MAX_DIFF_VERTI));
					bonus2 = exp(-d2(v,ub)/(double)square(MAX_DIFF_VERTI));
					bonus3 = exp(-d2(v,ut)/(double)square(MAX_DIFF_VERTI));
					if( abs(v.x - 15)<=2 && abs(v.y-45) <= 2){
						cout << u << ub << ut << endl;
						cout << bonus1 << "\t" << bonus2 << "\t" << bonus3 << "\t"<< endl;
					}
					similar += max(bonus1, max(bonus2,bonus3));
				}
			}
			cout << endl << "vector:" << v << similar <<endl;
			score = square(similar)*10000/v.y; //heuristic
			if(score > bestScore){
				bestScore = score;
				verticalVector = v;
			}
		}
	}
	*vert = verticalVector;

	line(connections, Point(20,20), Point(20+vert->x,20+vert->y),
             Scalar(255), 1, 8, 0);
	line(connections, Point(20,20), Point(20+hori->x,20+hori->y),
             Scalar(255), 1, 8, 0);

	for(int i = 0; i < relevantP.size(); i++){
		circle( connections, relevantP[i], 3, Scalar( 255 ), -1, 8);
	}
	imwrite("connections.png", connections);
	log("connections drawn");
	
	return relevantP;
}

Mat cropTop(Mat im){
	bool blackRow = true;
	int i;
	for(i = 0; i<im.rows; i++){
		for(int j = 0; j<im.cols; j++){
			if(im.at<uchar>(i,j)>THRESHOLD_LIGHT){
				blackRow = false;
			}
		}
		if(!blackRow) break;
	}
	if(i==im.rows) return im;
	return im(Rect(0, i, im.cols, im.rows-i));
}
Mat autoCrop(Mat im){
	im = cropTop(im);
	flip(im,im,0);//x-axis
	im = cropTop(im);
	transpose(im,im);
	im = cropTop(im);
	flip(im,im,0);//x-axis
	im = cropTop(im);
	transpose(im,im);
	flip(im,im,-1);//x and y axis
	
	return im;
}
Mat symbol(Mat image, Point center){
	Mat symbol;
	Mat rectangle = image(Rect(center.x-SYMBOL_SIZE/2,center.y-SYMBOL_SIZE/2,SYMBOL_SIZE,SYMBOL_SIZE));
	rectangle = autoCrop(rectangle);
	resize(rectangle, symbol, Size(16,16), 0, 0, INTER_LINEAR);
	threshold(symbol, symbol, THRESHOLD_LIGHT, 255, 0);
	return symbol;
}

char recognize(Mat s, double *confidence){
	for (int y = 0; y<16; y++){
		for (int x = 0; x<16; x++){
			if(s.at<uchar>(y,x)){
				cout<<"8";
			}
			else cout<<" ";
		}
		cout<<endl;
	}
	char character;
	cout << "type the symbol or \".\": ";
	string str;
	cin >> str;
	character = toupper(str[0]);
	bool valid = false;
	
	for (int i = 0; i < symbolsSize; i++){
		if (symbols[i] == character){
			valid = true;
		}
	}
	if (!valid){
		cout << "unknown character" <<endl;
		*confidence = 0;
		return character;
	}
	*confidence = 1;
	return character;

}

Point2f selectOrigin(vector<Point2f> origins){
	double score;
	double best = 0;
	double d;
	int besti;
	for (int i = 0; i<origins.size(); i++){
		score = 0;
		for (int j = 0; j<origins.size();j++){
			d = d2(origins[i],
			                         origins[j]);
			score += 1/(1+3*d);
		}
		if (score > best){
			best = score;
			besti = i;
		}
	}
	return origins[besti];
}

bool analyze( Mat image ) {
	id = time(NULL);
	Mat grayscaleBig( image.rows, image.cols, CV_8UC1 );
	Mat grayscale( WIDTH, HEIGHT, CV_8UC1 );
	int from_to[] = { 2, 0 };
	mixChannels( &image, 1, &grayscaleBig, 1, from_to, 1 );//drop B and G
	

	
	resize(grayscaleBig, grayscale, Size(WIDTH,HEIGHT),0,0);
	imwrite("image.png", grayscale);
	Mat light, lightd;
	threshold( grayscale, light, THRESHOLD_LIGHT, 255, 0 ); //1: binary inverted threshold
	imwrite("light.png", light);
	dilate(light, lightd, core(2));
	erode(lightd, lightd, core(2));
	imwrite("points.png", light);
	log("image.png, light.png and points.png drawn");
	vector<Point> centers = findCenters(light);
	Point vert, hori;
	vector<Point> keys = isolateKeys(centers, &vert, &hori);
	log("keys isolated");
	
	if (!keys.size()) return false;
	vector<Point2f> origins;

	/* temporary
	ofstream kbfile;
	kbfile.open("keyboard.csv", ios_base::app);
	*/
	for(int i = 0; i < keys.size(); i++){
		Point2f coords = coordinates(vert, hori, keys[i]);
		cout << coords << endl;
		//kbfile << coords <<",";
		int x = keys[i].x*1920/WIDTH;
		int y = keys[i].y*1080/HEIGHT;
		if (x<SYMBOL_SIZE/2 || y<SYMBOL_SIZE/2 ||
	    	    x>=image.cols-SYMBOL_SIZE/2 || y>=image.rows-SYMBOL_SIZE/2){
			continue;
		}
		Mat s = symbol(grayscaleBig, Point(x,y));
		double confidence;
		char c = recognize(s, &confidence);
		if (confidence>=0.8){
			origins.push_back(Point2f(coords.x-expectedPos[c].x,
			                          coords.y-expectedPos[c].y));
			imwrite("images/"+char2string(c)+
			        "/"+int2string(id)+".png", s);
		}
		
		//kbfile << character << endl;
		cout << endl;
	}
	if (!origins.size()) return false;
	Point2f origin = selectOrigin(origins);
	
	for(int i = 0; i < keys.size(); i++){
		Point2f coords = coordinates(vert, hori, keys[i]);
		string closest;
		double d;
		double minDistance=1000000000;
		char s;
		for (int j = 0; j < symbolsSize; j++){
			s = symbols[j];
			d = d2(expectedPos[s],
			              Point2f(coords.x-origin.x,
			                      coords.y-origin.y));
			if (d<minDistance){
				minDistance = d;
				closest = s;
			}
		}
		putText(grayscale, closest, keys[i],
		        FONT_HERSHEY_SIMPLEX, 1,
			Scalar(255),1,8,false);
		cout << " relative: "
		     << Point2f(coords.x-origin.x,
		                coords.y-origin.y)
		     << endl;
		cout << closest << endl;
	}
	



	//TEMPORARY
	for(int i = 0; i < symbolsSize; i++){
		char symbol = symbols[i];
		Point2f e = expectedPos[symbol];
		Point2f f = Point2f(e.x+origin.x,e.y+origin.y);
		Point r = convertToElementar(
		       vert, hori, f);
		putText(grayscale, char2string(symbol), r,
		        FONT_HERSHEY_SIMPLEX, 0.5,
			Scalar(255),1,8,false);
		
	}

	imwrite("marked.png", grayscale);

	//kbfile.close();
	return true;
}

int main ( int argc,char **argv ) {
	if (argc == 10){
		MAX_SIZE = std::stoi(argv[1]);
		MIN_SIZE = std::stoi(argv[2]);
		THRESHOLD_LIGHT = std::stoi(argv[3]);
		MIN_DISTANCE = std::stoi(argv[4]);
		MAX_DISTANCE = std::stoi(argv[5]);
		MAX_DIFFERENCE = std::stoi(argv[6]);
		MAX_DIFF_VERTI = std::stoi(argv[7]);
		WIDTH = std::stoi(argv[8]);
		HEIGHT = std::stoi(argv[9]);
		cout << "MAX_SIZE = " << std::stoi(argv[1])<<endl;
		cout << "MIN_SIZE = " << std::stoi(argv[2])<<endl;
		cout << "THRESHOLD_LIGHT = " << std::stoi(argv[3])<<endl;
		cout << "MIN_DISTANCE = " << std::stoi(argv[4])<<endl;
		cout << "MAX_DISTANCE = " << std::stoi(argv[5])<<endl;
		cout << "MAX_DIFFERENCE = " << std::stoi(argv[6])<<endl;
		cout << "MAX_DIFF_VERTI" << MAX_DIFF_VERTI <<endl;
		cout << "WIDTH" << WIDTH <<endl;
		cout << "HEIGHT" << HEIGHT <<endl;
	} else {
		MAX_SIZE = 40;
		MIN_SIZE = 5;
		THRESHOLD_LIGHT = 70;
		MIN_DISTANCE = 10;
		MAX_DISTANCE = 85;
		MAX_DIFFERENCE = 2;
		MAX_DIFF_VERTI = 5;
		WIDTH = 960;
		HEIGHT = 540;
	}
	
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);

	srand(time(NULL));
	
	FILE *keyPos;
	keyPos = fopen("keyboard1.csv","r");
	float x,y;
	char c;
	while( fscanf(keyPos,"%f,%f,%c\n", &y, &x, &c )!= EOF){
		expectedPos[c]=Point2f(y,x);
	}
	fclose(keyPos);
	cout << "TEST1" << expectedPos['C']<<endl;
	Mat image;
	
	timestamp = clock();
	raspicam::RaspiCam_Cv Camera;
	Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 );
	Camera.set( CV_CAP_PROP_FRAME_WIDTH, 1920 );
	Camera.set( CV_CAP_PROP_FRAME_HEIGHT, 1080 );
	if (!Camera.open()) {
		cerr<<"Error opening the camera"<<endl;
		return -1;
	}
	log("warming up");
	Camera.grab();
	usleep(0.5*1000000);
	while (true){
		Camera.grab();
		Camera.retrieve ( image );
		analyze( image );
		setSpeedRadial(0,0);
		log("going to sleep");
		usleep(0.5*1000000);
		log("woke up");
	}
	Camera.release();
	log("camera released");
}
