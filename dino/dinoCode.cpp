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

#define DEBUG 0

#define ELEMENT_TYPE 2
#define OPENING_SIZE 2
#define CLOSING_SIZE 2

//Colours used in image analysis.
#define WHITE 255

#define VOXEL_DISC 400
#define VOXEL_PIXEL_RATIO 800

using namespace std;
using namespace cv;

int main()
{
	char* filename = (char *)"VoxelDino.ply";
	vector<Mat> images;
	vector<Mat> projections;
	vector<Mat> masks;
	vector<Voxel> voxelGrid;
	Mat X(3, 1, CV_32F), voxelPosition(4, 1, CV_32F), displayImage;
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
		char readname[128];
		Mat image, maskImage;

		//detectedVoxels are used to determine which voxels are to be 'cut' and which are not.
		vector<Voxel> detectedVoxels;

		//Changes the file name for the current image
		sprintf(readname, "dino%02d.jpg", (int)k);
		cout << "Loading " <<  readname << endl;

		//Used to retrieve the image from the file
		image = imread(readname);

		if(image.empty()) {
			//Checks that the file can be opened, if it can't, prints "can not open" 
			//and end the program
			cout << "can not open " << readname << endl;
			return -1;
		}
		#if DEBUG
		cout << "File Loaded\n\r";
		#endif

		//The pixel needs to be accessable when being written, thus the mask is made to have the same size as the image.
		maskImage = Mat::zeros(image.rows, image.cols, CV_8UC1);

		//Self written function, used to set to zero any pixels which are more blue than they are red or green.
		excludeColour(image, maskImage, BLUE, 1);

		// dilate to fill in any holes in the dinosaur
		dilate(maskImage, maskImage, Mat());

		images.push_back(image);
		masks.push_back(maskImage);

		Mat imageProj = projections[k];

		// Visibility testing
		for(size_t i=0; i < voxelGrid.size(); i++) {
			Voxel &currentVoxel = voxelGrid[i];

			voxelPosition.at<float>(0) = currentVoxel.x;
			voxelPosition.at<float>(1) = currentVoxel.y;
			voxelPosition.at<float>(2) = currentVoxel.z;
			voxelPosition.at<float>(3) = 1.0;

			X = imageProj * voxelPosition;

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
