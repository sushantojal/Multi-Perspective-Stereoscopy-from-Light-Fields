#pragma once
#include <cstdio>
#include <fstream>
#include <string>
#include <cstring>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>

using namespace cv;
using namespace std;


//generate the normalised disparity volume from the given set of depth maps

class disparityVolume
{
	private:
		Mat dvolume;


	public:
		disparityVolume();

		//create normalised disparity images from depth maps and write to file
		void createDisparityVolumeImages();

		//create and return a normalised 3d volume from the created disparity images
		Mat createDisparityVolume();

		//create and return a volume of maps in which every node has a value equal to its gradient alone the s-axis.
		Mat gradientInSVolume();
		
};
