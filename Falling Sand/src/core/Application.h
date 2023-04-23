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

public:
	Application();

	void Run();

	static std::unique_ptr<Application>& GetInstance();

	void InputHandle();
};

