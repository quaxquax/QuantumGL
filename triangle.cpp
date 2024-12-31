#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <cstddef>
#include <string.h>
#include <cstdio>

class ModernTriangle {
private:
    GLuint VBO;
    GLuint shaderProgram;
    
    GLuint createShader(GLenum type, const char* source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
        
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            printf("Shader compilation failed: %s\n", infoLog);
            return 0;
        }
        return shader;
    }

public:
    const char* vertexShaderSource = R"(
        attribute vec3 aPos;
        attribute vec3 aColor;
        varying vec3 vColor;
        void main() {
            gl_Position = vec4(aPos, 1.0);
            vColor = aColor;
        }
    )";

    const char* fragmentShaderSource = R"(
        precision mediump float;
        varying vec3 vColor;
        void main() {
            gl_FragColor = vec4(vColor, 1.0);
        }
    )";

    void initialize() {
        printf("Initializing ModernTriangle\n");
        
        // Create and compile shaders
        GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
        GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

        // Create shader program
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        // Check for linking errors
        GLint success;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            printf("Shader program linking failed: %s\n", infoLog);
        }

        // Clean up shaders
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Vertex data: position (x,y,z) and color (r,g,b) for each vertex
        float vertices[] = {
            -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Red vertex
             0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // Green vertex
             0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // Blue vertex
        };

        // Create and bind VBO
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Set up vertex attributes
        GLint posAttrib = glGetAttribLocation(shaderProgram, "aPos");
        GLint colorAttrib = glGetAttribLocation(shaderProgram, "aColor");

        printf("Position attribute location: %d\n", posAttrib);
        printf("Color attribute location: %d\n", colorAttrib);

        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(posAttrib);

        glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
                             (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(colorAttrib);
    }

    void draw() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        
        // Re-enable attributes every frame (WebGL1 best practice)
        GLint posAttrib = glGetAttribLocation(shaderProgram, "aPos");
        GLint colorAttrib = glGetAttribLocation(shaderProgram, "aColor");
        
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(posAttrib);
        
        glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
                             (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(colorAttrib);

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void cleanup() {
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
    }
};

ModernTriangle triangle;

void render_frame() {
    triangle.draw();
}

int main() {
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.majorVersion = 1;
    attrs.minorVersion = 0;
    
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context = emscripten_webgl_create_context("#canvas", &attrs);
    emscripten_webgl_make_context_current(context);

    triangle.initialize();

    // Set up the render loop
    emscripten_set_main_loop(render_frame, 0, true);

    return 0;
}
