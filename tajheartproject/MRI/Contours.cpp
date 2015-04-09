#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"

#define DEBUG 1


using namespace cv;
using namespace std;

Point mousePos;
int i;

void onMouse(int event, int x, int y, int flags, void* userdata)
{
	if(event == EVENT_LBUTTONDOWN)
	{
		mousePos.x = x;
		mousePos.y = y;
	}
}


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

	int thres = 80;
  Mat image = originalImage, pyr;
  Mat edgeImage, contrastImage, blurImage;
  Mat drawnImage = Mat::zeros(originalImage.size(), CV_8UC3);

  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  RNG rng(12345);

	// cout << "Would you like to center the search area? [y/n]\n\r";
	// cin >> i;
	// if(i=="y"){
	// 	namedWindow("Click Image", WINDOW_NORMAL);
	//   imshow("Click Image", image);
	//   setMouseCallback("Click Image", onMouse, NULL)
	//   cout << mousePos;
	// }

  // pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
  // pyrUp(pyr, image, image.size());
	image.convertTo(contrastImage, -1, 2.2, 10);
  
  Canny(contrastImage, edgeImage, thres, 2*thres, 3);
  blur(edgeImage, blurImage, Size(7,7));

  
	if (DEBUG) {cout << "Edge Image Created\n\r";}

  //threshold(image, binaryImage, 50, 255, THRESH_BINARY);

  findContours(blurImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
	if (DEBUG) {cout << "Contours Found\n\r";}

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
	  namedWindow("Blur Image", WINDOW_NORMAL);
	  imshow("Blur Image", blurImage); 
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