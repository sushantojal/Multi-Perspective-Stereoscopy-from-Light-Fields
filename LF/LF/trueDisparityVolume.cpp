
#include"trueDisparityVolume.h"
using namespace cv;

trueDisparityVolume::trueDisparityVolume()
{

}

void trueDisparityVolume::createTrueDisparityVolumeImages()
{
	int width, height, channels;
	int num_images = 50;				//current dataset has 50 images; can be increased
	float baseline_count = 0;


	/*Scaled values of focal length and baseline of cameras used to generate datasets.
	Actual f value = 50mm (7800 pixels); actual baseline value = 10mm
	These values are scaled down to accomodate a larger disparity range
	True disparity = focal length * baseline * reciprocal value of depth of pixel
	This true disparity volume is constructed with respect to the first normalised disparity image.
	*/

	for (int filenumber = 25; filenumber <= 75; filenumber++)
	{
		string filepath = "output/disparityImages/disparity_00" + to_string(filenumber) + ".bmp";
		Mat image;
		image = imread(filepath, IMREAD_UNCHANGED);
		width = image.cols;
		height = image.rows;
		Mat tDispImage = Mat::zeros(height, width, CV_8UC1);
		cv::multiply(cv::Scalar::all(baseline_count), image, tDispImage);
		string outfilepath = "output/trueDisparityImages/true_disparity_00" + to_string(filenumber) + ".bmp";
		imwrite(outfilepath, tDispImage);
		baseline_count += 0.025f;
	}
}

void trueDisparityVolume::generateEpipolarPlane(int u, Mat tdvolume)
{
	int num_images = tdvolume.size[2]; int width = tdvolume.size[1]; Mat testCut;
	if (tdvolume.type() != 16)		
	{
		testCut = Mat::zeros(num_images, width, CV_8UC1);

		for (int i = 0; i < num_images; i++)
		{
			for (int j = 0; j < width; j++)
			{
				testCut.at<uchar>(i, j) = tdvolume.at<uchar>(u, j, i);
			}
		}
	}
	else							//for 3channel image volumes
	{
		testCut = Mat::zeros(num_images, width, CV_8UC3);

		for (int i = 0; i < num_images; i++)
		{
			for (int j = 0; j < width; j++)
			{
				testCut.at<cv::Vec3b>(i, j) = tdvolume.at<cv::Vec3b>(u, j, i);
			}
		}
	}

	namedWindow("testcut", WINDOW_NORMAL);
	setWindowProperty("testcut", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	imshow("testcut", GetSquareImage(testCut, 1000));
	//imshow("testcut", testCut);
	waitKey(0);
}

Mat trueDisparityVolume::GetSquareImage( const cv::Mat& img, int target_width = 1000 )
{
    int width = img.cols,
       height = img.rows;

    cv::Mat square = cv::Mat::zeros( target_width, target_width, img.type() );

    int max_dim = ( width >= height ) ? width : height;
    float scale = ( ( float ) target_width ) / max_dim;
    cv::Rect roi;
    if ( width >= height )
    {
        roi.width = target_width;
        roi.x = 0;
        roi.height = height * scale * 3;
        roi.y = ( target_width - roi.height ) / 2;
    }
    else
    {
        roi.y = 0;
        roi.height = target_width * 3;
        roi.width = width * scale;
        roi.x = ( target_width - roi.width ) / 2;
    }

    cv::resize( img, square( roi ), roi.size() );

    return square;
}

Mat trueDisparityVolume::createTrueDisparityVolume()
{
	int num_images = 50;
	Mat temp = imread("output/trueDisparityImages/true_disparity_0025.bmp", IMREAD_UNCHANGED);
	int width = temp.cols; int height = temp.rows;

	int sizes[] = { height, width, num_images };
	cv::Mat tdvolume = cv::Mat(3, sizes, CV_8UC1, Scalar::all(0));
	int filenumber = 25;

	for (int i = 0; i < num_images; i++)
	{
		string filepath = "output/trueDisparityImages/true_disparity_00" + to_string(filenumber) + ".bmp";
		Mat image;
		image = imread(filepath, IMREAD_UNCHANGED);
		for (int j = 0; j < height; j++)
		{
			for (int k = 0; k < width; k++)
			{
				tdvolume.at<uchar>(j, k, i) = image.at<uchar>(j, k);
			}
		}
		filenumber++;
	}

	//test tdvolume by creating a cut at any random depth
	/*int cut = 25;
	Mat testCut = Mat::zeros(height, width, CV_8UC1);
	cout << testCut.size[0] << endl;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			testCut.at<uchar>(i, j) = tdvolume.at<uchar>(i, j, cut);
		}
	}
	namedWindow("testcut", WINDOW_AUTOSIZE);
	imshow("testcut", testCut);
	waitKey(0);*/

	return tdvolume;
}

Mat trueDisparityVolume::generateTsGVolume(Mat goaldisp, Mat tdvolume)
{

	int num_images = 50;
	Mat temp = imread("output/trueDisparityImages/true_disparity_0025.bmp", IMREAD_UNCHANGED);
	int width = temp.cols; int height = temp.rows;

	int sizes[] = { height, width, num_images };
	cv::Mat tsgvolume = cv::Mat(3, sizes, CV_8UC1, Scalar::all(0));

	for (int i = 0; i < num_images; i++)
	{
		for (int j = 0; j < height; j++)
		{
			for (int k = 0; k < width; k++)
			{
				tsgvolume.at<uchar>(j, k, i) = (uchar)(abs( (int)tdvolume.at<uchar>(j, k, i) - (int)goaldisp.at<uchar>(j, k) ));
			}
		}
	}
	return tsgvolume;
}

Mat trueDisparityVolume::gettdvolume()
{
	return tdvolume;
}

Mat trueDisparityVolume::gettsgvolume()
{
	return tsgvolume;
}

