#include "Window.h"

#include <iostream>


Vulkan::Window::Window(int width, int height, const std::string& title) : window{ nullptr, deleteWindow }, title{ title } {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); //TODO make it resizable
	window = std::unique_ptr<GLFWwindow, decltype(&deleteWindow)>(glfwCreateWindow(width, height, this->title.c_str(), nullptr, nullptr), deleteWindow);
	std::cout << "\n+ Window created";
}



GLFWwindow* Vulkan::Window::operator+() const {
	return window.get();
}



void Vulkan::Window::deleteWindow(GLFWwindow* window) {
	glfwDestroyWindow(window);
}