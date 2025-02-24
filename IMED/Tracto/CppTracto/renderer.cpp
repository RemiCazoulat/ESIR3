#include "renderer.h"
#include <iostream>

Renderer::Renderer(int width, int height, const char* title) {
    if (!glfwInit()) {
        throw std::runtime_error("Échec d'initialisation de GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Échec de création de la fenêtre GLFW");
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        throw std::runtime_error("Échec d'initialisation de GLAD");
    }

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int w, int h) {
        glViewport(0, 0, w, h);
    });

    projectionMatrix = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 400.0f);
    // =39, const int y_o = 44, const int z_o = 35
    cameraPos = glm::vec3(39.0f, 44.0f, 80.0f);
    cameraTarget = glm::vec3(39.0f, 44.0f, 35.0f);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    updateViewMatrix();

    initOpenGL();
    setupShaders();
}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &CBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
}

void Renderer::initOpenGL() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &CBO);
}

void Renderer::updateViewMatrix() {
    viewMatrix = glm::lookAt(cameraPos, cameraTarget, cameraUp);
}

void Renderer::rotateCamera(float angle) {
    float radius = glm::length(cameraPos - cameraTarget);
    cameraPos.x = cameraTarget.x + radius * cos(angle);
    cameraPos.z = cameraTarget.z + radius * sin(angle);
    updateViewMatrix();
}


void Renderer::loadBrainAsGrid(const std::vector<float>& b0, int dimX, int dimY, int dimZ) {
    std::vector<float> vertices;
    std::vector<float> colors;
    float minVal = *std::min_element(b0.begin(), b0.end());
    float maxVal = *std::max_element(b0.begin(), b0.end());
    // On parcourt le tableau b0 et on crée un voxel pour chaque valeur
    for (int x = 0; x < dimX; ++x) {
        for (int y = 0; y < dimY; ++y) {
            for (int z = 30; z < 31; ++z) {
                int index = x + dimX * (y + dimY * z);
                float value = b0[index];
                value = (value - minVal) / (maxVal - minVal);
                if (value > 0.0f) {  // Si la valeur est positive, on crée un voxel
                    // Coordonnées des 8 sommets du cube (voxel)
                    float halfSize = 0.5f;  // Taille des cubes
                    float offsetX = (float)x;
                    float offsetY = (float)y;
                    float offsetZ = (float)z;

                    // Cube
                    /*vertices.insert(vertices.end(), {
                            offsetX - halfSize, offsetY - halfSize, offsetZ - halfSize,  // 0
                            offsetX + halfSize, offsetY - halfSize, offsetZ - halfSize,  // 1
                            offsetX + halfSize, offsetY + halfSize, offsetZ - halfSize,  // 2
                            offsetX - halfSize, offsetY + halfSize, offsetZ - halfSize,  // 3
                            offsetX - halfSize, offsetY - halfSize, offsetZ + halfSize,  // 4
                            offsetX + halfSize, offsetY - halfSize, offsetZ + halfSize,  // 5
                            offsetX + halfSize, offsetY + halfSize, offsetZ + halfSize,  // 6
                            offsetX - halfSize, offsetY + halfSize, offsetZ + halfSize   // 7
                    });*/

                    vertices.insert(vertices.end(), {offsetX, offsetY, offsetZ});

                    // Couleur basée sur la valeur de `b0`
                    glm::vec3 color(value, value, value);  // Niveau de gris

                    colors.insert(colors.end(), {
                            color.r, color.g, color.b,  // 0
                            });
                }
            }
        }
    }
    verticesSize = vertices.size();
    // Stocker les données dans OpenGL
    glBindVertexArray(VAO);

    // VBO pour les positions des vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // CBO pour les couleurs des vertices
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
}


void Renderer::setupShaders() {
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        out vec3 fragColor;
        uniform mat4 view;
        uniform mat4 projection;
        void main() {
            gl_Position = projection * view * vec4(aPos, 1.0);
            fragColor = aColor;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 fragColor;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(fragColor, fragColor.x);
        }
    )";

    unsigned int vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    unsigned int fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        throw std::runtime_error("Erreur de linkage du programme shader : " + std::string(infoLog));
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}


unsigned int Renderer::compileShader(const char* source, GLenum shaderType) {
    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        throw std::runtime_error("Erreur de compilation du shader : " + std::string(infoLog));
    }

    return shader;
}

void Renderer::loadSegments(const std::vector<Segment>& segments) {
    if (glfwWindowShouldClose(window)) {
        return;
    }

    std::vector<float> vertices;
    std::vector<float> colors;

    for (const auto& segment : segments) {
        // Coordonnées des points
        vertices.insert(vertices.end(), { segment.start.x, segment.start.y, segment.start.z });
        vertices.insert(vertices.end(), { segment.end.x, segment.end.y, segment.end.z });

        // Calcul de la couleur selon la direction
        glm::vec3 dir = glm::normalize(segment.end - segment.start);
        glm::vec3 color = glm::abs(dir);

        // Utilisation de la direction comme couleur (RGB)
        colors.insert(colors.end(), { color.r, color.g, color.b });
        colors.insert(colors.end(), { color.r, color.g, color.b });
    }
    verticesSize = vertices.size();
    glBindVertexArray(VAO);

    // VBO pour les positions
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // CBO pour les couleurs
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
}

void Renderer::drawVoxels() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glPointSize(2.0f);
    // Dessiner les voxels
    glDrawArrays(GL_POINTS, 0, verticesSize);  // 4 vertices per voxel (cube)

    glBindVertexArray(0);
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Renderer::drawSegments() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glDrawArrays(GL_LINES, 0, verticesSize / 3);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
}
