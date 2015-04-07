#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"


using namespace cv;
using namespace std;

int main()
{
	const char* filename = argc >= 2 ? argv[1] : "MRI1_01.png";

  Mat originalImage = imread(filename);
  if(originalImage.empty())
  {
    help();
    cout << "can not open " << filename << endl;
    return -1;
  }

  Mat grayImage, edgeImage;
  cvtColor(originalImage, grayImage, CV_BGR2GRAY);
  Canny(grayImage, edgeImage, 100, 200, 3);

  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  RNG rng(12345);
  findContours(grayImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));

  Mat drawnImage = Mat::zeros(grayImage.size(), CV_8UC3);
  for(int i=0; i<contours.size(); i++)
  {
  	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
  	drawContours(drawnImage, contours, i, color, CV_FILLED, 8, hierarchy);
  }

  namedWindow("Contours Image", WINDOW_NORMAL);
  imshow("Contours Image", drawnImage);
  waitKey(0);
}

// double a = contourArea(controus[i], false);
//   	if(a>largest_area){
//   		largest_area = 
//   	}