
#include <cstdio>
#include <fstream>
#include <string>
#include "disparityVolume.h"

disparityVolume::disparityVolume()
{

}

void disparityVolume::createDisparityVolumeImages()
{

	int width, height, channels;
	int num_images = 50;
	float scale_factor = 25;
	float baseline_factor = 0;

	for (int filenumber = 25; filenumber <= 75; filenumber++)
	{
		string filepath = "dataset2/mansion_depth/mansion_depth_00" + to_string(filenumber) + ".png";
		Mat image;
		image = imread(filepath, IMREAD_UNCHANGED);
		width = image.cols;
		height = image.rows;

		Mat dispImage = Mat::zeros(height, width, CV_8UC1);
		cv::subtract(cv::Scalar::all(255), image, dispImage);

		cv::divide(cv::Scalar::all(255), dispImage, dispImage);
		cv::multiply(cv::Scalar::all(25), dispImage, dispImage);

		string outfilepath = "output/disparityImages/disparity_00" + to_string(filenumber) + ".bmp";
		imwrite(outfilepath, dispImage);

	}
}

Mat disparityVolume::createDisparityVolume()
{
	int num_images = 50;
	Mat temp = imread("output/disparityImages/disparity_0025.bmp", IMREAD_UNCHANGED);
	int width = temp.cols; int height = temp.rows;

	int sizes[] = { height, width, num_images };
	this->dvolume = cv::Mat(3, sizes, CV_8UC1, Scalar::all(0));
	int filenumber = 25;

	for (int i = 0; i < num_images; i++)
	{
		string filepath = "output/disparityImages/disparity_00" + to_string(filenumber) + ".bmp";
		Mat image;
		image = imread(filepath, IMREAD_UNCHANGED);
		for (int j = 0; j < height; j++)
		{
			for (int k = 0; k < width; k++)
			{
				dvolume.at<uchar>(j, k, i) = image.at<uchar>(j, k);
			}
		}
		filenumber++;
	}

	//test tdvolume by creating a cut at any random depth, similar to generating epipolar plane

	//int cut = 25;
	//Mat testCut = Mat::zeros(height, width, CV_8UC1);
	//cout << testCut.size[0] << endl;
	//for (int i = 0; i < height; i++)
	//{
	//for (int j = 0; j < width; j++)
	//{
	//testCut.at<uchar>(i, j) = dvolume.at<uchar>(i, j, cut);
	//}
	//}
	//namedWindow("testcut", WINDOW_AUTOSIZE);
	//imshow("testcut", testCut);
	//waitKey(0);
	return dvolume;
}


Mat disparityVolume::gradientInSVolume()
{
	Mat gsVolume = dvolume;

	int height = gsVolume.size[0]; int width = gsVolume.size[1]; int num_images = gsVolume.size[2];

	for (int i = 0; i < num_images - 1; i++)
	{
		for (int j = 0; j < height; j++)
		{
			for (int k = 0; k < width; k++)
			{
				gsVolume.at<uchar>(j, k, i) = abs(gsVolume.at<uchar>(j, k, i) - gsVolume.at<uchar>(j, k, i + 1));
			}
		}
	}

	return gsVolume;
}