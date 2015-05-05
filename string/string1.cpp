#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
#include <stdio.h>
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions

#define DEBUG 0

//Used for element creation.
#define ELEMENT_TYPE 2
#define OPENING_SIZE 1
#define CLOSING_SIZE 1

//Used to access all of the images.
#define ONESCOLUMN 6
#define TENSCOLUMN 5
#define NUMIMAGES 16

//Colours used in image analysis.
#define WHITE 255

//Size of the images when displayed.
#define WINDOWX 533
#define WINDOWY 400
#define STRING1 130
#define STRING2 155

//Pixel to mm ratio
#define RATIO 1.13139


using namespace cv;
using namespace std;

char input;
int eleType, pixelLengths[15]; 
double stringLengths[5], stringLength;

double largestArea, area;

Mat openElement, closeElement;
Scalar intenseAvg, color[3], morphAvg;



//Function Prototypes

void onMouse(int event, int x, int y, int flags, void* userdata);
void disImage(char* winName, Mat Image, int Position);
void thinningIteration(Mat& im, int iter);
void thinning(Mat& im);

int main(int argc, char** argv) {

	//The first file name.
	string filename = "String1_1.jpg";

		//There are three types of elements for dilation and eroding, rectangular, cross, and ellipse.
		if( ELEMENT_TYPE == 0 ){ eleType = MORPH_RECT; }
		else if( ELEMENT_TYPE == 1 ){ eleType = MORPH_CROSS; }
		else if( ELEMENT_TYPE == 2) { eleType = MORPH_ELLIPSE; }

		//These are the elements using for opening and closing, through erosion and dilation.
		openElement = getStructuringElement(eleType , Size( 2*OPENING_SIZE+1, 2*OPENING_SIZE+1 ), Point(OPENING_SIZE, OPENING_SIZE));
		closeElement = getStructuringElement(eleType , Size( 2*CLOSING_SIZE+1, 2*CLOSING_SIZE+1 ), Point(CLOSING_SIZE, CLOSING_SIZE));

	//This for loop is used to go through the different strings.
	for(int i = 1; i <= 3; i++) {

		//Used to replace the first number in the filename.
		stringstream first;

		first << i;
		filename.replace(6,1,first.str());

		//This for loop is used to swap between photos of the same string.
		for(int k = 1; k <= 5; k++) {

			//Used to replace the second number in the filename
			stringstream second;

			second << k;
			filename.replace(8,1,second.str());
			
			if (DEBUG) {cout << filename << endl;}

			//Read in the file, if possible, else exit function.
			Mat originalImage = imread(filename, 0);
			if(originalImage.empty()) {
				//Checks that the file can be opened, if it can't, prints "can not open" 
				//and end the program
				cout << "can not open " << filename << endl;
				return -1;
			}
			if (DEBUG) {cout << "File Loaded\n\r";}


			//Creates the image variables used for this project, one is used for each step
			//to facilitate debugging and understanding the code.
			Mat image = originalImage.clone(), threshImage, erodedImage, resultImage, openImage, closeImage, compareImage, contrastImage, gradImage, dilatedImage, skelImage;

			if (DEBUG) {cout << "Variables Created\n\r";}

			//Contrasts and brightens the image.
			image.convertTo(contrastImage, -1, 1.5, 10);

			if (DEBUG) {cout << "Contrast Performed\n\r";}

			//Morphological Gradient, created by subtracting the eroded image from the dilated image.
			erode(contrastImage, erodedImage, openElement);
			dilate(contrastImage, dilatedImage, openElement);
			subtract(dilatedImage, erodedImage, gradImage);

			if (DEBUG) {cout << "Morphological Gradient Image Created\n\r";}

			//Thresholding is performed to differentiate the background from the string.
			intenseAvg = mean(gradImage);
			threshold(gradImage, threshImage, intenseAvg[0]*5, WHITE, THRESH_BINARY);

			if (DEBUG) {cout << "Thresholding Performed\n\r";}

			//Performs closing on the image, this is done before opening in order to remove small holes in the string.
			dilate(threshImage, dilatedImage, closeElement);
			erode(dilatedImage, closeImage, closeElement);

			if (DEBUG) {cout << "Opening Performed\n\r";}

			//Performs opening on the image, in order to remove noise from the image.
			erode(closeImage, erodedImage, openElement);
			dilate(erodedImage, openImage, openElement);

			if (DEBUG) {cout << "Closing Performed\n\r";}

			//Skeletonizes the image for analysis.
			skelImage = openImage.clone();
			thinning(skelImage);

			if (DEBUG) {cout << "Skeletonization Performed\n\r";}

			//Counts the white pixels remaining in the image
			pixelLengths[(5*i)+k-6] = countNonZero(skelImage);
			stringLengths[k-1] = pixelLengths[(5*i)+k-6];

			stringLengths[k-1] = RATIO*stringLengths[k-1];
			cout << filename << ": " << stringLengths[k-1] << endl;

			disImage((char *)"Current Image", originalImage, 1);
			disImage((char *)"Contrast Image", contrastImage, 2);
			disImage((char *)"Morphological Gradient Image", gradImage, 3);
			disImage((char *)"Thresh Image", threshImage, 4);
			disImage((char *)"Opened Image", openImage, 5);
			disImage((char *)"Skeletonized Image", skelImage, 6);

			waitKey(0);
		}

	stringLength = 0;
	for (int j = 0; j < 5; j++) {
		stringLength += stringLengths[j];
	}

	stringLength = stringLength/5;
	cout << "For string "<< i << " the string length is " << stringLength << "." << endl;
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


//Perform one iteration of the Zhang Suen thinning algorithm.
//Don't call this function directly from your code.

//@param  im    Binary image with range = 0-1
//@param  iter  0=even, 1=odd

void thinningIteration(Mat& image, int iter)
{
    Mat marker = Mat::zeros(image.size(), CV_8UC1);

    for (int i = 1; i < image.rows-1; i++)
    {
        for (int j = 1; j < image.cols-1; j++)
        {
            uchar p2 = image.at<uchar>(i-1, j);
            uchar p3 = image.at<uchar>(i-1, j+1);
            uchar p4 = image.at<uchar>(i, j+1);
            uchar p5 = image.at<uchar>(i+1, j+1);
            uchar p6 = image.at<uchar>(i+1, j);
            uchar p7 = image.at<uchar>(i+1, j-1);
            uchar p8 = image.at<uchar>(i, j-1);
            uchar p9 = image.at<uchar>(i-1, j-1);

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

    image &= ~marker;
}

/**
 * Function for thinning the given binary image
 *
 * @param  im  Binary image with range = 0-WHITE
 */
void thinning(Mat& im)
{
    im /= WHITE;

    Mat prev = Mat::zeros(im.size(), CV_8UC1);
    Mat diff;

    do {
        thinningIteration(im, 0);
        thinningIteration(im, 1);
        absdiff(im, prev, diff);
        im.copyTo(prev);
    } 
    while (countNonZero(diff) > 0);

    im *= WHITE;
}