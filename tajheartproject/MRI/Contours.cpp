#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"

#define DEBUG 1


using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	const char* filename = argc >= 2 ? argv[1] : "MRI1_01.png";

  Mat originalImage = imread(filename);
  if(originalImage.empty())
  {
    cout << "can not open " << filename << endl;
    return -1;
  }
	if (DEBUG) {cout << "File Loaded\n\r";}

  Mat image = originalImage, pyr;
  // pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
  // pyrUp(pyr, image, image.size());

  Mat grayImage, edgeImage, binaryImage;
  cvtColor(image, grayImage, CV_BGR2GRAY);
  Canny(image, edgeImage, 40, 60, 3);
  cvtColor(image, binaryImage, CV_BGR2GRAY);
	if (DEBUG) {cout << "Binary Image Created\n\r";}

  //threshold(image, binaryImage, 50, 255, THRESH_BINARY);

  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  RNG rng(12345);
  findContours(binaryImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
	if (DEBUG) {cout << "Contours Found\n\r";}

  Mat drawnImage = Mat::zeros(binaryImage.size(), CV_8UC3);

  for(int i=0; i<contours.size(); i++)
  {
  	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
  	drawContours(drawnImage, contours, i, color, CV_FILLED, 8, hierarchy);
  }

  if(DEBUG) 
  {
	  cout << "Contours Drawn\n\r";
	  namedWindow("Original Image", WINDOW_NORMAL);
	  imshow("Original Image", image);
	  namedWindow("Gray Image", WINDOW_NORMAL);
	  imshow("Gray Image", grayImage);  
	  namedWindow("Edge Image", WINDOW_NORMAL);
	  imshow("Edge Image", edgeImage);
  }

  namedWindow("Contours Image", WINDOW_NORMAL);
  imshow("Contours Image", drawnImage);
  waitKey(0);
}

// double a = contourArea(controus[i], false);
//   	if(a>largest_area){
//   		largest_area = 
//   	}