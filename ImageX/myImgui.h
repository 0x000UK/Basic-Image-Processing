#pragma once


#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <map>
#include "Image.h"

class MyImgui
{
	GLFWwindow* window;
	int w_width;
	int w_height;

public:
	std::string FPS;
	std::map<std::string, Image> map;
	std::vector<std::string> files;

	MyImgui(GLFWwindow* wndw);
	void Init();


	//Image window
	void display(GLuint textureID);
	// Start the ImGui frame
	void Begin();
	void End();
	void Render();

	//Destroy all context
	void Destroy();
};

