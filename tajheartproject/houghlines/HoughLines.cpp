#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdio.h>
#include <opencv2/opencv.hpp>

#include <iostream>

using namespace cv;
using namespace std;

int thres = 100;
int linelength = 150;
int linegap = 20;

char writename[25] = "t100min150max20a22b10"; 

void help()
{
  cout << "\nThis program demonstrates line finding with the Hough transform.\n"
         "Usage:\n"
         "./houghlines <image_name>, Default is pic1.jpg\n" << endl;
}

int main(int argc, char** argv)
{
  const char* filename = argc >= 2 ? argv[1] : "taj.jpg";

  Mat originalimage = imread(filename, 0);
  if(originalimage.empty())
  {
    help();
    cout << "can not open " << filename << endl;
    return -1;
  }

  Mat edgeimage, lineimage;
  originalimage.convertTo(edgeimage, -1, 2.2, 10);
  Canny(edgeimage, edgeimage, 50, 200, 3);
  cvtColor(edgeimage, lineimage, CV_GRAY2BGR);
  

  #if 0
    vector<Vec2f> lines;
    HoughLines(edgeimage, lines, 1, CV_PI/180, 100, 0, 0 );

    for( size_t i = 0; i < lines.size(); i++ )
    {
      float rho = lines[i][0], theta = lines[i][1];
      Point pt1, pt2;
      double a = cos(theta), b = sin(theta);
      double x0 = a*rho, y0 = b*rho;
      pt1.x = cvRound(x0 + 1000*(-b));
      pt1.y = cvRound(y0 + 1000*(a));
      pt2.x = cvRound(x0 - 1000*(-b));
      pt2.y = cvRound(y0 - 1000*(a));
      line( lineimage, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
    }
  #else
    vector<Vec4i> lines;
    HoughLinesP(edgeimage, lines, 1, CV_PI/180, thres, linelength, linegap);
    for( size_t i = 0; i < lines.size(); i++ )
    {
      Vec4i l = lines[i];
      line( lineimage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
    }
  #endif


  //strcpy(filename, "t%umin%umax%u.jpg\0",thres, linelength, linegap);

  imwrite(writename, lineimage);

  namedWindow("Edge Image", WINDOW_NORMAL);
  imshow("Edge Image", edgeimage);
  namedWindow("detected lines", WINDOW_NORMAL);
  imshow("detected lines", lineimage);

  waitKey();

  return 0;
}