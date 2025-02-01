#pragma once
#include "GameObject.h"
#include <string>
#include <map>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

struct FontCharacter
{
	unsigned int TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	unsigned int Advance;
};

class Graphics
{
public:
	static Graphics& Get();

public:
	bool initializeGraphics();
	void setupObjectBuffers();
	void setupTextBuffers();
	unsigned int compileShader(unsigned int type, const char* source);
	std::string getCurrentExeDir();
	bool loadFont(const char* fontPath);

	bool shouldClose();
	void renderFrame(double elapsedTime);
	void renderObject(GameObject targetObject);
	void renderText(std::string text, float x, float y, float scale, glm::vec3 color);

private:
	const int APOS_SIZE = 3;
	const int ATEXCOORDS_SIZE = 2;

	GLFWwindow* window = nullptr;
	unsigned int objectVAO, objectVBO, objectEBO;
	unsigned int objectShaderProgram;
	unsigned int textVAO, textVBO;
	unsigned int textShaderProgram;

	int objectTransformLoc, objectProjectionLoc, objectColorLoc;
	int textTransformLoc, textProjectionLoc, textTextureLoc, textColorLoc;

	std::map<char, FontCharacter> FontCharacters;
};