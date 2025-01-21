#pragma once
#include "GameObject.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

class Graphics
{
public:
	static Graphics& Get();

public:
	bool initializeGraphics();
	void setupBuffers(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO);
	unsigned int compileShader(unsigned int type, const char* source);

	bool shouldClose();
	void renderFrame(double elapsedTime);
	void renderObject(GameObject targetObject);

private:
	const int VERTEX_SIZE = 3;

	GLFWwindow* window = nullptr;
	unsigned int VAO, VBO, EBO;
	unsigned int shaderProgram;

	int colorLoc;
	int transformLoc;
	int projectionLoc;
};