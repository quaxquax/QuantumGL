#include <GLES2/gl2.h>
#include "ExpressionEvaluator.h"
#include "BSPTree.h"
#include "LockGL.h"

#include GL_GL_H
#include "glu.h"
#include "gl3.h"
#include GL_GLU_H

#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <vector>

#include <emscripten/emscripten.h>


// Structure to store display lists
struct DisplayList {
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    GLsizei indexCount;
};

// Container to manage display lists
std::vector<DisplayList> displayLists;

// Function to generate display lists
GLuint glGenLists(GLsizei range) {
    GLuint start = displayLists.size();
    displayLists.resize(start + range);
    return start;
}

// Function to create a display list
void glNewList(GLuint list, GLenum mode, const GLfloat* vertices, const GLuint* indices, GLsizei vertexCount, GLsizei indexCount) {
    if (list >= displayLists.size()) {
        // Handle error: list index out of range
        return;
    }

    DisplayList& dl = displayLists[list];
    glGenVertexArrays(1, &dl.VAO);
    glGenBuffers(1, &dl.VBO);
    glGenBuffers(1, &dl.EBO);

    glBindVertexArray(dl.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, dl.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dl.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLuint), indices, GL_STATIC_DRAW);

    // Assuming a simple layout with positions and colors
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    dl.indexCount = indexCount;
}

// Function to call a display list
void glCallList(GLuint list) {
    if (list >= displayLists.size()) {
        // Handle error: list index out of range
        return;
    }

    const DisplayList& dl = displayLists[list];
    glBindVertexArray(dl.VAO);
    glDrawElements(GL_TRIANGLES, dl.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// Function to delete display lists
void glDeleteLists(GLuint list, GLsizei range) {
    for (GLsizei i = 0; i < range; ++i) {
        if (list + i < displayLists.size()) {
            DisplayList& dl = displayLists[list + i];
            glDeleteVertexArrays(1, &dl.VAO);
            glDeleteBuffers(1, &dl.VBO);
            glDeleteBuffers(1, &dl.EBO);
        }
    }
}

// Example usage

GLuint textureID;

void setupTexture() {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char redPixel[3] = {255, 10, 120}; // RGB color
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, redPixel);

    glEnable(GL_TEXTURE_2D);
}

void renderTriangle() {
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBegin(GL_TRIANGLES);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(0.5f, -0.5f);
    glTexCoord2f(0.5f, 1.0f); glVertex2f(0.0f, 0.5f);
    glEnd();
}

void render_frame() {
    glClear(GL_COLOR_BUFFER_BIT);
    renderTriangle();
}

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL with Emscripten", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    setupTexture();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // Generate and create a display list
    GLfloat vertices[] = {
        // positions         // colors
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // bottom right
       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,  // bottom left
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f   // top left 
    };

    GLuint indices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE, vertices, indices, sizeof(vertices) / sizeof(vertices[0]), sizeof(indices) / sizeof(indices[0]));

    // Use Emscripten's main loop
    emscripten_set_main_loop(render_frame, 0, 1);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
