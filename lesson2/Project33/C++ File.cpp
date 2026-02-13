#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

// Vertex Shader: يستقبل الموقع واللون ويطبق دوران
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertexColor;

uniform float angle; // زاوية الدوران

void main()
{
    // مصفوفة دوران حول المحور Z
    mat4 rotation = mat4(
        cos(angle), -sin(angle), 0.0, 0.0,
        sin(angle),  cos(angle), 0.0, 0.0,
        0.0,         0.0,        1.0, 0.0,
        0.0,         0.0,        0.0, 1.0
    );

    gl_Position = rotation * vec4(aPos, 1.0);
    vertexColor = aColor;
}
)";

// Fragment Shader: يعرض اللون المتدرج
const char* fragmentShaderSource = R"(
#version 330 core
in vec3 vertexColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(vertexColor, 1.0);
}
)";

int main()
{
    if (!glfwInit())
    {
        std::cout << "GLFW initialization failed\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Rotating Rectangle (Modern OpenGL)", NULL, NULL);
    if (!window)
    {
        std::cout << "Window creation failed\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW initialization failed\n";
        return -1;
    }

    // إحداثيات المستطيل + ألوان (تدرجات الأزرق)
    float vertices[] = {
        // الموقع              // اللون (R,G,B)
        -0.3f, -0.7f, 0.0f,    0.0f, 0.0f, 0.3f,
         0.3f, -0.7f, 0.0f,    0.0f, 0.0f, 0.6f,
         0.3f,  0.7f, 0.0f,    0.0f, 0.0f, 0.9f,
        -0.3f,  0.7f, 0.0f,    0.5f, 0.7f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // الموقع
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // اللون
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Compile Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Compile Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Shader Program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Uniform location
    GLint angleLoc = glGetUniformLocation(shaderProgram, "angle");

    float angle = 0.0f;

    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // خلفية بيضاء
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // تحديث زاوية الدوران (أبطأ)
        angle += 0.002f;
        glUniform1f(angleLoc, angle);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
