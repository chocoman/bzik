#include <stdlib.h>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <string.h>
#include <fstream>
using namespace std;

char* symbols = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
int nSymbols = 36;

bool convertToPixelValueArray(cv::Mat &img,int pixelarray[])
{
    if ( img.rows != 16 || img.cols != 16 ){
        return false;
    }
    int i =0;
    for(int x=0;x<16;x++)
    {  
        for(int y=0;y<16;y++)
        {
            pixelarray[i] = (img.at<uchar>(x,y) == 255) ? 1 : 0;
            i++;
        }
    }
    return true;
}


string convertInt( int number )
{
    stringstream ss;
    ss << number;
    return ss.str();
}

 
void readFile( std::string datasetPath, std::string outputfile )
{
    fstream file(outputfile,ios::out);
    
    for(int i=0; i<nSymbols; i++)
    {
        char symbol = symbols[i];
        ifstream nSamplesFile( datasetPath + symbol + "/nSamples" );
        if (!nSamplesFile.is_open()){
            cout << "could not find or open the file "
                 <<  datasetPath + symbol + "/nSamples" << endl;
            return;
        }
        int nSamples;
	nSamplesFile >> nSamples;
        cout << nSamples << " samples for the symbol " << symbol << endl;
        nSamplesFile.close();

        for(int sample = 0; sample<nSamples; sample++)
        {
            std::string imagePath = datasetPath + symbol +
                                    "/img"+convertInt(sample)+".png";
            cv::Mat img = cv::imread(imagePath,0);
 
            int pixelValueArray[256];
 
            if(!convertToPixelValueArray( img, pixelValueArray )){
                cout << "failed to convert " << imagePath << endl;
            }
            for(int d=0;d<256;d++){
                file << pixelValueArray[d] << ",";
            }
            file << symbol << endl;
        }
    }
    file.close();
}
int main()
{
    cout << "Reading the training set..." << endl;
    readFile("/home/martin/Documents/programovani/bzik/training/testset/", 
             "/home/martin/Documents/programovani/bzik/training/testset.txt");
    cout << "operation completed" << endl;
    return 0;
}
