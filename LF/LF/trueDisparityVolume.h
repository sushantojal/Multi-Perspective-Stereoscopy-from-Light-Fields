#pragma once
#include <cstdio>
#include <fstream>
#include <string>
#include <cstring>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <math.h>

using namespace cv;
using namespace std;


//represents the true disparity volume
class trueDisparityVolume
{

private:

	//the difference of the true disparity volume with the goal disparity map mat.
	Mat tsgvolume;

	//the true disparity volume mat
	Mat tdvolume;

public:
	trueDisparityVolume();

	//returns the tsg volume
	Mat gettsgvolume();

	//returns the tdvolume
	Mat gettdvolume();

	//create true disparity volume images and write them to file
	void createTrueDisparityVolumeImages();

	//generate the epiploar plane for a volume along the v-axis at 'u'. 
	void generateEpipolarPlane(int u, Mat tdvolume);

	//create and return the true disparity volume from the generated true disparity images
	Mat createTrueDisparityVolume();

	//generate and return the volume which has the difference of true disparity images with the goal disparity map
	Mat generateTsGVolume(Mat goaldisp, Mat tdvolume);

	//helper function to view output
	Mat trueDisparityVolume::GetSquareImage(const cv::Mat& img, int target_width );


};