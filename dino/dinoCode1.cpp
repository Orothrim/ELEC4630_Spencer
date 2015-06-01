#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/video/background_segm.hpp>
#include "iostream"
#include "/home/swift/code/customfunctions/DisplayFrames.h"

#define DEBUG 1

#define ELEMENT_TYPE 2
#define OPENING_SIZE 2
#define CLOSING_SIZE 2

//Colours used in image analysis.
#define WHITE 255

//Size of the images when displayed.
#define WINDOWX 500
#define WINDOWY 400

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{

	Mat image, originalImage, openElement, closeElement, ignoreBlueImage, maskImage;

	//Pointers later used to go through each pixel in the images
	uchar* p;
	uchar* q;

	int eleType;

	//Create element for dilation and erosion
	if( ELEMENT_TYPE == 0 ){ eleType = MORPH_RECT; }
	else if( ELEMENT_TYPE == 1 ){ eleType = MORPH_CROSS; }
	else if( ELEMENT_TYPE == 2) { eleType = MORPH_ELLIPSE; }

	openElement = getStructuringElement(eleType , Size( 2*OPENING_SIZE + 1, 2*OPENING_SIZE+1 ), Point(OPENING_SIZE, OPENING_SIZE));
	closeElement = getStructuringElement(eleType , Size( 2*CLOSING_SIZE + 1, 2*CLOSING_SIZE+1 ), Point(CLOSING_SIZE, CLOSING_SIZE));

	#if DEBUG
	cout << "Elements Created" << endl;
	#endif

	string filename = "dino00.jpg";

	for(int i = 0; i <= 3; i++) {

		//Used to replace the first number in the filename.
		stringstream first;

		first << i;
		filename.replace(4,1,first.str());

		//This for loop is used to swap between photos of the same string.
		for(int j = 0; j <= 9; j++) {

			if(i > 2 && j > 5) {
				return -1;
			}
			//Used to replace the second number in the filename
			stringstream second;

			second << j;
			filename.replace(5,1,second.str());

			if (DEBUG) {cout << filename << endl;}

			//Read in the file, if possible, else exit function.
			Mat originalImage = imread(filename);

			if(originalImage.empty()) {
				//Checks that the file can be opened, if it can't, prints "can not open" 
				//and end the program
				cout << "can not open " << filename << endl;
				return -1;
			}

			image = originalImage.clone();

			// cvtColor(image, grayImage, CV_BGR2GRAY);

			ignoreBlueImage = originalImage.clone();
			maskImage = Mat::zeros(ignoreBlueImage.rows, ignoreBlueImage.cols, CV_8UC1);

			for( int k = 0; k < ignoreBlueImage.rows; ++k) {
				p = ignoreBlueImage.ptr<uchar>(k);
				q = maskImage.ptr<uchar>(k);
				for (int l = 0; l < 3*ignoreBlueImage.cols; l += 3) {
					if (p[l] > p[l+1] && p[l] > p[l+2]) {
						p[l] = 0;
						p[l+1] = 0;
						p[l+2] = 0;
					}
					else {
						q[(l+1)/3] = WHITE;
					}
				}
			}

			// cvtColor(ignoreBlueImage, maskImage, CV_BGR2GRAY);


			//Display Images
			disImage((char *)"Current Image", originalImage, 1);
			disImage((char *)"Ignore Blue Image", ignoreBlueImage, 2);
			disImage((char *)"Mask Image", maskImage, 3);
			// disImage((char *)"Bottom Hat Image", botImage, 4);
			// disImage((char *)"Thresh Image", threshImage, 5);
			// disImage((char *)"Skeletonized Image", skelImage, 6);

			waitKey(0);
		}
	}
}