#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <array>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct Segment {
    glm::vec3 start;
    glm::vec3 end;
};

class Renderer {
public:
    Renderer(int width, int height, const char* title);
    ~Renderer();
    void loadSegments(const std::vector<Segment>& segments);
    void drawSegments();
    void drawVoxels();
    void rotateCamera(float angle);
    void loadBrainAsGrid(const std::vector<float>& b0, int dimX, int dimY, int dimZ);

private:
    GLFWwindow* window;
    unsigned int VAO, VBO, CBO, shaderProgram;
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::vec3 cameraPos, cameraTarget, cameraUp;
    int verticesSize;
    void initOpenGL();
    void updateViewMatrix();
    void setupShaders();
    unsigned int compileShader(const char* source, GLenum shaderType);
};

