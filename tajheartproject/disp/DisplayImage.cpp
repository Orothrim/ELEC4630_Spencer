#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <cv.h>
#include <highgui.h>

using namespace cv;

int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    Mat image;
    image = imread( argv[1], 1 );

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }

    Mat gray_image;
    cvtColor( image, gray_image, CV_BGR2GRAY);

    imwrite("Gray_Image1.jpg", gray_image);

    namedWindow("Display Image", WINDOW_AUTOSIZE );
    imshow("Display Image", image);

    namedWindow("Gray_Image", CV_WINDOW_AUTOSIZE);
    imshow("Gray_Image", gray_image);

    waitKey(0);

    return 0;
}