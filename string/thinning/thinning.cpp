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

#define ELEMENT_TYPE 1
#define OPENING_SIZE 1
#define CLOSING_SIZE 1

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
int cnt = 0, brightness = 0, contourIndex = 0, intense, diffCount, eleType;

double largestArea, area;

Mat openElement, closeElement;
Rect bRect;
Scalar intenseAvg, color[3], morphAvg;

uchar* p;
uchar* q;


//Function Prototypes

void onMouse(int event, int x, int y, int flags, void* userdata);
void disImage(char* winName, Mat Image, int Position);

int main(int argc, char** argv) {

	Mat originalImage = imread("chinese.png", 0);

	Mat image = originalImage.clone();
	threshold(image, image, 127, 255, THRESH_BINARY); 
	Mat skel(image.size(), CV_8UC1, Scalar(0));
	Mat temp;
	Mat eroded;
	 
	Mat element = getStructuringElement(MORPH_CROSS, Size(3, 3));
	 
	bool done;		
	do
	{
		erode(image, eroded, element);
		dilate(eroded, temp, element); // temp = open(image)
		subtract(image, temp, temp);
		bitwise_or(skel, temp, skel);
		eroded.copyTo(image);

		done = (countNonZero(image) == 0);
	} while (!done);

	disImage((char *)"Original Image", originalImage, 1);
	disImage((char *)"Skeletonized Image", skel, 2);
	waitKey(0);
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