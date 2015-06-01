#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cassert>
#include "/home/swift/code/customfunctions/DisplayFrames.h"
#include "/home/swift/code/customfunctions/GetProjectionMatrix.h"
#include "/home/swift/code/customfunctions/ExcludeColour.h"
#include "/home/swift/code/customfunctions/OutputFile.h"
#include "/home/swift/code/customfunctions/VoxelStruct.h"

#define DEBUG 1

#define ELEMENT_TYPE 2
#define OPENING_SIZE 2
#define CLOSING_SIZE 2

//Colours used in image analysis.
#define WHITE 255

#define VOXEL_DISC 400
#define VOXEL_PIXEL_RATIO 800

//Size of the images when displayed.
#define WINDOWX 500
#define WINDOWY 400

using namespace std;
using namespace cv;

// #ifndef voxel_struct
// #define voxel_struct
// Voxel Struct
// struct Voxel
// {
// 	float x, y, z;
// 	float depth; // used to decide which camera colours the voxel
// 	unsigned char r, g, b;
// 	unsigned char camIndex;
// };
// #endif

int main()
{
	char* filename = (char *)"VoxelDino.ply";
	vector<Mat> images;
	vector<Mat> projections;
	vector<Mat> masks;
	vector<Voxel> voxelGrid;
	Mat X(3, 1, CV_32F);
	Mat voxelPosition(4, 1, CV_32F);
	Point3f voxelOrigin(-200, -200, -200);

	int xdiv = VOXEL_DISC; // voxel discretization
	int ydiv = VOXEL_DISC;
	int zdiv = VOXEL_DISC;

	//Size of the voxels
	float voxelSize = VOXEL_PIXEL_RATIO / xdiv;

	//Gets projection matrix from Dino Projection matricies.txt, I wrote this function
	projections = getProjectionMatrix((char *)"Dino projection matrices.txt");

	// Initialize the grid of voxels
	voxelGrid.resize(xdiv * ydiv * zdiv);

	int k = 0;
	for(int x=0; x < xdiv; x++) {
		for(int y=0; y < ydiv; y++) {
			for(int z=0; z < zdiv; z++) {

				//Effectively creating all of the voxels as transparent voxels, where the camera that will colour them is unselected
				voxelGrid[k].x = voxelOrigin.x + x*voxelSize;
				voxelGrid[k].y = voxelOrigin.y + y*voxelSize;
				voxelGrid[k].z = voxelOrigin.z + z*voxelSize;
				voxelGrid[k].depth = FLT_MAX;
				k++;
			}
		}
	}

	for(size_t k=0; k < projections.size(); k++) {
		char str[128];
		Mat image, maskImage;
		vector<Voxel> detectedVoxels;

		sprintf(str, "dino%02d.jpg", (int)k);
		cout << "Loading " <<  str << endl;

		image = imread(string("visualize/") + str); assert(image.data);

		maskImage = Mat::zeros(image.rows, image.cols, CV_8UC1);

		excludeColour(image, maskImage, 0);

		// dilate to fill in any holes in the dinosaur
		dilate(maskImage, maskImage, Mat());

		images.push_back(image);
		masks.push_back(maskImage);

		Mat &P = projections[k];

		// Visibility testing
		for(size_t i=0; i < voxelGrid.size(); i++) {
			Voxel &currentVoxel = voxelGrid[i];

			voxelPosition.at<float>(0) = currentVoxel.x;
			voxelPosition.at<float>(1) = currentVoxel.y;
			voxelPosition.at<float>(2) = currentVoxel.z;
			voxelPosition.at<float>(3) = 1.0;

			X = P * voxelPosition;

			int x = X.at<float>(0) / X.at<float>(2);
			int y = X.at<float>(1) / X.at<float>(2);

			if(x < 0 || x >= maskImage.cols || y < 0 || y >= maskImage.rows) {
				continue;
			}

			if(maskImage.at<uchar>(y,x)) {
				if(X.at<float>(2) < currentVoxel.depth) {
					currentVoxel.depth = X.at<float>(2);

					currentVoxel.r = image.at<Vec3b>(y,x)[2];
					currentVoxel.g = image.at<Vec3b>(y,x)[1];
					currentVoxel.b = image.at<Vec3b>(y,x)[0];

					currentVoxel.camIndex = k;
				}

				detectedVoxels.push_back(currentVoxel);
			}
		}

		voxelGrid = detectedVoxels;
	}

	cout << "Refining voxels ... " << endl;

	//Reduce Voxel size
	int sub_division = 2;
	voxelSize /= sub_division;

	vector<Voxel> new_voxels;

	for(size_t i=0; i < voxelGrid.size(); i++) {
		for(int x=1; x < sub_division; x++) {
			for(int y=1; y < sub_division; y++) {
				for(int z=1; z < sub_division; z++) {
					Voxel v = voxelGrid[i];

					v.x = v.x + x*voxelSize;
					v.y = v.y + y*voxelSize;
					v.z = v.z + z*voxelSize;

					v.depth = FLT_MAX;

					new_voxels.push_back(v);
				}
			}
		}
	}


	// Output to Meshlab readable file
	cout << "Saving to " << filename << endl;

	outputMeshlab(voxelGrid, new_voxels, filename);
}
