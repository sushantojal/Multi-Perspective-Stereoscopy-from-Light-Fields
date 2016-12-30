#pragma once
#include <stdio.h>
#include "graph.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <algorithm>
using namespace std;
using namespace cv;

#define INF 100000
//
//input: 
//		gsvolume: volume containing gradient along the s axis for every node/pixel
//		dvolume: normalised disparity volume 
//		tsgvolume: difference of true disparity volume with the goal disparty map
//		rvolume: volume of raw input images
//
//output:	
//		image which has the closest approximation to the desired disparity map
//

Mat calculateMincut(Mat gsvolume, Mat dvolume, Mat tsgvolume, Mat rvolume);