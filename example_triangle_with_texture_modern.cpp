// Modern OpenGL ES 2.0 triangle
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <cstddef>
#include <cstdio>

class ModernTexturedTriangle {
private:
    GLuint VBO, shaderProgram, textureID;
    GLint posAttrib, texCoordAttrib, texUniform;

    const char* vertexShaderSource = R"(
        attribute vec2 aPos;
        attribute vec2 aTexCoord;
        varying vec2 vTexCoord;
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            vTexCoord = aTexCoord;
        }
    )";

    const char* fragmentShaderSource = R"(
        precision mediump float;
        varying vec2 vTexCoord;
        uniform sampler2D uTexture;
        void main() {
            gl_FragColor = texture2D(uTexture, vTexCoord);
        }
    )";

    GLuint createShader(GLenum type, const char* source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
        return shader;
    }

    void setupTexture() {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        unsigned char redPixel[3] = {255, 10, 120}; // RGB color
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, redPixel);
    }

public:
    void initialize() {
        GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
        GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        float vertices[] = {
            // Position    // Texture coords
            -0.5f, -0.5f, 0.0f, 0.0f,  // Bottom left
             0.5f, -0.5f, 1.0f, 0.0f,  // Bottom right
             0.0f,  0.5f, 0.5f, 1.0f   // Top
        };

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        posAttrib = glGetAttribLocation(shaderProgram, "aPos");
        texCoordAttrib = glGetAttribLocation(shaderProgram, "aTexCoord");
        texUniform = glGetUniformLocation(shaderProgram, "uTexture");

        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(texCoordAttrib);

        setupTexture();
    }

    void draw() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(texUniform, 0);  // Use texture unit 0
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void cleanup() {
        glDeleteBuffers(1, &VBO);
        glDeleteTextures(1, &textureID);
        glDeleteProgram(shaderProgram);
    }
};

ModernTexturedTriangle triangle;

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
    emscripten_set_main_loop(render_frame, 0, true);

    return 0;
}

