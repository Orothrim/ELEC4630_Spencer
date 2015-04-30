#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
#include <stdio.h>

#define DEBUG 1
#define SQUARE_SIZE 180
#define THRESH 60

using namespace cv;
using namespace std;

Point mousePos;
Point cropPos, centerPos;

char i;
int key;
int largestArea = 0;
int contourIndex = 0;
Rect bRect;
double heartArea;
int click = 0;


void onMouse(int event, int x, int y, int flags, void* userdata) {
	//Event that is attached to a mouseclick after "setMouseCallback" occurs.
	if(event == EVENT_LBUTTONDOWN) {
		mousePos.x = x;
		mousePos.y = y;
	}
}


int main(int argc, char** argv) {

	//If no argument is given to "./Contours" then MRI1_01.png is used.
	for(int k = 1; k <= 16; k++) {

		string filename = "MRI1_01.png";
		stringstream convert;

		if (k > 1) {
			convert << k;
			if (k < 10) {
				filename.replace(6,1,convert.str());
			}
			else {
				filename.replace(5,2,convert.str());
			}
		}
			cout << filename << endl;

		Mat originalImage = imread(filename);
		if(originalImage.empty()) {
			//Checks that the file can be opened, if it can't, prings "can not open" 
			//and end the program
			cout << "can not open " << filename << endl;
			return -1;
		}
//		if (DEBUG) {cout << "File Loaded\n\r";}

		//cropPos is set to the middle pixel in the image
		cropPos.x = 292;
		cropPos.y = 360;
		centerPos.x = 90;
		centerPos.y = 90;

		//Creates the image variables used for this project, one is used for each step
		//to facilitate debugging.
		Mat image = originalImage.clone(), pyr;
		Mat edgeImage, contrastImage, blurImage, binaryImage, contourImage, croppedImage;


		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		RNG rng(12345);

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

		Mat drawnImage = Mat::zeros(croppedImage.size(), CV_8UC3);

		//Scales the image down to half it's size then back up, the purpose of this is 
		//to get rid of some of the noise in the image.  Similarly to a blur technique
		pyrDown(croppedImage, pyr, Size(croppedImage.cols/2, croppedImage.rows/2));
		pyrUp(pyr, croppedImage, croppedImage.size());
		croppedImage.convertTo(contrastImage, -1, 2.2, 10);

		//Edge features of the image are detected using the Canny method
		Canny(contrastImage, edgeImage, THRESH, 2*THRESH, 3);

		//Blurs the image to reduce the noise in it
		blur(edgeImage, blurImage, Size(8,8));

		//Only need to print if DEBUG is 1
		if (DEBUG) {cout << "Edge Image Created\n\r";}

		//Move the blurImage into croppedImage so blurImage won't be affected by findContours
		// and can be displayed for debugging.
		croppedImage = blurImage.clone();

		//Finds any contours in the image, neglecting any holes in the contours.
		findContours(croppedImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0,0));

		//Only need to print if DEBUG is 1
		if (DEBUG) {cout << "Contours Found\n\r";}

		//Go through each contour trying to find the largest area one, which should be the heart after cropping.
		largestArea = 0;
		for(int i=0; i<contours.size(); i++) {
			double area = contourArea(contours[i], false);
			bRect = boundingRect(contours[i]);
			if(area > largestArea && bRect.contains(centerPos)) {
				//cout << bRect << endl;
				largestArea = area;
				contourIndex = i;
			}
		}

		//Use blue for the largest area contour, then draw it.
		Scalar color = Scalar(255, 0, 0);
		drawContours(drawnImage, contours, contourIndex, color, CV_FILLED, 8, hierarchy);

		namedWindow("Contours Image", WINDOW_NORMAL);
		imshow("Contours Image", drawnImage);
		resizeWindow("Contours Image", 325, 400);
		moveWindow("Contours Image", 330, 425);


		//Finds the appropriate area and prints it.
		heartArea = contourArea(contours[contourIndex], false);
		cout << "The heart is " << heartArea << " pixels in area.\n\r";
		//These are only shown when debugging.
		if(DEBUG) {
			//cout << "Contours Drawn\n\r";
			namedWindow("Original Image", WINDOW_NORMAL);
			imshow("Original Image", originalImage);
			resizeWindow("Original Image", 325, 400);
			moveWindow("Original Image", 0, 0);

			namedWindow("Edge Image", WINDOW_NORMAL);
			imshow("Edge Image", edgeImage);
			resizeWindow("Edge Image", 325, 400);
			moveWindow("Edge Image", 330, 0);

			namedWindow("Blur Image", WINDOW_NORMAL);
			imshow("Blur Image", blurImage);
			resizeWindow("Blur Image", 325, 400);
			moveWindow("Blur Image", 0, 425);
		}
		waitKey(0);
		destroyAllWindows();
	}
}