
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "stb-master\stb.h"
#include "stb-master\stb_image.h"
#include "stb-master\stb_image_write.h"
#include <GL/glew.h>
#include <GL/glfw3.h>
#include <gl/glut.h>
#include <string>
#include <iostream>
#include <cstdio>
#include <list>
#include "trueDisparityVolume.h"
#include "disparityVolume.h"
#include "calculateMincut.h"
#include "lfinterpolate.h"

using namespace std;
using namespace cv;

string vertexShader = string("#version 150\n"
	"in vec3 position;\n"
	"in vec3 color;\n"
	"in vec2 texCoord;\n"
	"out vec3 fragColor;\n"
	"out vec2 TexCoord;\n"
	"void main()\n"
	"{\n"
	"gl_Position = vec4(position, 1.0);\n"
	"fragColor = color;\n"
	"TexCoord = texCoord;\n"
	"}\n");

string fragmentShader = string("#version 150\n"
	"out vec4 outColor;\n"
	"in vec2 TexCoord;\n"
	"in vec3 fragColor;\n"
	"uniform sampler2D texLeft;\n"
	"uniform sampler2D texRight;\n"
	"uniform float time;\n"
	"void main()\n"
	"{\n"
	"vec4 leftColor = texture( texLeft, TexCoord );\n"
	"vec4 rightColor = texture( texRight, TexCoord);\n"
	"outColor =  vec4(leftColor.x, rightColor.y, rightColor.z , 1.0 );\n"
	"}\n"
);



GLuint vao;
GLuint vbo;
GLuint ebo;
float vertices[] = {

	-1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	-1.0f,-1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

};

GLuint elements[] = { 0, 1, 2, 2, 3, 0 };
GLuint vShader;
GLuint fShader;
GLuint program;
GLuint tex[2];

void createEBO();
void createVAO();
void createVBO();
void createShaders();
void createTex(string im1, string im2);
void init(string im1, string im2);
void loadtex();

void createEBO()
{
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
}

void createVAO()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}

void createVBO()
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void createShaders()
{
	const char * vertShader = vertexShader.c_str();
	vShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShader, 1, &vertShader, NULL);
	glCompileShader(vShader);

	const char * fragShader = fragmentShader.c_str();
	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader, 1, &fragShader, NULL);
	glCompileShader(fShader);


	GLint status;
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &status);

	char buffer[512];
	glGetShaderInfoLog(vShader, 512, NULL, buffer);

	printf("%s", buffer);


	glGetShaderiv(fShader, GL_COMPILE_STATUS, &status);
	glGetShaderInfoLog(fShader, 512, NULL, buffer);

	printf("%s", buffer);
}

void createTex(string im1, string im2)
{

	glGenTextures(2, tex);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, channels;
	//unsigned char* image = SOIL_load_image("datasets/mansion_image_raw/mansion_image-raw_0035.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	//string filepath = "datasets/mansion_image_raw/mansion_image-raw_00" + to_string(im1) + ".jpg";

	string filepath = im1;

	unsigned char* image = stbi_load(filepath.c_str(), &width, &height, &channels, 3);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	printf("%d %d", &width, &height);
	stbi_image_free(image);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//unsigned char* image2 = SOIL_load_image("datasets/mansion_image_raw/mansion_image-raw_0036.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	//filepath = "datasets/mansion_image_raw/mansion_image-raw_00" + to_string(im2) + ".jpg";
	filepath = im2;

	unsigned char* image2 = stbi_load(filepath.c_str(), &width, &height, &channels, 3);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
	printf("%d %d", &width, &height);
	stbi_image_free(image2);


}


void init(string im1, string im2)
{

	createVAO();

	createVBO();
	createEBO();
	createTex(im1, im2);
	createShaders();

	program = glCreateProgram();
	glAttachShader(program, vShader);
	glAttachShader(program, fShader);

	glBindFragDataLocation(program, 0, "outColor");
	glLinkProgram(program);
	glUseProgram(program);

	GLuint posAttrib = glGetAttribLocation(program, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * (sizeof(float)), 0);

	GLuint colorAttrib = glGetAttribLocation(program, "color");
	glEnableVertexAttribArray(colorAttrib);
	glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 8 * (sizeof(float)), (const void*)(3 * sizeof(float)));


	GLuint texAttrib = glGetAttribLocation(program, "texCoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void *)(6 * sizeof(float)));


	glUniform1i(glGetUniformLocation(program, "texLeft"), 0);
	glUniform1i(glGetUniformLocation(program, "texRight"), 1);


	//check OpenGL error
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error: " << err << endl;
	}

}

void loadtex()
{
	cout << "Loading stereo pair" << endl;

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	//fullscreen
	GLFWwindow* window = glfwCreateWindow(1600, 1200, "test", glfwGetPrimaryMonitor(), nullptr);
	//GLFWwindow* window = glfwCreateWindow(1600, 1200, "linear remapping", nullptr, nullptr);


	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();


	string im1 = "dataset2/images/mansion_image_0025.jpg";
	
	//im2.bmp is being imwritten as a result of energy minimization cut in main() below
	string im2 = "im2.bmp";


	init(im1, im2);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUniform4f(glGetUniformLocation(program, "isRed"), 1.0f, 0.0f, 0.0f, 1.0f);
		glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	glfwTerminate();
}

Mat createRawImageVolume()
{
	int num_images = 50;
	Mat temp = imread("dataset2/images/mansion_image_0025.jpg", IMREAD_UNCHANGED);
	int width = temp.cols; int height = temp.rows;

	int sizes[] = { height, width, num_images };
	Mat rvolume = cv::Mat(3, sizes, CV_8UC3, Scalar::all(0));
	int filenumber = 25;	

	for (int i = 0; i < num_images; i++)
	{
		string filepath = "dataset2/images/mansion_image_00" + to_string(filenumber) + ".jpg";
		Mat image = imread(filepath, IMREAD_UNCHANGED);

		for (int j = 0; j < height; j++)
		{
			for (int k = 0; k < width; k++)
			{
				rvolume.at<cv::Vec3b>(j, k, i) = image.at<cv::Vec3b>(j, k);
			}
		}
		filenumber++;
	}
	return rvolume;
	
}


void displaylightfield(Mat imageVol, int num_images)
{
	int counter = 1; int height = imageVol.size[0]; int width = imageVol.size[1]; int speed = 0;
	if (num_images == 50)
		speed = 10;
	else
		speed = 1;
	int chk = 0;
	while (1)
	{

		int cut = counter;
		Mat testCut = Mat::zeros(height, width, CV_8UC3);
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				testCut.at<cv::Vec3b>(i, j) = imageVol.at<cv::Vec3b>(i, j, cut);
			}
		}
		namedWindow("testcut", WINDOW_NORMAL);
		setWindowProperty("testcut", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
		imshow("testcut", testCut);
		int k = waitKey(speed);
		if (k == 27)
			break;
		counter++;
		if (counter == num_images - 1)
			counter = 1;
	}
}

int main(int argc, char** argv)
{
	int u = 100;
	trueDisparityVolume* trdispVol = new trueDisparityVolume();

	Mat rawImageVolume = createRawImageVolume();
	cout << "raw image volume created..." << endl;

	displaylightfield(rawImageVolume, 50);

	trdispVol->generateEpipolarPlane(u, rawImageVolume);

	

	disparityVolume* dispVol = new disparityVolume();
	Mat dvolume = dispVol->createDisparityVolume();
	cout << "disparity volume created..." << endl;
	trdispVol->generateEpipolarPlane(u, dvolume);


	//optionally interpolate and synthesis new images
	//grain size determines the number of new views, for eg, if grain = 10 and num_images = 50, then new total number of views = 500

	//lfinterpolate *lf = new lfinterpolate();
	//int grain = 10;				//number of new images to be synthesised between two succesive images
	//Mat lf_ = lf->interp(rawImageVolume, dvolume, grain);
	//
	//trdispVol->generateEpipolarPlane(u, lf_);
	////displaylightfield(lf_, 500);
	
	Mat tdvolume = trdispVol->createTrueDisparityVolume();
	cout << "true disparity volume created..." << endl;
	trdispVol->generateEpipolarPlane(u, tdvolume);

	Mat goaldisp = imread("goaldisparity2.jpg", IMREAD_UNCHANGED);

	Mat tsgvolume = trdispVol->generateTsGVolume(goaldisp, tdvolume);
	cout << "(true disparity - goal disparity) volume created..." << endl;
	trdispVol->generateEpipolarPlane(u, tsgvolume);

	Mat gsvolume = dispVol->gradientInSVolume();
	cout << "gradient in s volume created..." << endl;

	cout << "calculating mincut..." << endl;
	Mat generatedimage = calculateMincut(gsvolume, dvolume, tsgvolume, rawImageVolume);
	imwrite("im2.bmp", generatedimage);

	namedWindow("final image", WINDOW_NORMAL);
	//imshow("final image", generatedimage);
	waitKey(0);

	loadtex();
	/*int u; cin >> u;*/

	//cout << "True disparity Volume - Goal Disparity" << endl;
	//trdispVol->generateEpipolarPlane(u, tsgvolume);



	//int u = 300;
	//trueDisparityVolume* trdispVol = new trueDisparityVolume();

	//Mat rawImageVolume = createRawImageVolume();
	//cout << "raw image volume created..." << endl;

	//displaylightfield(rawImageVolume, 50);

	//trdispVol->generateEpipolarPlane(u, rawImageVolume);

	//trueDisparityVolume* trdispVol = new trueDisparityVolume();
	//trdispVol->createTrueDisparityVolumeImages();

	//disparityVolume* dispVol = new disparityVolume();
	//dispVol->createDisparityVolumeImages();
	//Mat dvolume = dispVol->createDisparityVolume();
	//cout << "disparity volume created..." << endl;
	//trdispVol->generateEpipolarPlane(u, dvolume);


	////optionally interpolate and synthesis new images
	////grain size determines the number of new views, for eg, if grain = 10 and num_images = 50, then new total number of views = 500

	////lfinterpolate *lf = new lfinterpolate();
	////int grain = 10;				//number of new images to be synthesised between two succesive images
	////Mat lf_ = lf->interp(rawImageVolume, dvolume, grain);
	////
	////trdispVol->generateEpipolarPlane(u, lf_);
	//////displaylightfield(lf_, 500);



	//Mat tdvolume = trdispVol->createTrueDisparityVolume();
	//cout << "true disparity volume created..." << endl;
	//trdispVol->generateEpipolarPlane(u, tdvolume);

	//Mat goaldisp = imread("goaldisparity2.jpg", IMREAD_UNCHANGED);

	//Mat tsgvolume = trdispVol->generateTsGVolume(goaldisp, tdvolume);
	//cout << "(true disparity - goal disparity) volume created..." << endl;
	//trdispVol->generateEpipolarPlane(u, tsgvolume);

	//Mat gsvolume = dispVol->gradientInSVolume();
	//cout << "gradient in s volume created..." << endl;

	//cout << "calculating mincut..." << endl;
	//Mat generatedimage = calculateMincut(gsvolume, dvolume, tsgvolume, rawImageVolume);
	//imwrite("im2.bmp", generatedimage);

	//namedWindow("final image", WINDOW_NORMAL);
	////imshow("final image", generatedimage);
	//waitKey(0);

	//loadtex();





}