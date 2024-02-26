#pragma once

#include <cstdio>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <glad/glad.h>


enum Edits {
	CONTRAST,
	BRIGHTNESS,
	COLORMASK
};

class Image
{

public:

	std::string filename;
	std::string lastedited;

	//opengl
	GLuint texID;
	GLuint orgTexID;

	//image 
	cv::Mat Img;
	cv::Mat copy;

	//size
	int width;
	int height;
	int w_width;
	int w_height;

	int channels;
	const void* pixels;

	Image() = default;
	Image(std::string filepath);
	Image(int w, int h, int ch);
	Image(const Image& img);
	~Image();

	friend std::ostream& operator<<(std::ostream& os, const Image& obj)
	{
		// write obj to stream
		return os.write(obj.filename.c_str(), obj.filename.size());
	}
	bool read(std::string);
	bool write(std::string);
	int cv_display(std :: string );

	void grayscale_avg();
	void grayscale_lum();

	void color_mask(float* r, float* g, float* b);
	void brightness(float*);
	void contrast(float* );
	void hue_saturation(int*, float*);
	void gammaCorrection(float* gamma);


	//blur
	void BoxBlur(int* blurRadius);
	void GaussBlur(int* blurRadius, float*, float*);
	void MedianBlur(int*);


};

