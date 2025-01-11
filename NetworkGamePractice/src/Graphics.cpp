#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "graphics.h"

// Vertex Data (Square)
float squareVertices[] = {
	// x, y, z
	-0.5f, -0.5f, 0.0f,  // Bottom-left
	 0.5f, -0.5f, 0.0f,  // Bottom-right
	 0.5f,  0.5f, 0.0f,  // Top-right
	-0.5f,  0.5f, 0.0f   // Top-left
};
unsigned int squareIndices[] = {
	0, 1, 2, // Triangle 1
	2, 3, 0  // Triangle 2
};

// Shader
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void setupBuffers(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO)
{
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

unsigned int compileShader(unsigned int type, const char* source)
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

void renderObject(unsigned int VAO, unsigned int shaderProgram, float x, float y, float width, float height, float r, float g, float b)
{
	glUseProgram(shaderProgram);

	// set color
	int colorLoc = glGetUniformLocation(shaderProgram, "color");
	glUniform4f(colorLoc, r, g, b, 1.0f);

	// transform
	int transformLoc = glGetUniformLocation(shaderProgram, "transform");
	float transform[16] = {
		width, 0.0f,  0.0f, 0.0f,
		0.0f,  height, 0.0f, 0.0f,
		0.0f,  0.0f,  1.0f, 0.0f,
		x,     y,     0.0f, 1.0f
	};
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}