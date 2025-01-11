#pragma once

constexpr int VERTEX_SIZE = 3;

// Vertex Data (Square)
extern float squareVertices[];
extern unsigned int squareIndices[];

// Shader
extern const char* vertexShaderSource;
extern const char* fragmentShaderSource;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void setupBuffers(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO);
unsigned int compileShader(unsigned int type, const char* source);
void renderObject(unsigned int VAO, unsigned int shaderProgram, float x, float y, float width, float height, float r, float g, float b);