#include "myImgui.h"
#include <GLFW/glfw3.h>
#include <string>
#include <filesystem>


// Windows-specific headers
#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#endif


MyImgui::MyImgui(GLFWwindow* wndw) : window( wndw)
{
	 //files = getFilesFromFolder();
}

void MyImgui::Init()
{
	
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	io.ConfigFlags |= ImGuiWindowFlags_NoTitleBar;				// Disable Title Bar
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

	// Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 2.0f;
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);;
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); 
		style.Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		style.WindowPadding.y = 10.0f;
		//io.FontGlobalScale = 1.5f;
		style.FramePadding.y = 8.0f;
		style.ItemSpacing.y = 5.0f;

	}

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

}

void MyImgui::display(GLuint textureID)
{
	ImGui::Begin("Image Example");

	ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(textureID)), ImVec2(800, 600));

	ImGui::End();
}

void MyImgui::Begin()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void MyImgui::End()
{
	int width, height;
	ImGuiIO& io = ImGui::GetIO();
	glfwGetWindowSize(window, &width, &height);
	io.DisplaySize = ImVec2((float)width, (float)height);

	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

// Function to open a file dialog for opening a file
bool OpenFile(std::string& filePath) {
	OPENFILENAMEA ofn;
	CHAR szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Image Files\0 * .jpg; *.jpeg; *.png; *.bmp\0All Files\0 * .*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameA(&ofn) == TRUE) {
		filePath = ofn.lpstrFile;
		return true;
	}

	return false;
}

// Function to open a file dialog for saving a file
bool SaveFile(std::string& filePath) {
	OPENFILENAMEA ofn;
	CHAR szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "jpeg\0*.jpeg\0jpg\0*.jpg\0png\0*.png\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

	if (GetSaveFileNameA(&ofn) == TRUE) {
		filePath = ofn.lpstrFile;
		return true;
	}

	return false;
}

bool CreateNewFile(const std::string& filePath) {
	HANDLE hFile = CreateFileA(
		filePath.c_str(),                 // File path
		GENERIC_WRITE,                    // Access mode
		0,                                // Share mode
		NULL,                             // Security attributes
		CREATE_ALWAYS,                    // Creation disposition (CREATE_ALWAYS creates a new file or overwrites an existing one)
		FILE_ATTRIBUTE_NORMAL,            // File attributes
		NULL                              // Template file
	);

	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
		return true;
	}

	return false;
}




Image img;
std::vector<int> oddIntegers = { 1, 3, 5, 7, 9,11, 13, 15, 17, 19, 21 };
void MyImgui::Render()
{
	//window
	static bool showdemo = false;
	static int selectedItem = 0;
	static bool show_image = false;
	static bool showdocked = true;
	static bool showblurwindow = false;
	static bool default_edit = false;

	static float zoom = 1.0f;
	static float r = 1, g = 1, b = 1, contrast = 0;
	static float brightness = 0;
	static int hue = 0;
	static float saturation = 0;
	static float gamma = 0;
	//blur
	static int selected_gauss_kernel = 1;
	static int selected_median_kernel = 1;
	static int boxradius = 1;
	static float sigma_x = 0, sigma_y = 0;

	Begin();
	// Begin the main menu bar
	
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_flags);
	//ImGui::Begin("Menubar", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {

			if (ImGui::MenuItem("New File")) {
				// Handle "New" action
				//std::string filePath;
				//if (OpenFile(filePath)) {
					// Handle the selected file path (e.g., print it to console)
					//std::cout <<"Selected File: \n" << filePath.c_str()<< std::endl;





				//}
			}
			if (ImGui::MenuItem("Open File")) {
				// Handle "Open" action
				std::string filePath;
				if (OpenFile(filePath)) {
					// Handle the selected file path (e.g., print it to console)
					Image newImg = Image(filePath);
					map[newImg.filename] = newImg;
					files.push_back(newImg.filename);
					img = newImg;
					show_image = true;
				}
			}
			if (ImGui::MenuItem("Save File")) {
				// Handle "Save" action
				std::string filePath;
				if (SaveFile(filePath)) {
					// Handle the selected file path (e.g., print it to console)
					std::cout << "Selected File to save: \n" << filePath.c_str() << std::endl;
					if(!img.write(filePath))
						std::cout << "Failed to Save file\n" << std::endl;
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Default")) {
				// Handle "New" action
				default_edit = true;
			}
			if (ImGui::BeginMenu("Grayscale")) {
				if (ImGui::MenuItem("average")) {
					img.grayscale_avg();
				}
				if (ImGui::MenuItem("luminscence")) {
					// Handle "Option B"
					img.grayscale_lum();
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Blur")) {
				// Handle "Save" action
				showblurwindow = true;
			}
			if (ImGui::MenuItem("Reset")) {
				// Handle "Save" action
				r = 1; g = 1; b = 1;
				contrast = 0;
				brightness = 0;
				hue = 0;
				saturation = 1;
				boxradius = 1;
				selected_gauss_kernel = 1;
				gamma = 0;
				img.copy = img.Img;
				img.texID = img.orgTexID;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View")) {
			if (ImGui::MenuItem("Basic Editing")) {
				// Handle "New" action
				std::string filePath;
				if (OpenFile(filePath)) {
					// Handle the selected file path (e.g., print it to console)
					std::cout << "Selected File: \n" << filePath.c_str() << std::endl;
				}
			}
			if (ImGui::MenuItem("File selection")) {
				// Handle "Open" action
				showdemo = true;

			}
			if (ImGui::MenuItem("Demowindow")) {
				// Handle "Save" action
				showdemo = true;
			}
			ImGui::EndMenu();
		}


		// Add more menus as needed
		ImGui::SameLine(ImGui::GetContentRegionAvail().x + 100.0f); // Adjust the position as needed
		if (ImGui::Button("-")) {
			// Handle cross button click
			glfwIconifyWindow(window);
		}
		if (ImGui::Button("X")) {
			// Handle cross button click
			glfwSetWindowShouldClose(window, true);
		}



		ImGui::EndMainMenuBar();
	}
	
	if (default_edit) {
		
		

		ImGui::Begin("Edit", nullptr);
		/// Color Mask
		ImGui::SeparatorText("Color Filters");
		if (ImGui::SliderFloat("Red", &r, 0, 1)) {
			img.color_mask(&r, &g, &b);
			img.lastedited = "mask";
		}
		if (ImGui::SliderFloat("Green", &g, 0, 1)) {
			img.color_mask(&r, &g, &b);
			img.lastedited = "mask";
		}
		if (ImGui::SliderFloat("Blue", &b, 0, 1)) {
			img.color_mask(&r, &g, &b);
			img.lastedited = "mask";
		}
		
		/// ZOOM
		ImGui::SeparatorText("Zoom level");
		ImGui::DragFloat("Zoom", &zoom,1.0f,1.0f,100.0f);

		/// BRIGHTNESS
		ImGui::SeparatorText("Brightness and Contrast");

		if (ImGui::SliderFloat("Brightness", &brightness, -100, 100)) {
			img.brightness(&brightness);
			img.lastedited = "bright";
		}
		if (ImGui::SliderFloat("Contrast", &contrast, 0.000, 5.000)) {
			img.contrast(&contrast);
			img.lastedited = "contrast";
		}
		ImGui::SeparatorText("Hue and Saturation");
		if (ImGui::SliderInt("Hue", &hue, -100, 100)) {
			img.hue_saturation(&hue, &saturation);
			img.lastedited = "hue_sat";
		}
		if (ImGui::SliderFloat("Saturation", &saturation, -1.00, 1.000)) {
			img.hue_saturation(&hue, &saturation);
			img.lastedited = "hue-sat";
		}
		ImGui::SeparatorText("Gamma");
		if (ImGui::SliderFloat("gamma", &gamma, 1.000f, 5.000f)) {
			img.gammaCorrection(&gamma);
			img.lastedited = "gamma";
		}
		ImGui::End();
	}

	if (showblurwindow) {
		
		ImGui::Begin("Blur", nullptr);


		
		ImGui::SeparatorText("Homogenous Blur");
		// Input field with a hint and a button
		ImGui::Text("\n");
		if (ImGui::SliderInt("Box Kernal Size", &boxradius, 1, 30)) {
			img.BoxBlur(&boxradius);
		}
		ImGui::Text("\n\n");
		// Input field  with a hint and a button
		ImGui::SeparatorText("Gaussian Blur");
		ImGui::Text("\n");

		if (ImGui::BeginCombo("kernal size", std::to_string(selected_gauss_kernel).c_str())) {
			for (int i = 0; i < oddIntegers.size(); ++i) {
				bool isSelected = (selected_gauss_kernel == oddIntegers[i]);
				if (ImGui::Selectable(std::to_string(oddIntegers[i]).c_str(), isSelected)) {
					selected_gauss_kernel = oddIntegers[i];
					img.GaussBlur(&selected_gauss_kernel, &sigma_x, &sigma_x);
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
					// Set the selected item as default focus
				}
			}
			ImGui::EndCombo();
		}
		ImGui::SliderFloat("dx", &sigma_x, 0, 10);
		ImGui::SliderFloat("dy", &sigma_y, 0, 10);

		ImGui::Text("\n\n");
		// Input field  with a hint and a button
		ImGui::SeparatorText("Median Blur");
		ImGui::Text("\n");

		if (ImGui::BeginCombo("median kernal size", std::to_string(selected_median_kernel).c_str())) {
			for (int i = 0; i < oddIntegers.size(); ++i) {
				bool isSelected = (selected_median_kernel == oddIntegers[i]);
				if (ImGui::Selectable(std::to_string(oddIntegers[i]).c_str(), isSelected)) {
					selected_median_kernel = oddIntegers[i];
					img.MedianBlur(&selected_median_kernel);
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
					// Set the selected item as default focus
				}
			}
			ImGui::EndCombo();
		}
		ImGui::End();
	}
	

	if (showdocked) {
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		ImGui::Begin("Docked Window", &showdocked, ImGuiWindowFlags_NoTitleBar);
		ImGui::Text("Select an item:");
		// Pass vector of strings to Combo


		
		std::vector<const char*> itemLabels;
		for (const auto& item : files) {
			itemLabels.push_back(item.c_str());
		}
		if (ImGui::Combo("##combo", &selectedItem, itemLabels.data(), static_cast<int>(itemLabels.size()))) {
			img = map[files[selectedItem]];
			ImGui::SetItemDefaultFocus();
			show_image = true;
		}
		ImGui::SameLine();
		static bool cv = false;
		if (ImGui::Checkbox("CV Image", &cv))
		{
			if (!map.empty()) {
				int close = map[files[selectedItem]].cv_display(map[files[selectedItem]].filename);
				if (close == -1) cv = false;
				//else cv = true;
			}
			else {
				std::cout << "No image to dispaly, please open one from file explorer" << std::endl;
			}
		}
		// Your ImGui content goes here

		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}
	if (show_image) {
		
		ImGui::Begin("Image", nullptr, ImGuiWindowFlags_NoTitleBar /*| ImGuiWindowFlags_NoDecoration*/);
		// Get the available content region size
		ImVec2 contentSize = ImGui::GetContentRegionAvail();
		if (img.height > img.width) {
			float ratio = contentSize.y / img.height;
			img.w_height = img.height * ratio;
			img.w_width = img.width * ratio;
		}
		else {
			float ratio = contentSize.x / img.width;
			img.w_height = img.height * ratio;
			img.w_width = img.width * ratio;
		}
		// Set the image size to fit inside the content region
		ImVec2 imageSize(img.w_width*zoom, img.w_height*zoom);
		ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(img.texID)),imageSize);
		ImGui::End();
	}
		End();
	
	

}

void MyImgui::Destroy()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
