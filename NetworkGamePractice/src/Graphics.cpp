#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Graphics.h"
#include "Game.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float left = -width / 2.0f;
	float right = width / 2.0f;
	float bottom = -height / 2.0f;
	float top = height / 2.0f;
	glOrtho(left, right, bottom, top, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Game::Get().OnPressedKey(key, scancode, action, mods);
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
	float left = -SCREEN_WIDTH / 2.0f;
	float right = SCREEN_WIDTH / 2.0f;
	float bottom = -SCREEN_HEIGHT / 2.0f;
	float top = SCREEN_HEIGHT / 2.0f;
	glOrtho(left, right, bottom, top, -1.0f, 1.0f);
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

		void main()
		{
			gl_Position = transform * vec4(aPos, 1.0);
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

	// Buffer Swap
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Graphics::renderObject(GameObject targetObject)
{
	glUseProgram(shaderProgram);

	// set color
	int colorLoc = glGetUniformLocation(shaderProgram, "color");
	glUniform4f(colorLoc, targetObject.getColor().r, targetObject.getColor().g, targetObject.getColor().b, 1.0f);

	// transform
	int transformLoc = glGetUniformLocation(shaderProgram, "transform");
	float transform[16] = {
		targetObject.getWidth(), 0.0f, 0.0f, 0.0f,
		0.0f, targetObject.getHeight(), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		targetObject.getPosition().x, targetObject.getPosition().y, 0.0f, 1.0f
	};
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
