#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
#include <stdio.h>

#define DEBUG 1

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
Point cropPos, outerPos;

int i;
char input;
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
		// centerPos.x = CROPMIDPOINT;
		// centerPos.y = CROPMIDPOINT;

		//Creates the image variables used for this project, one is used for each step
		//to facilitate debugging and understanding the code.
		Mat image = originalImage.clone(), reducedImage, croppedImage, drawnImage, erodedImage, stepImage, areaImage, edgeImage, contrastImage, blurImage, binaryImage, contourImage, borderImage, threshImage;
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
			cin >> input;
			cout << input << endl;
			if(input == 'y' || input == 'Y') {
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
		//croppedImage = image(Rect((cropPos.x-(SQUARE_SIZE*0.5)), (cropPos.y-(SQUARE_SIZE*0.5)), SQUARE_SIZE, SQUARE_SIZE));

		if (DEBUG) {cout << "Image Cropped\n\r";}

		//Convert image to gray scale, (although it is already gray it is still treated as BGR)
		cvtColor(image, drawnImage, CV_BGR2GRAY);

		//Scales the image down to half it's size then back up, the purpose of this is 
		//to get rid of some of the noise in the image.  Similarly to a blur technique
		pyrDown(image, reducedImage, Size(image.cols/2, image.rows/2));
		pyrUp(reducedImage, image, image.size());
		
		image.convertTo(contrastImage, -1, 2.2, 10);
		if (DEBUG) {cout << "First noise reduction performed\n\r";}

		//Edge features of the image are detected using the Canny method
		Canny(contrastImage, edgeImage, THRESH, 2*THRESH, 3);

		if (DEBUG) {cout << "Edge detection performed\n\r";}

		// disImage((char *)"Edge Image", edgeImage, 1);

		//Blurs the image to reduce the noise in it
		blur(edgeImage, blurImage, Size(7,7));

		if (DEBUG) {cout << "Image blurred\n\r";}

		double radiusOuter = 100; //sqrt(pow(outerPos.x - cropPos.x, 2) + pow(outerPos.y - cropPos.y,2));
		double sectionAngle = atan((outerPos.y - cropPos.y)/(outerPos.x - cropPos.y));

		vector<vector<Scalar> > nodes;

		// for (int l = 0; l < NUM_COLUMNS; ++l) {
		// 	float colAngle = sectionAngle + l*(2*CV_PI/NUM_COLUMNS);
		// 	for (int j = 0; j < NUM_ROWS; ++ j) {
		// 		double pixX = (j/40)*(radiusOuter*cos()+cropPos.x);
		// 		double pixY = (j/40)*(radiusOuter*sin()+cropPos.y);
		// 		nodes[j][i][0] = blurImage.at<uchar>(round(pixY), round(pixX));
		// 	}
		// }

		cout << nodes << endl;

		float weight = 0.8;

		// Canny(blurImage, threshImage, THRESH, 2*THRESH, 3);
		threshold(blurImage, threshImage, THRESH, 255, THRESH_BINARY);

		//for (int l = 0; l < NUM_ROWS; ++)



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