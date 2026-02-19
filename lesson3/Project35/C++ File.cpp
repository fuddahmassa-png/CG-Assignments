#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// ================= متغيرات التحكم =================
float moveX = 0.0f;
float moveY = 0.0f;
float alphaValue = 0.6f;
float colorShift = 0.0f;

// ================= Key Callback =================
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) // يحدث مرة واحدة عند الضغط
    {
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, true);

        if (key == GLFW_KEY_W)
            moveX += 0.1f;

        if (key == GLFW_KEY_S)
            moveY += 0.1f;

        if (key == GLFW_KEY_C)
        {
            colorShift += 0.2f;
            if (colorShift > 1.0f)
                colorShift = 0.0f;
        }

        if (key == GLFW_KEY_T)
        {
            alphaValue -= 0.2f;
            if (alphaValue < 0.1f)
                alphaValue = 1.0f;
        }
    }
}

// ================= Resize Callback =================
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// ================= Vertex Shader =================
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"uniform vec3 offset;\n"
"uniform float colorShift;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos + offset, 1.0);\n"
"   ourColor = aColor + vec3(colorShift, 0.0, 0.0);\n"
"}\0";

// ================= Fragment Shader =================
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"uniform float uAlpha;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, uAlpha);\n"
"}\n\0";

int main()
{
    // ============ GLFW Init ============
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Interactive Project", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback); // ربط الكيبورد

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to init GLEW\n";
        return -1;
    }

    // ============ تفعيل العمق والشفافية ============
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ============ بناء الشيدر ============
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ============ جسمين بمحور Z مختلف ============
    float vertices[] = {
        // المثلث الأمامي (Z = 0.0)
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,

         // المثلث الخلفي (Z = 0.8)
         -0.2f, -0.2f, 0.8f,  0.0f, 0.0f, 1.0f,
          0.8f, -0.2f, 0.8f,  0.0f, 0.0f, 1.0f,
          0.3f,  0.8f, 0.8f,  0.0f, 0.0f, 1.0f
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // ============ الحصول على مواقع Uniform مرة واحدة ============
    glUseProgram(shaderProgram);
    int offsetLoc = glGetUniformLocation(shaderProgram, "offset");
    int alphaLoc = glGetUniformLocation(shaderProgram, "uAlpha");
    int colorLoc = glGetUniformLocation(shaderProgram, "colorShift");

    // ============ حلقة الرسم ============
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // رسم الخلفي أولاً
        glUniform3f(offsetLoc, 0.0f, 0.0f, 0.0f);
        glUniform1f(alphaLoc, 1.0f);
        glUniform1f(colorLoc, 0.0f);
        glDrawArrays(GL_TRIANGLES, 3, 3);

        // رسم الأمامي مع التفاعل
        glUniform3f(offsetLoc, moveX, moveY, 0.0f);
        glUniform1f(alphaLoc, alphaValue);
        glUniform1f(colorLoc, colorShift);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}