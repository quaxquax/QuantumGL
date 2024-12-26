#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <iostream>

void errorCallback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

void render() {
  glClear(GL_COLOR_BUFFER_BIT);
  
  glBegin(GL_TRIANGLES);
  glColor3f(1.0f, 0.0f, 0.0f);   // Red
  glVertex2f(-0.5f, -0.5f);
  glColor3f(0.0f, 1.0f, 0.0f);   // Green
  glVertex2f(0.5f, -0.5f);
  glColor3f(0.0f, 0.0f, 1.0f);   // Blue
  glVertex2f(0.0f, 0.5f);
  glEnd();
  // Instead of swapping buffers, flush the commands
  glFlush();
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwSetErrorCallback(errorCallback);
    
    // Set OpenGL version hints for OSX
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    // Disable double buffering
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
    glfwSwapInterval(0);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Single-Buffered OpenGL Example", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
 	render();
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
