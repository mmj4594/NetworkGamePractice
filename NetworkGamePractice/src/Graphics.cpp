#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Graphics.h"
#include "Game.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glfwSetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Game::Get().onKey(window, key, scancode, action, mods);
}

Graphics& Graphics::Get()
{
	static Graphics instance;
	return instance;
}

bool Graphics::initializeGraphics()
{
	// Initialize GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return false;
	}

	// Set OpenGL version (3.3 Core Profile)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a window
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Window", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(window, key_callback);

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		glfwTerminate();
		return false;
	}

	// Set Viewport Size
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// VAO/VBO/EBO Setting	
	setupBuffers(VAO, VBO, EBO);

	// Shader Compile
	const char* vertexShaderSource =
	R"(
		#version 330 core
		layout(location = 0) in vec3 aPos;
		uniform mat4 transform;
		uniform mat4 projection;

		void main()
		{
			gl_Position = projection * transform * vec4(aPos, 1.0);
		}
	)";
	const char* fragmentShaderSource =
	R"(
		#version 330 core
		out vec4 FragColor;
		uniform vec4 color;

		void main()
		{
			FragColor = color;
		}
	)";
	unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
	unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	// Create Shader Program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	colorLoc = glGetUniformLocation(shaderProgram, "color");
	transformLoc = glGetUniformLocation(shaderProgram, "transform");
	projectionLoc = glGetUniformLocation(shaderProgram, "projection");

	return true;
}

void Graphics::setupBuffers(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO)
{
	// Vertex Data (Square)
	constexpr float squareVertices[] = {
		// x, y, z
		-0.5f, -0.5f, 0.0f,  // Bottom-left
		 0.5f, -0.5f, 0.0f,  // Bottom-right
		 0.5f,  0.5f, 0.0f,  // Top-right
		-0.5f,  0.5f, 0.0f   // Top-left
	};
	constexpr unsigned int squareIndices[] = {
		0, 1, 2, // Triangle 1
		2, 3, 0  // Triangle 2
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareIndices), squareIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, VERTEX_SIZE, GL_FLOAT, GL_FALSE, VERTEX_SIZE * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

unsigned int Graphics::compileShader(unsigned int type, const char* source)
{
	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cerr << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " shader compilation failed\n" << infoLog << std::endl;
	}
	return shader;
}

bool Graphics::shouldClose()
{
	return glfwWindowShouldClose(window);
}

void Graphics::renderFrame(double elapsedTime)
{
	// Clear Screen
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Object Rendering
	Graphics::Get().renderObject(Game::Get().player1);
	Graphics::Get().renderObject(Game::Get().player2);
	Graphics::Get().renderObject(Game::Get().ball);
	Graphics::Get().renderObject(Game::Get().net);
	Graphics::Get().renderObject(Game::Get().leftWall);
	Graphics::Get().renderObject(Game::Get().rightWall);
	Graphics::Get().renderObject(Game::Get().floor);
	Graphics::Get().renderObject(Game::Get().ceil);

	// Buffer Swap
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Graphics::renderObject(GameObject targetObject)
{
	if (shaderProgram == 0 || VAO == 0)
	{
		std::cerr << "renderObject: Shader program or VAO is not initialized\n";
		return;
	}

	glUseProgram(shaderProgram);

	// set color
	glUniform4f(colorLoc, targetObject.getColor().r, targetObject.getColor().g, targetObject.getColor().b, 1.0f);

	// transform
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, glm::vec3(targetObject.getPosition(), 0.0f));
	transform = glm::scale(transform, glm::vec3(targetObject.getWidth(), targetObject.getHeight(), 1.0f));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	// projection
	glm::mat4 projection = glm::ortho(0.f, (float)SCREEN_WIDTH, 0.f, (float)SCREEN_HEIGHT, -1.f, 1.f);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
