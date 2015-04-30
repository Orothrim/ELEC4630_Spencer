#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
#include <stdio.h>

#define DEBUG 0

//Cropped area size.
#define SQUARE_SIZE 200
#define CROPMIDPOINT 100

//Canny edge detection threshold, it is quite low due to the low 
//brightness of the images.
#define THRESH 60

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
#define WINDOWX 325
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

Point mousePos;
Point cropPos, centerPos;

int i;
double heartArea;
int click = 0;


//Function Prototypes

void onMouse(int event, int x, int y, int flags, void* userdata);
void disImage(char* winName, Mat Image, int Position);

int main(int argc, char** argv) {


	string filename = "MRI1_01.png";

	//If no argument is given to "./Contours" then MRI1_01.png is used.
	for(int k = 1; k <= NUMIMAGES; k++) {

		//renames the file for each iteration of the for loop.
		
		stringstream convert;

		if (k > 1) {
			convert << k;
			if (k < 10) {
				filename.replace(ONESCOLUMN, 1, convert.str());
			}
			else {
				filename.replace(TENSCOLUMN, 2, convert.str());
			}
		}


		Mat originalImage = imread(filename);
		if(originalImage.empty()) {
			//Checks that the file can be opened, if it can't, prints "can not open" 
			//and end the program
			cout << "can not open " << filename << endl;
			return -1;
		}
		if (DEBUG) {cout << "File Loaded\n\r";}

		//cropPos is set to the middle pixel in the image
		cropPos.x = MIDDLEX;
		cropPos.y = MIDDLEY;
		centerPos.x = CROPMIDPOINT;
		centerPos.y = CROPMIDPOINT;

		//Creates the image variables used for this project, one is used for each step
		//to facilitate debugging and understanding the code.
		Mat image = originalImage.clone(), reducedImage, croppedImage, drawnImage, erodedImage, stepImage, areaImage, edgeImage, contrastImage, blurImage, binaryImage, contourImage, borderImage;
		Mat rotateMatrix(2, 3, CV_32FC1);
		Mat kernel = Mat::ones(3, 3, CV_8U);

		//Pointers later used to go through each pixel in the images
		uchar* p;
		uchar* q;

		if (DEBUG) {cout << "Variables Created\n\r";}

		//Asks for a mouse click to locate the heart, if any character except y or Y 
		//is supplied it uses the center of the image
		if(click == 0) {
			cout << "Would you like to center the search area? [y/n]\n\r";
			cin >> i;
			if(i == 'y' || i == 'Y') {
				namedWindow("Click Image", WINDOW_AUTOSIZE);
				setMouseCallback("Click Image", onMouse, NULL);
				imshow("Click Image", image);
				waitKey(0);
				destroyWindow("Click Image");
				if(DEBUG) {cout << mousePos <<"\n\r";}
				cropPos = mousePos;
			}
			else {click = 1;}
		}

		//Crops the main image around the cropPos point
		croppedImage = image(Rect((cropPos.x-(SQUARE_SIZE*0.5)), (cropPos.y-(SQUARE_SIZE*0.5)), SQUARE_SIZE, SQUARE_SIZE));

		if (DEBUG) {cout << "Image Cropped\n\r";}

		//Convert image to gray scale, (although it is already gray it is still treated as BGR)
		cvtColor(croppedImage, drawnImage, CV_BGR2GRAY);

		//Scales the image down to half it's size then back up, the purpose of this is 
		//to get rid of some of the noise in the image.  Similarly to a blur technique
		pyrDown(croppedImage, reducedImage, Size(croppedImage.cols/2, croppedImage.rows/2));
		pyrUp(reducedImage, croppedImage, croppedImage.size());
		croppedImage.convertTo(contrastImage, -1, 2.2, 10);
		if (DEBUG) {cout << "First noise reduction performed\n\r";}

		//Edge features of the image are detected using the Canny method
		Canny(contrastImage, edgeImage, THRESH, 2*THRESH, 3);

		if (DEBUG) {cout << "Edge detection performed\n\r";}

		//Blurs the image to reduce the noise in it
		blur(edgeImage, blurImage, Size(8,8));

		if (DEBUG) {cout << "Image blurred\n\r";}

		//Set all pixels in the blurred edge image to white if they are not zero.
		//The purpose of this is to give a clear binary image for further operations.
		for( int i = 0; i < blurImage.cols; ++i) {
			p = blurImage.ptr<uchar>(i);
			for (int j = 0; j < blurImage.rows; ++j) {
				if (!(p[j] == 0)) {
					p[j] = WHITE;
				}
			}
		}

		if (DEBUG) {cout << "Non-zero pixels set to white\n\r";}

		//Fill the border around the heart ventricle with a color that isn't in the image
		//otherwise (MANICOLOUR), this will allow the two borders to be easily identified.
		floodFill(blurImage, Point(0,70), Scalar(MANICOLOUR), 0, Scalar(), Scalar(), 4);

		if (DEBUG) {cout << "Heart border surrounded\n\r";}

		//Set any pixel that is the MANICOLOUR to black and all other pixels to white, this
		//is to ensure the borders can easily be picked out of the image.
		for( int i = 0; i < blurImage.cols; ++i) {
		  p = blurImage.ptr<uchar>(i);
			for (int j = 0; j < blurImage.rows; ++j) {
				if (!(p[j] == MANICOLOUR)) {
					p[j] = WHITE;
				}
				else {
					p[j] = 0;
				}
			}
		}

		if (DEBUG) {cout << "File Loaded\n\r";}

		//The function erode is used to slightly reduce the size of the white areas in the
		//image so as to allow a border to be extracted from the image.
		erode(blurImage, erodedImage, kernel);
		areaImage = blurImage - erodedImage;

		if (DEBUG) {cout << "Borders found\n\r";}

		//Fill the heart with MANICOLOUR so that the area can be counted.
		floodFill(areaImage, Point(CROPMIDPOINT,CROPMIDPOINT), Scalar(MANICOLOUR), 0, Scalar(), Scalar(), 4);
		heartArea = 0;

		if (DEBUG) {cout << "Heart filled\n\r";}

		//Count each instance of MANICOLOUR, to get the heart area.
		for( int i = 0; i < areaImage.cols; ++i) {
			p = areaImage.ptr<uchar>(i);
			for (int j = 0; j < areaImage.rows; ++j) {
				if (p[j] == MANICOLOUR) {
					heartArea++;
				}
			}
		}

		//Prints the area of the heart.

		cout << "\nIn image " << filename << " the heart is " << heartArea << " pixels in area.\n\r";
		
		if (DEBUG) {cout << "Area counted\n\r";}

		//Draw the borders from the border image onto the original cropped image.
		for( int i = 0; i < areaImage.cols; ++i) {
			p = areaImage.ptr<uchar>(i);
			q = drawnImage.ptr<uchar>(i);
			for (int j = 0; j < areaImage.rows; ++j) {
				if (p[j] == WHITE) {
					q[j] = WHITE;
				}
			}
		}

		if (DEBUG) {cout << "Borders overlaid\n\r";}

		//Display the final product, along with the most important steps.
		disImage((char *)"Drawn Image", drawnImage, 4);
		disImage((char *)"Original Image", originalImage, 1);
		disImage((char *)"Edge Image", edgeImage, 2);
		disImage((char *)"Area Image", areaImage, 3);

		waitKey(0);
		destroyAllWindows();
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