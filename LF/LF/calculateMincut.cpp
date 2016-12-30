
#include "calculateMincut.h"

#define PMAX 3.0
#define LAMBDA 0.5

Mat calculateMincut(Mat gsvolume, Mat dvolume, Mat tsgvolume, Mat rvolume)
{
	int y;
	int height = dvolume.size[0]; int width = dvolume.size[1]; int num_images = dvolume.size[2];
	int n_nodes = height * width * num_images;
	int n_nodes_exlast = height * width * (num_images-1); 

	typedef Graph<int, int, int> GraphType;
	GraphType *g = new GraphType(/*estimated # of nodes*/ n_nodes, /*estimated # of edges*/ n_nodes * 5);

	
	//add every pixel of the light field as a node to the graph
	for (int i = 0; i < n_nodes; i++)
		g->add_node();

	//add t_weights only for the first and last images of the volume
	for (int i = 0; i < width*height; i++)
		g->add_tweights(i, INF, 0);

	for ( int i = n_nodes_exlast - 1; i < n_nodes; i ++ )
		g->add_tweights(i, 0, INF);


	/*add n_weights; includes:
							true disparity - goal disparity term along the 's' axis
							smoothness terms along the 'u' and the 'v' axes*/

	int node_num = 0;
	for (int i = 0; i < num_images - 1; i++)
	{
		for (int j = 0; j < height; j++)
		{
			for (int k = 0; k < width; k++)
			{
				double smoothing_factor = std::max(PMAX, std::max((double)(gsvolume.at<uchar>(j, k, i)), (LAMBDA*dvolume.at<uchar>(j, k, i))));
				g->add_edge(node_num, node_num + (width * height),    /* capacities */  ((int)(tsgvolume.at<uchar>(j,k,i))), 0);

				//add all the edges for the smoothening energy parameters along the 'u' and 'v' directions.
				
				if ( k <= width - 2 )
					g->add_edge(node_num, node_num + 1, 
						(int)smoothing_factor, 0);

				if (k >= 1)
					g->add_edge(node_num, node_num -1,
						(int)smoothing_factor, 0);

				if ( j <= height - 2)
					g->add_edge(node_num, node_num + width,
						(int)smoothing_factor, 0);

				if ( j >= 1 )
					g->add_edge(node_num, node_num - width, 
						(int)smoothing_factor, 0);

				node_num++;
			}
		}
	}
	cout << "num_nodes = " << node_num << endl;


	//compute the maxflow across the tsg volume

	int flow = g->maxflow();
	printf("Flow = %d\n", flow);
	
	int ** indexarr = new int *[height];
	for (int i = 0; i < height; i++)
		indexarr[i] = new int[width];


	//to find out the first occurence where the node type changes from source to sink; ie, the node which is part of the mincut
	//iterate over the volume width wise across all the images across all the rows, save mincut image node to indexarr
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			for (int k = 0; k < num_images; k++)
			{
				int node_ind = (k * width*height) + (i * width) + j;
				if (g->what_segment(node_ind) == GraphType::SINK)
				{
					indexarr[i][j] = k - 1;
					break;
				}
			}
		}
	}

	Mat outimage = Mat::zeros(height, width, CV_8UC3);

	//populate the image mat to be returned with the pixel value of nodes making up the mincut.
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			outimage.at<cv::Vec3b>(i, j) = rvolume.at<cv::Vec3b>(i, j, indexarr[i][j]);
		}
	}

	return outimage;
}


