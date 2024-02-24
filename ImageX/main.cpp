#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "myImgui.h"
#include <opencv2/opencv.hpp>





void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}



float windowWidth = 800.0f;
float windowHeight = 600.0f;
float aspectRatio = windowWidth / windowHeight;
bool isDragging = false;
double mouseX, mouseY;

int main() {


    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);   // Resizable
    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(windowWidth,windowHeight, "Window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL();

    //Init Imgui
    MyImgui guiWindow = MyImgui(window);
    guiWindow.Init();


    // Variables to create periodic event for FPS displaying
    double prevTime = 0.0;
    double crntTime = 0.0;
    double timeDiff;
    // Keeps track of the amount of frames in timeDiff
    unsigned int counter = 0;

    // Main loop
    while (!glfwWindowShouldClose(window)) {

        // Updates counter and times
        crntTime = glfwGetTime();
        timeDiff = crntTime - prevTime;
        counter++;

        if (timeDiff >= 1.0 / 30.0)
        {
            // Creates new title
            std::string FPS = std::to_string((1.0 / timeDiff) * counter);
            std::string ms = std::to_string((timeDiff / counter) * 1000);
           
            std::string newTitle = "FPS : " + FPS + " ms : " + ms;
            guiWindow.FPS = newTitle;

            // Resets times and counter
            prevTime = crntTime;
            counter = 0;
        }

        // Handle GLFW events
        glClear(GL_COLOR_BUFFER_BIT);
        //glfwPollEvents();
       
        guiWindow.Render();
        

        // Swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    guiWindow.Destroy();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}



// Callback function for mouse button events
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            // Check if the mouse click is inside the window
            if (xpos >= 0.0f && xpos <= windowWidth && ypos >= 0.0f && ypos <= 20.0f) {
                isDragging = true;
                mouseX = xpos;
                mouseY = ypos;
                //lastUpdateTime = glfwGetTime();
            }
        }
        else if (action == GLFW_RELEASE) {
            isDragging = false;
        }
    }
}

// Callback function for cursor position events
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (isDragging) {

        // Calculate the offset from the initial click position
        float dx = static_cast<float>(xpos - mouseX);
        float dy = static_cast<float>(ypos - mouseY);

        int x, y;
        glfwGetWindowPos(window, &x, &y);
        // Update the window position
        glfwSetWindowPos(window, static_cast<int>(x + dx), static_cast<int>( y+ dy));

        mouseX = xpos;
        mouseY = ypos;
        //lastUpdateTime = currentTime;
    }
}

