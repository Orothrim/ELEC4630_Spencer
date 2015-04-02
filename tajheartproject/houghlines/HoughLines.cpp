#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdio.h>
#include <opencv2/opencv.hpp>

#include <iostream>

#define MIN_GRAD 0.3
#define MAX_GRAD 0.5

//#define START_LOWER 1400
#define START_UPPER 1300

using namespace cv;
using namespace std;

int thres = 100;
int linelength = 150;
int linegap = 20;
float xstart, xend, ystart, yend, grad;

char writename[40] = "t100min150max20a22b10.jpg\0"; 

void help()
{
  cout << "\nThis program demonstrates line finding with the Hough transform.\n"
         "Usage:\n"
         "./houghlines <image_name>, Default is pic1.jpg\n" << endl;
}

int main(int argc, char** argv)
{
  const char* filename = argc >= 2 ? argv[1] : "taj.jpg";

  Mat originalimage = imread(filename);
  if(originalimage.empty())
  {
    help();
    cout << "can not open " << filename << endl;
    return -1;
  }

  Mat edgeimage, lineimage;
  Mat grayimage = imread(filename, 0);

  // originalimage.copyTo(grayimage);
  // cvtColor( grayimage, grayimage, CV_BGR2GRAY);
  grayimage.convertTo(edgeimage, -1, 2.2, 10);
  Canny(edgeimage, edgeimage, 50, 200, 3);
  cvtColor(edgeimage, lineimage, CV_GRAY2BGR);
  

  
  vector<Vec4i> lines;
  HoughLinesP(edgeimage, lines, 1, CV_PI/180, thres, linelength, linegap);
  for( size_t i = 0; i < lines.size(); i++ )
  {
    Vec4i l = lines[i];

    if (l[1] < l[3]){
      xstart = l[0];
      ystart = l[1];
      xend = l[2];
      yend = l[3];

      grad = ((yend-ystart)/(xend-xstart));

      if (MIN_GRAD < grad && grad < MAX_GRAD && ystart > START_UPPER) {

        //cout << "\n Line Gradient: "<< grad <<"\n";
        line( originalimage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
      }
    }
  }
  
  imwrite(writename, originalimage);

  //namedWindow("Edge Image", WINDOW_NORMAL);
  //imshow("Edge Image", edgeimage);
  namedWindow("detected lines", WINDOW_NORMAL);
  imshow("detected lines", originalimage);

  waitKey();

  return 0;
}