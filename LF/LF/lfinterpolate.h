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

using namespace std;
using namespace cv;

//for interpolating and synthesising more images from a light field with a lesser number of miages.

class lfinterpolate
{
	private:
		Mat interplf;

	public:

		Mat interp(Mat rvolume, Mat dvolume, int grain);
};