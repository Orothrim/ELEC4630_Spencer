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

  //If no argument is supplied to the program it uses 'taj.jpg'.
  const char* filename = argc >= 2 ? argv[1] : "taj.jpg";

  //Reads in the image, or prints "can not open" and ends the program.
  Mat originalImage = imread(filename);
  if(originalImage.empty()) {
    help();
    cout << "can not open " << filename << endl;
    return -1;
  }

  //Mat files for holding different versions of the image, for debugging.
  Mat edgeImage, lineImage, binaryImage, contrastImage;
  
  //Reads the image as a grayscale image
  Mat grayImage = imread(filename, 0);

  //Contrasts the grayImage in order to bring the lines out more.
  grayImage.convertTo(contrastImage, -1, ALPHA, BETA);

  //Performs canny edge detection on the contrasted image.
  Canny(contrastImage, edgeImage, 50, 200, 3);

  //Turns the image into a form that HoughLinesP can operate on.
  cvtColor(edgeImage, lineImage, CV_GRAY2BGR);
  
  //Vector of 4 value vectors for containing the lines.
  vector<Vec4i> lines;

  //Performs the houghlines transformation on the image, returning the start pixel 
  //and end pixel of each line.
  HoughLinesP(edgeImage, lines, 1, CV_PI/180, THRES, LINELENGHT, LINEGAP);
  
  //Runs through each of the lines to find ones that suit the requirements for
  //the project.
  for( size_t i = 0; i < lines.size(); i++ ) {
    Vec4i l = lines[i];

    if (l[1] < l[3]){
      xStart = l[0];
      yStart = l[1];
      xEnd = l[2];
      yEnd = l[3];

      //Finds the equation of the current line, so as to compare it to the 
      //required lines.
      grad = ((yEnd-yStart)/(xEnd-xStart));
      yCept = -1*xStart*grad + yStart;

      //The gradients of the required 3 lines are very similar, thus if the line
      //has a very different gradient it is dismissed.
      if (MIN_GRAD < grad && grad < MAX_GRAD) {

        //We want to find the lowest portion of the line identified by houghlinesP
        //and then extend the line to a certain height.
        if (yEnd > END_UPPER) {

          //We don't want overlayed lines, thus any lines with the same/close Y
          //intercept as a previously selected line gets dismissed.
          if (!(yCept > lineCepts[0] - Y_RANGE && yCept < lineCepts[0] + Y_RANGE) && 
            !(yCept > lineCepts[1] - Y_RANGE && yCept < lineCepts[1] + Y_RANGE) && 
            !(yCept > lineCepts[2] - Y_RANGE && yCept < lineCepts[2] + Y_RANGE)) {
            
            //Debugging information, only printed if DEBUG equals 1.
            if (DEBUG) {cout << "\n Gradient: "<< grad <<" Y Intercept: "<< yCept <<" yStart: "<< yStart <<"\n";}
            
            //Draws the line onto the image, extending it to the end of the water feature.
            line( originalImage, Point((Y_CEILING - yCept)/grad, Y_CEILING), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
            lineCepts[lineCount] = yCept;
            lineGrads[lineCount] = grad;
            lineCount++;
          }
        }
      }
    }
  }
  
  
  //imwrite(writename, originalImage);

  if (DEBUG) {
    namedWindow("Edge Image", WINDOW_NORMAL);
    imshow("Edge Image", edgeImage);
    resizeWindow("Edge Image", 1000, 750);
    moveWindow("Edge Image", 0, 0);
  }

  namedWindow("Detected Lines", WINDOW_NORMAL);
  imshow("Detected Lines", originalImage);
  resizeWindow("Detected Lines", 1000, 750);
  moveWindow("Detected Lines", 1005, 0);


  waitKey();

  return 0;
}