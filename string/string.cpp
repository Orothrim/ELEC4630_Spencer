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

#define ELEMENT_TYPE 2
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
int cnt = 0, brightness = 0, contourIndex = 0, intense, diffCount;

double largestArea, area;

Mat openElement, closeElement;
Rect bRect;
Scalar intenseAvg, color[3];

vector<vector<Point> > contours;
vector<Vec4i> hierarchy;
vector<Vec3f> circles;
uchar* p;
uchar* q;

int USB = open( "/dev/rfcomm0", O_RDWR| O_NOCTTY );

//Function Prototypes

void onMouse(int event, int x, int y, int flags, void* userdata);
void disImage(char* winName, Mat Image, int Position);
int write(char message[10]);
Mat opening()

int main(int argc, char** argv) {

	string filename = "String1_1";
	stringstream select;

	select << argc;
	filename.replace(6,1,select.str());

	for(int k = 1; k <= 5; k++) {


		stringstream convert;

		convert << k;
		filename.replace(0,1,convert.str());
		
		cout << filename << endl;

		Mat originalImage = imread(filename, 0);
		if(originalImage.empty()) {
			//Checks that the file can be opened, if it can't, prings "can not open" 
			//and end the program
			cout << "can not open " << filename << endl;
			return -1;
		}
		if (DEBUG) {cout << "File Loaded\n\r";}

		openElement = getStructuringElement(MORPH_ELLIPSE , Size( 2*OPENING_SIZE + 1, 2*OPENING_SIZE+1 ), Point(OPENING_SIZE, OPENING_SIZE));
		closeElement = getStructuringElement(MORPH_ELLIPSE , Size( 2*CLOSING_SIZE + 1, 2*CLOSING_SIZE+1 ), Point(CLOSING_SIZE, CLOSING_SIZE));

		//Creates the image variables used for this project, one is used for each step
		//to facilitate debugging and understanding the code.
		Mat image = originalImage.clone(), threshImage, reducedImage, grayImage, erodedImage, edgeImage, contoursImage, blueImage, greenImage, redImage, resultImage, openImage, compareImage, goalImage;

		intenseAvg = mean(image);

		// grayImage = imread(filename, 0);
		threshold(image, threshImage, intenseAvg[0], 255, THRESH_BINARY);

		erode(threshImage, erodedImage, openElement);
		dilate(erodedImage, resultImage, openElement);

		// dilate(openImage, erodedImage, closeElement);
		// erode(erodedImage, resultImage, closeElement);

		goalImage = imread(maskname, 0);

		bitwise_xor(resultImage, goalImage, compareImage);

		diffCount = countNonZero(compareImage);

		cout << diffCount << endl;

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

int write(char message[10]) {
	int n_written = 0, spot = 0;

	do {
		n_written = write( USB, &message[spot], 1 );
		spot += n_written;
	} while (message[spot-1] != '\r' && n_written > 0);
}

void Erosion(Mat openImage) {
  int erosion_type;
  if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
  else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
  else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

  Mat element = getStructuringElement( erosion_type,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );

  /// Apply the erosion operation
  erode( src, erosion_dst, element );
  imshow( "Erosion Demo", erosion_dst );
}

/** @function Dilation */
void Dilation( int, void* )
{
  int dilation_type;
  if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
  else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
  else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }

  Mat element = getStructuringElement( dilation_type,
                                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                       Point( dilation_size, dilation_size ) );
  /// Apply the dilation operation
  dilate( src, dilation_dst, element );
  imshow( "Dilation Demo", dilation_dst );
}