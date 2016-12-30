

#include "lfinterpolate.h"

Mat lfinterpolate::interp(Mat rvolume, Mat dvolume, int grain)
{
	int height = rvolume.size[0]; int width = rvolume.size[1]; int num_images = rvolume.size[2];
	int sizes[] = { height, width, (num_images * grain) };

	Mat rinterpvolume = cv::Mat(3, sizes, CV_8UC3, Scalar::all(0));
	int disp = 0; cv::Vec3b pixvalue; float iter = 1.0; float u_; int g = 0;

	for (int i = 0; i < num_images * 10; i++)
	{
		for (int j = 0; j < height; j++)
		{
			for (int k = 0; k < width; k++)
			{
				if (i % 10 == 0)
				{
					iter = 1.0;
					rinterpvolume.at<cv::Vec3b>(j, k, i) = rvolume.at<cv::Vec3b>(j, k, (i / 10));
					disp = (int)(dvolume.at<uchar>(j, k, (i / 10)));
					u_ = k;
				}
				else
				{
					u_ = (float)k - iter*((float)disp/(255.0*25.0));
					if (u_ >= 0)
						rinterpvolume.at<cv::Vec3b>(j, (int)u_, i) = rvolume.at<cv::Vec3b>(j, k, (i / 10));
					iter += 0.0001f;
				}
			}
		}
	}
	return rinterpvolume;


}