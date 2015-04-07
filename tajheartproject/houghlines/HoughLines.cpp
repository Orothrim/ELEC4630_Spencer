#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>

#define MIN_GRAD 0.3
#define MAX_GRAD 0.5

#define END_UPPER 1550
#define START_UPPER 1300

using namespace cv;
using namespace std;

int thres = 100;
int linelength = 300;
int linegap = 20;
float alpha = 2.2;
int beta = 10;
float xstart, xend, ystart, yend, grad;

char writename[40] = "t100min300max20a22b10v3.jpg\0"; 

void help()
{
  cout << "\nThis program demonstrates line finding with the Hough transform.\n"
         "Usage:\n"
         "./houghlines <image_name>, Default is pic1.jpg\n" << endl;
}

int main(int argc, char** argv)
{
  const char* filename = argc >= 2 ? argv[1] : "taj.jpg";

  Mat originalImage = imread(filename);
  if(originalImage.empty())
  {
    help();
    cout << "can not open " << filename << endl;
    return -1;
  }

  Mat edgeImage, lineImage, binaryImage, contrastImage;
  Mat grayImage = imread(filename, 0);

  

  grayImage.convertTo(contrastImage, -1, 2.2, 10);
  Canny(contrastImage, edgeImage, 50, 200, 3);
  cvtColor(edgeImage, lineImage, CV_GRAY2BGR);
  

  
  vector<Vec4i> lines;
  HoughLinesP(edgeImage, lines, 1, CV_PI/180, thres, linelength, linegap);
  for( size_t i = 0; i < lines.size(); i++ )
  {
    Vec4i l = lines[i];

    if (l[1] < l[3]){
      xstart = l[0];
      ystart = l[1];
      xend = l[2];
      yend = l[3];

      grad = ((yend-ystart)/(xend-xstart));

      if (MIN_GRAD < grad && grad < MAX_GRAD && ystart > START_UPPER && yend > END_UPPER) {
        cout << "\n xstart "<< xstart <<" xend "<< xend << " ystart " << ystart << " yend " << yend  <<"\n";
        line( originalImage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
      }
    }
  }
  
  imwrite(writename, originalImage);

  namedWindow("Edge Image", WINDOW_NORMAL);
  imshow("Edge Image", edgeImage);
  namedWindow("detected lines", WINDOW_NORMAL);
  imshow("detected lines", originalImage);

  waitKey();

  return 0;
}