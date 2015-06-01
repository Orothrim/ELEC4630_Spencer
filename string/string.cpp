#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
#include <stdio.h>
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include "/home/swift/code/customfunctions/DisplayFrames.h"
#include "/home/swift/code/customfunctions/ZhangSuenFunctions.h"

#define DEBUG 0

//Used for element creation.
#define ELEMENT_TYPE 0
#define OPENING_SIZE 1
#define CLOSING_SIZE 3

//Used to access all of the images.
#define ONESCOLUMN 6
#define TENSCOLUMN 5
#define NUMIMAGES 16

//Colours used in image analysis.
#define WHITE 255

//Size of the images when displayed.
// #define WINDOWX 533
// #define WINDOWY 400
#define STRING1 130
#define STRING2 155

//Pixel to mm ratio
#define RATIO 0.7
#define ROOT2 1.41421356237

using namespace cv;
using namespace std;

char input;
int eleType;
double stringLengths[5], stringLength, pixelLengths[15];

double length = 0;

Mat openElement, closeElement;
Scalar intenseAvg, color[3], morphAvg;



int main(int argc, char** argv) {

	//The first file name.
	string filename = "String1_1.jpg";

		//There are three types of elements for dilation and eroding, rectangular, cross, and ellipse.
		if( ELEMENT_TYPE == 0 ){ eleType = MORPH_RECT; }
		else if( ELEMENT_TYPE == 1 ){ eleType = MORPH_CROSS; }
		else if( ELEMENT_TYPE == 2) { eleType = MORPH_ELLIPSE; }

		//These are the elements using for opening and closing, through erosion and dilation.
		openElement = getStructuringElement(eleType , Size( OPENING_SIZE+1, OPENING_SIZE+1 ), Point(OPENING_SIZE, OPENING_SIZE));
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
			Mat image = originalImage.clone(), threshImage, erodedImage, resultImage, openImage, closeImage, compareImage, contrastImage, gradImage, dilatedImage, skelImage, countImage, botImage;
			Mat countArray = (Mat_<double>(3,3) << ROOT2, 1, ROOT2, 1, 0, 1, ROOT2, 1, ROOT2);

			if (DEBUG) {cout << "Variables Created\n\r";}

			//Contrasts and brightens the image.
			// image.convertTo(contrastImage, -1, 1.5, 10);
			equalizeHist(image, contrastImage);

			if (DEBUG) {cout << "Contrast Performed\n\r";}

			//Performs a bottom hat operation, or a black-top hat operation.
			dilate(contrastImage, dilatedImage, closeElement);
			erode(dilatedImage, closeImage, closeElement);
			subtract(closeImage, contrastImage, botImage);

			if (DEBUG) {cout << "Bottom Hat Image Created\n\r";}

			//Thresholding is performed to differentiate the background from the string.
			intenseAvg = mean(botImage);
			threshold(botImage, threshImage, 0, WHITE, THRESH_BINARY | THRESH_OTSU);

			if (DEBUG) {cout << "Thresholding Performed\n\r";}

			//Skeletonizes the image for analysis.
			skelImage = threshImage.clone();
			zhangSuen(skelImage);

			if (DEBUG) {cout << "Skeletonization Performed\n\r";}

			countImage = skelImage.clone();

			cout << countNonZero(skelImage) << endl;

			//Counts the white pixels remaining in the image
			countImage /= WHITE;
			length = 0;
			for(int j = 0; j < countImage.rows - 1; ++j)
			{

				//A pointer to each row of the image being used
				uchar* previous = countImage.ptr<uchar>(j - 1);
				uchar* current  = countImage.ptr<uchar>(j);
				uchar* next = countImage.ptr<uchar>(j + 1);

				for(int l = 0; l < countImage.cols - 1; ++l)
				{
					if (!(current[l] == 0)) {

						//Applies the following array to each set pixel in the row, to get the string lengths for that pixel:
						// [sqrt2 , 1 , sqrt2]
						// [1     , 0 ,     1]
						// [sqrt2 , 1 , sqrt2]
						length += (current[l-1] + current[l+1] + previous[l] + next[l] + ROOT2*previous[l+1] + ROOT2*next[l + 1] + ROOT2*previous[l - 1] + ROOT2*next[l - 1]);

						current[l] = 0;
					}
				}
			}

			pixelLengths[(5*i)+k-6] = length;

			countImage *= WHITE;

			//Sets the length in an array
			stringLengths[k-1] = pixelLengths[(5*i)+k-6];

			//RATIO was found using the pixel to mm ratio in the first 10 images
			//stringLengths[k-1] = RATIO*stringLengths[k-1];
			cout << filename << ": " << stringLengths[k-1] << endl;

			//Display Images
			disImage((char *)"Current Image", originalImage, 1);
			disImage((char *)"Contrast Image", contrastImage, 2);
			disImage((char *)"Bottom Hat Image", botImage, 4);
			disImage((char *)"Thresh Image", threshImage, 5);
			disImage((char *)"Skeletonized Image", skelImage, 6);

			waitKey(0);
		}

	//Uses the average of the 5 images to get the string length.
	stringLength = 0;
	for (int j = 0; j < 5; j++) {
		stringLength += stringLengths[j];
	}

	stringLength = stringLength/5;
	cout << "For string "<< i << " the string length is " << stringLength << "." << endl;
	waitKey(0);

	}
}