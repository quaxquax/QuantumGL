#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <emscripten/emscripten.h>
#include <stdio.h>

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

    // Use Emscripten's main loop
    emscripten_set_main_loop(render_frame, 0, 1);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


