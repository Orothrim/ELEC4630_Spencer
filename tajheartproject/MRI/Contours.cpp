#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"

#define DEBUG 1
#define SQUARE_SIZE 200

using namespace cv;
using namespace std;

Point mousePos;
Point cropPos;
char i;
int key;
int largestArea = 0;
int contourIndex = 0;

void onMouse(int event, int x, int y, int flags, void* userdata) {
	if(event == EVENT_LBUTTONDOWN) {
		mousePos.x = x;
		mousePos.y = y;
	}
}


int main(int argc, char** argv) {
	const char* filename = argc >= 2 ? argv[1] : "MRI1_01.png";

  Mat originalImage = imread(filename);
  if(originalImage.empty()) {
    cout << "can not open " << filename << endl;
    return -1;
  }
	if (DEBUG) {cout << "File Loaded\n\r";}

	cropPos.x = 292;
	cropPos.y = 360;

	int thres = 80;
  Mat image = originalImage.clone(), pyr;
  Mat edgeImage, contrastImage, blurImage, binaryImage, contourImage, croppedImage;


  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  RNG rng(12345);

	cout << "Would you like to center the search area? [y/n]\n\r";
	cin >> i;
	if(i=='y') {
		namedWindow("Click Image", WINDOW_AUTOSIZE);
	  setMouseCallback("Click Image", onMouse, NULL);
	  imshow("Click Image", image);
	  waitKey(0);
	  destroyWindow("Click Image");
	  if(DEBUG) {cout << mousePos <<"\n\r";}
	  cropPos = mousePos;
	}

	croppedImage = image(Rect((cropPos.x-(SQUARE_SIZE*0.5)), (cropPos.y-(SQUARE_SIZE*0.5)), SQUARE_SIZE, SQUARE_SIZE));

  Mat drawnImage = Mat::zeros(croppedImage.size(), CV_8UC3);

  pyrDown(croppedImage, pyr, Size(croppedImage.cols/2, croppedImage.rows/2));
  pyrUp(pyr, croppedImage, croppedImage.size());
	croppedImage.convertTo(contrastImage, -1, 2.2, 10);

  Canny(contrastImage, edgeImage, thres, 2*thres, 3);
  blur(edgeImage, blurImage, Size(7,7));

	if (DEBUG) {cout << "Edge Image Created\n\r";}

	croppedImage = blurImage.clone();

  findContours(croppedImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
	if (DEBUG) {cout << "Contours Found\n\r";}

  for(int i=0; i<contours.size(); i++) {
		double area = contourArea(contours[i], false);
		if(area > largestArea) {
			largestArea = area;
			contourIndex = i;
		}
  }

 	Scalar color = Scalar(255, 0, 0);
  drawContours(drawnImage, contours, contourIndex, color, CV_FILLED, 8, hierarchy);

	namedWindow("Contours Image", WINDOW_NORMAL);
	imshow("Contours Image", drawnImage);
	resizeWindow("Contours Image", 325, 400);
	moveWindow("Contours Image", 330, 425);

	if(DEBUG) {
		cout << "Contours Drawn\n\r";
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
}
