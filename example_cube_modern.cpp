#include <GLFW/glfw3.h>
#include <OpenGL/glext.h>
#include <iostream>

#include <cmath>

#define degreesToRadians(x) x*(3.141592f/180.0f)

// Vertex Shader Source Code
const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

// Fragment Shader Source Code
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 1.0, 1.0, 1.0); // White color
}
)";

// Cube vertices (positions only)
float cubeVertices[] = {
    // Front face
    -0.5f, -0.5f,  0.5f, // Bottom-left
     0.5f, -0.5f,  0.5f, // Bottom-right
     0.5f,  0.5f,  0.5f, // Top-right
    -0.5f,  0.5f,  0.5f, // Top-left

    // Back face
    -0.5f, -0.5f, -0.5f, // Bottom-left
     0.5f, -0.5f, -0.5f, // Bottom-right
     0.5f,  0.5f, -0.5f, // Top-right
    -0.5f,  0.5f, -0.5f  // Top-left
};

// Cube indices for rendering with GL_TRIANGLES
unsigned int cubeIndices[] = {
    0, 1, 2, 2, 3, 0, // Front face
    4, 5, 6, 6, 7, 4, // Back face
    0, 4, 7, 7, 3, 0, // Left face
    1, 5, 6, 6, 2, 1, // Right face
    3, 7, 6, 6, 2, 3, // Top face
    0, 4, 5, 5, 1, 0  // Bottom face
};

// Helper functions to create matrices
void createIdentityMatrix(float matrix[16]) {
    for (int i = 0; i < 16; ++i)
        matrix[i] = (i % 5 == 0) ? 1.0f : 0.0f;
}

void createPerspectiveMatrix(float fov, float aspect, float near, float far, float result[16]) {
    float tanHalfFov = tanf(fov / 2.0f);
    createIdentityMatrix(result);

    result[0] = 1.0f / (aspect * tanHalfFov);
    result[5] = 1.0f / tanHalfFov;
    result[10] = -(far + near) / (far - near);
    result[11] = -1.0f;
    result[14] = -(2.0f * far * near) / (far - near);
    result[15] = 0.0f;
}

void createRotationMatrix(float angle, float x, float y, float z, float result[16]) {
    float c = cosf(angle);
    float s = sinf(angle);
    float invC = 1.0f - c;

    result[0] = x * x * invC + c;
    result[1] = y * x * invC + z * s;
    result[2] = x * z * invC - y * s;
    result[3] = 0.0f;

    result[4] = x * y * invC - z * s;
    result[5] = y * y * invC + c;
    result[6] = y * z * invC + x * s;
    result[7] = 0.0f;

    result[8] = x * z * invC + y * s;
    result[9] = y * z * invC - x * s;
    result[10] = z * z * invC + c;
    result[11] = 0.0f;

    result[12] = 0.0f;
    result[13] = 0.0f;
    result[14] = 0.0f;
    result[15] = 1.0f;
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Rotating Cube", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Enable depth test and back face culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Build and compile shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Cube VAO and VBO setup
    GLuint VBO, VAO, EBO;
    glGenVertexArraysAPPLE(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArrayAPPLE(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArrayAPPLE(0);

    // Projection matrix
    float projectionMatrix[16];
    createPerspectiveMatrix(45.0f * (M_PI / 180.0f), 800.0f / 600.0f, 0.1f, 100.0f, projectionMatrix);

    // View matrix (camera position)
    float viewMatrix[16];
    createIdentityMatrix(viewMatrix);
    viewMatrix[14] = -2.0f; // Move camera back

    float angle = 0.0f;

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Rotation matrix
        float modelMatrix[16];
        createRotationMatrix(angle, 1.0f, 1.0f, 0.0f, modelMatrix);

        // Update rotation angle
        angle += 0.01f;

        // Use shader program
        glUseProgram(shaderProgram);

        // Set uniforms
        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projectionMatrix);

        // Render cube
        glBindVertexArrayAPPLE(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // Cleanup
    glDeleteVertexArraysAPPLE(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
}

//Simple cube no rotation

// // Vertex Shader Source Code
// const char* vertexShaderSource = R"(
// #version 330 core
// layout(location = 0) in vec3 aPos;
// void main() {
//     gl_Position = vec4(aPos, 1.0);
// }
// )";

// // Fragment Shader Source Code
// const char* fragmentShaderSource = R"(
// #version 330 core
// out vec4 FragColor;
// void main() {
//     FragColor = vec4(1.0, 1.0, 1.0, 1.0); // White color
// }
// )";

// float cubeVertices[] = {
//     // Front face
//     -0.5f, -0.5f,  0.5f,
//      0.5f, -0.5f,  0.5f,
//      0.5f,  0.5f,  0.5f,
//     -0.5f,  0.5f,  0.5f,
//     // Back face
//     -0.5f, -0.5f, -0.5f,
//      0.5f, -0.5f, -0.5f,
//      0.5f,  0.5f, -0.5f,
//     -0.5f,  0.5f, -0.5f
// };

// unsigned int cubeIndices[] = {
//     // Front face
//     0, 1, 2, 2, 3, 0,
//     // Back face
//     4, 5, 6, 6, 7, 4,
//     // Left face
//     0, 4, 7, 7, 3, 0,
//     // Right face
//     1, 5, 6, 6, 2, 1,
//     // Top face
//     3, 7, 6, 6, 2, 3,
//     // Bottom face
//     0, 4, 5, 5, 1, 0
// };

// void checkCompileErrors(GLuint shader, std::string type) {
//     GLint success;
//     GLchar infoLog[1024];
//     if (type == "PROGRAM") {
//         glGetProgramiv(shader, GL_LINK_STATUS, &success);
//         if (!success) {
//             glGetProgramInfoLog(shader, 1024, NULL, infoLog);
//             std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
//                       << infoLog << "\n";
//         }
//     } else {
//         glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
//         if (!success) {
//             glGetShaderInfoLog(shader, 1024, NULL, infoLog);
//             std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
//                       << infoLog << "\n";
//         }
//     }
// }

// int main() {
//     // Initialize GLFW
//     if (!glfwInit()) {
//         std::cerr << "Failed to initialize GLFW\n";
//         return -1;
//     }

//     // Create a GLFW window
//     GLFWwindow* window = glfwCreateWindow(800, 600, "Simple Cube", nullptr, nullptr);
//     if (!window) {
//         std::cerr << "Failed to create GLFW window\n";
//         glfwTerminate();
//         return -1;
//     }
//     glfwMakeContextCurrent(window);

//     // Build and compile shaders
//     GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//     glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
//     glCompileShader(vertexShader);
//     checkCompileErrors(vertexShader, "VERTEX");

//     GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//     glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
//     glCompileShader(fragmentShader);
//     checkCompileErrors(fragmentShader, "FRAGMENT");

//     GLuint shaderProgram = glCreateProgram();
//     glAttachShader(shaderProgram, vertexShader);
//     glAttachShader(shaderProgram, fragmentShader);
//     glLinkProgram(shaderProgram);
//     checkCompileErrors(shaderProgram, "PROGRAM");

//     glDeleteShader(vertexShader);
//     glDeleteShader(fragmentShader);

//     // Set up vertex data, buffers, and configure vertex attributes
//     GLuint VBO, VAO, EBO;
//     glGenVertexArraysAPPLE(1, &VAO);
//     glGenBuffers(1, &VBO);
//     glGenBuffers(1, &EBO);

//     glBindVertexArrayAPPLE(VAO);

//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(0);

//     glBindBuffer(GL_ARRAY_BUFFER, 0);
//     glBindVertexArrayAPPLE(0);

//     // Render loop
//     while (!glfwWindowShouldClose(window)) {
//         // Input handling
//         if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//             glfwSetWindowShouldClose(window, true);

//         // Render
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//         glUseProgram(shaderProgram);
//         glBindVertexArrayAPPLE(VAO);
//         glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
//         glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

//         glfwSwapBuffers(window);
//         glfwPollEvents();
//     }

//     // Cleanup
//     glDeleteVertexArraysAPPLE(1, &VAO);
//     glDeleteBuffers(1, &VBO);
//     glDeleteBuffers(1, &EBO);
//     glDeleteProgram(shaderProgram);

//     glfwDestroyWindow(window);
//     glfwTerminate();

//     return 0;
// }
