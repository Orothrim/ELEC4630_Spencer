#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
#include <stdio.h>
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions

#define DEBUG 1

#define OPEN_CLOSE 1 //1 uses and opening procedure, 0 uses a closing procedure.
#define YUV_RGB 0 //1 = YUV, 0 = RGB.

//Canny edge detection threshold, it is quite low due to the low 
//brightness of the images.
#define THRESH 60
#define AREA_THRESH 100

#define ELEMENT_TYPE 0
#define OPENING_SIZE 5
#define CLOSING_SIZE 3

//Used to access all of the images.
#define ONESCOLUMN 6
#define TENSCOLUMN 5
#define NUMIMAGES 16

//Middle point of images.
#define MIDDLEX 292
#define MIDDLEY 360

//Colours used in image analysis.
#define WHITE 255
#define MANICOLOUR 127

//Size of the images when displayed.
#define WINDOWX 533
#define WINDOWY 400
//First of four image positions.
#define FIRSTPOSX 0
#define FIRSTPOSY 0
//Second of four image positions.
#define SECONDPOSX WINDOWX+5
#define SECONDPOSY 0
//Third of four image positions.
#define THIRDPOSX 0
#define THIRDPOSY WINDOWY+25
//Fourth of four image positions.
#define FOURTHPOSX WINDOWX+5
#define FOURTHPOSY WINDOWY+25


using namespace cv;
using namespace std;

Point brightLoc, mousePos;

char input;
int cnt = 0, brightness = 0, contourIndex = 0, intense, eleType;
double diffCount, percentDiff;

double largestArea, area;

Mat openElement, closeElement;
Rect bRect;
Scalar intenseAvg, color[3];

vector<vector<Point> > contours;
vector<Vec4i> hierarchy;
vector<Vec3f> circles;
uchar* p;
uchar* q;
//uchar* eleType;

//Function Prototypes
void onMouse(int event, int x, int y, int flags, void* userdata);
void disImage(char* winName, Mat Image, int Position);
int write(char message[10]);

////////////////////////////
// FUNCTION SUCCESS RATE	//
//			1_fitc.tif				//
//			File Loaded				//
//			0.0146558					//
//			2_fitc.tif				//
//			File Loaded				//
//			0.0339746					//
//			3_fitc.tif				//
//			File Loaded				//
//			0.0389354					//
//			4_fitc.tif				//
//			File Loaded				//
//			0.0204008					//
//			5_fitc.tif				//
//			File Loaded				//
//			0.295463					//
//			6_fitc.tif				//
//			File Loaded				//
//			0.161916					//
//												//
////////////////////////////

int main(int argc, char** argv) {

	for(int k = 1; k <= 6; k++) {

		string filename = "1_fitc.tif";
		string maskname = "1_mask.tif";
		stringstream convert;

		convert << k;
		filename.replace(0,1,convert.str());
		maskname.replace(0,1,convert.str());
		
		cout << filename << endl;

		Mat originalImage = imread(filename, 0);
		if(originalImage.empty()) {
			//Checks that the file can be opened, if it can't, prings "can not open" 
			//and end the program
			cout << "can not open " << filename << endl;
			return -1;
		}
		if (DEBUG) {cout << "File Loaded\n\r";}

	  if( ELEMENT_TYPE == 0 ){ eleType = MORPH_RECT; }
	  else if( ELEMENT_TYPE == 1 ){ eleType = MORPH_CROSS; }
	  else if( ELEMENT_TYPE == 2) { eleType = MORPH_ELLIPSE; }

		openElement = getStructuringElement(eleType , Size( 2*OPENING_SIZE + 1, 2*OPENING_SIZE+1 ), Point(OPENING_SIZE, OPENING_SIZE));
		closeElement = getStructuringElement(eleType , Size( 2*CLOSING_SIZE + 1, 2*CLOSING_SIZE+1 ), Point(CLOSING_SIZE, CLOSING_SIZE));

		//Creates the image variables used for this project, one is used for each step
		//to facilitate debugging and understanding the code.
		Mat image = originalImage.clone(), threshImage, reducedImage, grayImage, erodedImage, edgeImage, contoursImage, blueImage, greenImage, redImage, resultImage, openImage, compareImage, goalImage;

		intenseAvg = mean(image);

		// grayImage = imread(filename, 0);
		threshold(image, threshImage, intenseAvg[0]*1.2, 255, THRESH_BINARY);

		erode(threshImage, erodedImage, openElement);
		dilate(erodedImage, resultImage, openElement);

		// dilate(openImage, erodedImage, closeElement);
		// erode(erodedImage, resultImage, closeElement);

		goalImage = imread(maskname, 0);

		bitwise_xor(resultImage, goalImage, compareImage);

		diffCount = countNonZero(compareImage);
		percentDiff = diffCount/1443520;

		cout << percentDiff << endl;

		disImage((char *)"Original Image", originalImage, 1);
		disImage((char *)"Result Image", resultImage, 2);
		disImage((char *)"Goal Image", goalImage, 3);
		disImage((char *)"Compare Image", compareImage, 4);

		waitKey(0);
	}
}

void disImage(char* winName, Mat Image, int Position) {
	namedWindow(winName, WINDOW_NORMAL);
	imshow(winName, Image);
	resizeWindow(winName, WINDOWX, WINDOWY);
	switch (Position) {
		case 1:
			moveWindow(winName, FIRSTPOSX, FIRSTPOSY);
			break;
		case 2:
			moveWindow(winName, SECONDPOSX, SECONDPOSY);
			break;
		case 3:
			moveWindow(winName, THIRDPOSX, THIRDPOSY);
			break;
		case 4:
			moveWindow(winName, FOURTHPOSX, FOURTHPOSY);
			break;
	}
	
}

void onMouse(int event, int x, int y, int flags, void* userdata) {
	//Event that is attached to a mouse click after "setMouseCallback" occurs.
	if(event == EVENT_LBUTTONDOWN) {
		mousePos.x = x;
		mousePos.y = y;
	}
}

// int write(char message[10]) {
// 	int n_written = 0, spot = 0;

// 	do {
// 		n_written = write( USB, &message[spot], 1 );
// 		spot += n_written;
// 	} while (message[spot-1] != '\r' && n_written > 0);
// }