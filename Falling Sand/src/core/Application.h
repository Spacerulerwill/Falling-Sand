#pragma once

#include <memory>
#include <vector>
#include <util/Constants.h>
#include <stdint.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader/Shader.h>
#include <core/ParticleBuffer.h>


class Application
{
private:
	static std::unique_ptr<Application> s_Instance;
	static GLFWwindow* p_Window;
	ParticleBuffer particle_buffer;

	bool lmouse, rmouse = false;

	int cursor_radius = 30;
	uint8_t particle_type = SAND;

public:
	Application();

	void Run();

	static std::unique_ptr<Application>& GetInstance();

	void MouseCallback(GLFWwindow* window, int button, int action, int mods);
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void InputHandle();
};

