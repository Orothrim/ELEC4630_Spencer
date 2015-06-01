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

#define ELEMENT_TYPE 0
#define OPENING_SIZE 5
#define CLOSING_SIZE 3

//Used to access all of the images.
#define ONESCOLUMN 6
#define TENSCOLUMN 5
#define NUMIMAGES 16

//Colours used in image analysis.
#define WHITE 255
#define MANICOLOUR 127

//Size of the images when displayed.
#define WINDOWX 533
#define WINDOWY 400

using namespace cv;
using namespace std;

int eleType;
double diffCount, percentDiff;

Mat openElement, closeElement;
Scalar intenseAvg;

//Function Prototypes
void disImage(char* winName, Mat Image, int Position);

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
		// if (DEBUG) {cout << "File Loaded\n\r";}

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
		// threshold(image, threshImage, 0, 255, THRESH_BINARY | THRESH_OTSU);

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
