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
#define OPENING_SIZE 3
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


using namespace cv;
using namespace std;

Point brightLoc, mousePos;

char input;
int cnt = 0, brightness = 0, contourIndex = 0, intense, eleType;
double diffCount, percentDiff, largestArea, area;
double maxVal, minVal;

Mat openElement, closeElement;
Rect bRect;
Scalar intenseAvg, colour;

vector<vector<Point> > contours;
vector<Vec4i> hierarchy;
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
//			0.295463					//  <<<<<<<<< IMPROVE THESE TWO
//			6_fitc.tif				//
//			File Loaded				//
//			0.161916					//  <<<<<<<<< IMPROVE THESE TWO
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

		Mat originalImage = imread(filename);
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

		if (DEBUG) {cout << "Elements Created\n\r";}

		//Creates the image variables used for this project, one is used for each step
		//to facilitate debugging and understanding the code.
		Mat image = originalImage.clone(), threshImage, grayImage, erodedImage, edgeImage, resultImage, openImage, compareImage, goalImage, distImage, closeImage, forImage, backImage, markerImage, markerMask, contourImage, subImage;

		cvtColor(image, image, CV_BGR2GRAY);

		if (DEBUG) {cout << "Variables Created\n\r";}

		intenseAvg = mean(image);

		// grayImage = imread(filename, 0);
		threshold(image, threshImage, intenseAvg[0], 255, THRESH_BINARY);

		erode(threshImage, erodedImage, openElement);
		dilate(erodedImage, openImage, openElement);

		dilate(openImage, erodedImage, closeElement);
		backImage = erodedImage.clone();
		erode(erodedImage, closeImage, closeElement);

		if (DEBUG) {cout << "Opening and Closing Procedure Finished\n\r";}

		distanceTransform(closeImage, distImage, CV_DIST_L2, 5);
		distImage.convertTo(distImage, CV_8U);
		minMaxLoc(distImage, &minVal, &maxVal);
		threshold(distImage, forImage, 0.6*maxVal, 255, THRESH_BINARY);

		if (DEBUG) {cout << "Distance Transform Complete\n\r";}
		
		contourImage = forImage.clone();
		findContours(contourImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		// subtract(backImage, forImage, subImage);

		// disImage((char *)"Subtract Image", subImage, 1);

		// backImage.convertTo(markerImage, CV_32F);

		disImage((char *)"Background Image", backImage, 1);

		intenseAvg = mean(backImage);
		cout << "fuck" << endl;
		backImage.convertTo(markerImage, CV_32SC1);
		disImage((char *)"32S Image", markerImage, 2);

		cout << "you" << endl;

		threshold(markerImage, markerImage, 200, 1, THRESH_BINARY_INV);

		// markerImage.convertTo(markerImage, CV_32SC1);
		// disImage((char *)"32S Image", markerImage, 2);

		disImage((char *)"Thresh Image", markerImage, 3);

		for( int i = 0; i< contours.size(); i++ ) {
			// cnt++;
			// colour = Scalar(cnt, 0, 0);
			drawContours(markerImage, contours, i, Scalar::all(i+2), CV_FILLED, 8, hierarchy, INT_MAX);
		}

		disImage((char *)"Drawn Image", markerImage, 4);

		if (DEBUG) {cout << "Contours Drawn\n\r";}

		watershed(originalImage, markerImage);

		if (DEBUG) {cout << "Watershed Complete\n\r";}

		Mat waterImage(markerImage.size(), CV_8U);

		disImage((char *)"Step 1 Image", waterImage, 5);

		if (DEBUG) {cout << "Image Converted\n\r";}

		goalImage = imread(maskname, 0);

		bitwise_xor(waterImage, goalImage, compareImage);

		disImage((char *)"Step 2 Image", waterImage, 6);


		if (DEBUG) {cout << "Comparison Made\n\r";}

		diffCount = countNonZero(compareImage);
		percentDiff = diffCount/(image.cols * image.rows);

		cout << percentDiff << endl;

		// disImage((char *)"Original Image", originalImage, 1);
		// disImage((char *)"Background Image", backImage, 2);
		// disImage((char *)"Forground Image", forImage, 3);
		// disImage((char *)"Result Image", waterImage, 4);
		// disImage((char *)"Goal Image", goalImage, 5);
		// disImage((char *)"Compare Image", compareImage, 6);

		waitKey(0);
	}
}

void disImage(char* winName, Mat Image, int Position) {
	namedWindow(winName, WINDOW_NORMAL);
	imshow(winName, Image);
	resizeWindow(winName, WINDOWX, WINDOWY);

	switch (Position) {
		case 1:
			moveWindow(winName, 0, 0);
			break;
		case 2:
			moveWindow(winName, WINDOWX+5, 0);
			break;
		case 3:
			moveWindow(winName, (WINDOWX*2)+10, 0);
			break;
		case 4:
			moveWindow(winName, 0, WINDOWY+25);
			break;
		case 5:
			moveWindow(winName, WINDOWX+5, WINDOWY+25);
			break;
		case 6:
			moveWindow(winName, (WINDOWX*2)+10, WINDOWY+25);
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