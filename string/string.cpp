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
#define OPENING_SIZE 1
#define CLOSING_SIZE 1
#define CLEANING_SIZE 2

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

Mat openElement, closeElement, cleanElement;
Rect bRect;
Scalar intenseAvg, color[3], morphAvg;

uchar* p;
uchar* q;


//Function Prototypes

void onMouse(int event, int x, int y, int flags, void* userdata);
void disImage(char* winName, Mat Image, int Position);
void thinningIteration(Mat& im, int iter);
void thinning(Mat& im);

int main(int argc, char** argv) {

	string filename = "String1_1.jpg";

		if( ELEMENT_TYPE == 0 ){ eleType = MORPH_RECT; }
		else if( ELEMENT_TYPE == 1 ){ eleType = MORPH_CROSS; }
		else if( ELEMENT_TYPE == 2) { eleType = MORPH_ELLIPSE; }

		openElement = getStructuringElement(eleType , Size( 2*OPENING_SIZE + 1, 2*OPENING_SIZE+1 ), Point(OPENING_SIZE, OPENING_SIZE));
		closeElement = getStructuringElement(eleType , Size( 2*CLOSING_SIZE + 1, 2*CLOSING_SIZE+1 ), Point(CLOSING_SIZE, CLOSING_SIZE));
		cleanElement = getStructuringElement(eleType , Size( 2*CLEANING_SIZE + 1, 2*CLEANING_SIZE+1 ), Point(CLEANING_SIZE, CLEANING_SIZE));

	for(int i = 1; i <= 3; i++) {

		stringstream first;

		first << i;
		filename.replace(6,1,first.str());

		for(int k = 1; k <= 5; k++) {

			stringstream second;

			second << k;
			filename.replace(8,1,second.str());
			
			cout << filename << endl;

			Mat originalImage = imread(filename, 0);
			if(originalImage.empty()) {
				//Checks that the file can be opened, if it can't, prings "can not open" 
				//and end the program
				cout << "can not open " << filename << endl;
				return -1;
			}
			if (DEBUG) {cout << "File Loaded\n\r";}


			//Creates the image variables used for this project, one is used for each step
			//to facilitate debugging and understanding the code.
			Mat image = originalImage.clone(), threshImage, erodedImage, resultImage, openImage, closeImage, compareImage, contrastImage, gradImage, dilatedImage, skelImage;

			image.convertTo(contrastImage, -1, 1.2, 10);


			//Morphological Gradient, created by subtracting the eroded image from the dilated image.
			erode(contrastImage, erodedImage, openElement);
			dilate(contrastImage, dilatedImage, openElement);
			subtract(dilatedImage, erodedImage, gradImage);

			intenseAvg = mean(gradImage);
			threshold(gradImage, threshImage, intenseAvg[0]*5, 255, THRESH_BINARY);

			dilate(threshImage, dilatedImage, closeElement);
			erode(dilatedImage, closeImage, closeElement);

			erode(closeImage, erodedImage, openElement);
			dilate(erodedImage, openImage, openElement);

			skelImage = openImage.clone();

			thinning(skelImage);

			disImage((char *)"Current Image", originalImage, 1);
			disImage((char *)"Contrast Image", contrastImage, 2);
			disImage((char *)"Morphological Gradient Image", gradImage, 3);
			disImage((char *)"Thresh Image", threshImage, 4);
			disImage((char *)"Opened Image", openImage, 5);
			disImage((char *)"Skeletonized Image", skelImage, 6);

			waitKey(0);
		}
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

/**
 * Perform one thinning iteration.
 * Normally you wouldn't call this function directly from your code.
 *
 * @param  im    Binary image with range = 0-1
 * @param  iter  0=even, 1=odd
 */
void thinningIteration(Mat& im, int iter)
{
    Mat marker = Mat::zeros(im.size(), CV_8UC1);

    for (int i = 1; i < im.rows-1; i++)
    {
        for (int j = 1; j < im.cols-1; j++)
        {
            uchar p2 = im.at<uchar>(i-1, j);
            uchar p3 = im.at<uchar>(i-1, j+1);
            uchar p4 = im.at<uchar>(i, j+1);
            uchar p5 = im.at<uchar>(i+1, j+1);
            uchar p6 = im.at<uchar>(i+1, j);
            uchar p7 = im.at<uchar>(i+1, j-1);
            uchar p8 = im.at<uchar>(i, j-1);
            uchar p9 = im.at<uchar>(i-1, j-1);

            int A  = (p2 == 0 && p3 == 1) + (p3 == 0 && p4 == 1) + 
                     (p4 == 0 && p5 == 1) + (p5 == 0 && p6 == 1) + 
                     (p6 == 0 && p7 == 1) + (p7 == 0 && p8 == 1) +
                     (p8 == 0 && p9 == 1) + (p9 == 0 && p2 == 1);
            int B  = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
            int m1 = iter == 0 ? (p2 * p4 * p6) : (p2 * p4 * p8);
            int m2 = iter == 0 ? (p4 * p6 * p8) : (p2 * p6 * p8);

            if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
                marker.at<uchar>(i,j) = 1;
        }
    }

    im &= ~marker;
}

/**
 * Function for thinning the given binary image
 *
 * @param  im  Binary image with range = 0-255
 */
void thinning(Mat& im)
{
    im /= 255;

    Mat prev = Mat::zeros(im.size(), CV_8UC1);
    Mat diff;

    do {
        thinningIteration(im, 0);
        thinningIteration(im, 1);
        absdiff(im, prev, diff);
        im.copyTo(prev);
    } 
    while (countNonZero(diff) > 0);

    im *= 255;
}