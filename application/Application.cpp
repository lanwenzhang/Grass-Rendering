#include "Application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Application* Application::mInstance = nullptr;
Application* Application::getInstance() {

	if (mInstance == nullptr) {
		mInstance = new Application();
	}

	return mInstance;
}

Application::Application() {

}

Application::~Application() {

}

bool Application::init(const int& width, const int& height) {

    mWidth = width;
    mHeight = height;

    // 1 Initialize the GLFW context
    glfwInit();
    // 1.1 OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    // 1,2 OpenGL rendering mode
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // 2 Create a window object
    mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL Project", NULL, NULL);
    if (mWindow == NULL) {
        return false;
    }
    // 2.1 Set current
    glfwMakeContextCurrent(mWindow);

    // Load all the functions by using glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    glfwSetFramebufferSizeCallback(mWindow, frameBufferSizeCallback);

    glfwSetWindowUserPointer(mWindow, this);

    glfwSetKeyCallback(mWindow, keyCallback);

    glfwSetMouseButtonCallback(mWindow, mouseCallback);

    glfwSetCursorPosCallback(mWindow, cursorCallback);

    glfwSetScrollCallback(mWindow, scrollCallback);

    return true;
}

bool Application::update() {
    if (glfwWindowShouldClose(mWindow)) {
        return false;
    }
    
    // 3.1 Get and send message
    glfwPollEvents();

    // 3.3 Render

    // 3.4 Swap double buffer
    glfwSwapBuffers(mWindow);

    return true;
}

void Application::destroy() {
    
    glfwTerminate();

    }

void Application::getCursorPosition(double* x, double* y){

    glfwGetCursorPos(mWindow, x, y);
}

void Application::frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    
    Application* self = (Application*) glfwGetWindowUserPointer(window);
    if(self->mResizeCallback != nullptr){ 
        self->mResizeCallback(width, height); 
    }
}

void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    Application* self = (Application*)glfwGetWindowUserPointer(window);
    if (self->mKeyBoardCallback != nullptr) {
        self->mKeyBoardCallback(key, action, mods);
    }
}

void Application::mouseCallback(GLFWwindow* window, int button, int action, int mods) {

    Application* self = (Application*)glfwGetWindowUserPointer(window);
    if (self->mMouseCallback != nullptr) {
        self->mMouseCallback(button, action, mods);
    }

}

void Application::cursorCallback(GLFWwindow* window, double xpos, double ypos) {

    Application* self = (Application*)glfwGetWindowUserPointer(window);
    if (self->mCursorCallback != nullptr) {
        self->mCursorCallback(xpos, ypos);
    }

}

void Application::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {

    Application* self = (Application*)glfwGetWindowUserPointer(window);
    if (self->mScrollCallback != nullptr) {
        self->mScrollCallback(yoffset);
    }
}