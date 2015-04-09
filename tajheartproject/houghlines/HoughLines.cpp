#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>

#define MIN_GRAD 0.3
#define MAX_GRAD 0.5

#define END_UPPER 1550

#define Y_RANGE 42
#define THRES 100
#define LINELENGHT 150
#define LINEGAP 20
#define ALPHA 2.2
#define BETA 10
#define Y_CEILING 1300

#define DEBUG 1


using namespace cv;
using namespace std;

float xStart, xEnd, yStart, yEnd, grad;
int lineCepts[3], lineGrads[3], yCeiling, yCept, lineCount = 0;

char writename[40] = "t100min150max20a22b10.jpg\0"; 

void help() {
  cout << "\nThis program demonstrates line finding with the Hough transform.\n"
         "Usage:\n"
         "./houghlines <image_name>, Default is pic1.jpg\n" << endl;
}

int main(int argc, char** argv) {
  const char* filename = argc >= 2 ? argv[1] : "taj.jpg";

  Mat originalImage = imread(filename);
  if(originalImage.empty()) {
    help();
    cout << "can not open " << filename << endl;
    return -1;
  }

  Mat edgeImage, lineImage, binaryImage, contrastImage;
  Mat grayImage = imread(filename, 0);

  

  grayImage.convertTo(contrastImage, -1, ALPHA, BETA);
  Canny(contrastImage, edgeImage, 50, 200, 3);
  cvtColor(edgeImage, lineImage, CV_GRAY2BGR);
  

  
  vector<Vec4i> lines;
  HoughLinesP(edgeImage, lines, 1, CV_PI/180, THRES, LINELENGHT, LINEGAP);
  for( size_t i = 0; i < lines.size(); i++ ) {
    Vec4i l = lines[i];

    if (l[1] < l[3]){
      xStart = l[0];
      yStart = l[1];
      xEnd = l[2];
      yEnd = l[3];

      grad = ((yEnd-yStart)/(xEnd-xStart));
      yCept = -1*xStart*grad + yStart;

      if (MIN_GRAD < grad && grad < MAX_GRAD) {
        if (yEnd > END_UPPER) {
          if (!(yCept > lineCepts[0] - Y_RANGE && yCept < lineCepts[0] + Y_RANGE) && !(yCept > lineCepts[1] - Y_RANGE && yCept < lineCepts[1] + Y_RANGE) && !(yCept > lineCepts[2] - Y_RANGE && yCept < lineCepts[2] + Y_RANGE)){
            if (DEBUG) {cout << "\n Gradient: "<< grad <<" Y Intercept: "<< yCept <<" yStart: "<< yStart <<"\n";}
            
            line( originalImage, Point((Y_CEILING - yCept)/grad, Y_CEILING), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
            lineCepts[lineCount] = yCept;
            lineGrads[lineCount] = grad;
            lineCount++;
          }
        }
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