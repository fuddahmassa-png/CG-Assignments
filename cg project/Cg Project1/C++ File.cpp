#include <iostream>
#include <math.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float carPosX = -1.2f;
bool moveCar = false;


const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"uniform float uTime;\n"
"uniform float carOffset;\n"
"uniform int objType;\n"
"void main() {\n"
"   vec3 p = aPos;\n"
"   if(objType == 1) {\n" // حركة الشمس
"       p.x += cos(uTime * 0.4) * 0.85;\n"
"       p.y += sin(uTime * 0.4) * 1.1;\n"
"   } else if(objType == 2) {\n" // حركة السيارة والعجلات
"       p.x += carOffset;\n"
"   }\n"
"   gl_Position = vec4(p, 1.0);\n"
"   ourColor = aColor;\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"uniform float uTime;\n"
"uniform bool isSky;\n"
"void main() {\n"
"   if(isSky) {\n"
"       // تأثير الليل والنهار متزامن مع حركة الشمس\n"
"       float dayIntensity = clamp(sin(uTime * 0.4) + 0.3, 0.0, 1.0);\n"
"       vec3 skyDay = vec3(0.4, 0.6, 0.9);\n"
"       vec3 skyNight = vec3(0.05, 0.05, 0.12);\n"
"       FragColor = vec4(mix(skyNight, skyDay, dayIntensity), 1.0);\n"
"   } else {\n"
"       FragColor = vec4(ourColor, 1.0);\n"
"   }\n"
"}\n\0";

// دالة رسم الدوائر (للشمس والعجلات)
void createCircle(std::vector<float>& v, float cx, float cy, float rad, float r, float g, float b) {
    v.push_back(cx); v.push_back(cy); v.push_back(0); v.push_back(r); v.push_back(g); v.push_back(b);
    for (int i = 0; i <= 30; i++) {
        float ang = 2.0f * 3.14159f * i / 30.0f;
        v.push_back(cx + rad * cos(ang)); v.push_back(cy + rad * sin(ang)); v.push_back(0);
        v.push_back(r); v.push_back(g); v.push_back(b);
    }
}

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Masa's Final Project - InfoEng", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glewInit();

    // بناء برنامج الشيدر
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER); glShaderSource(vs, 1, &vertexShaderSource, NULL); glCompileShader(vs);
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(fs, 1, &fragmentShaderSource, NULL); glCompileShader(fs);
    unsigned int prog = glCreateProgram(); glAttachShader(prog, vs); glAttachShader(prog, fs); glLinkProgram(prog);
    glDeleteShader(vs); glDeleteShader(fs);

    // --- مصفوفة النقاط ---
    std::vector<float> verts = {
        // 1. خلفية السماء (0-3) - لرسم تأثير الليل والنهار
        -1,1,0, 0,0,0,  1,1,0, 0,0,0,  1,-1,0, 0,0,0,  -1,-1,0, 0,0,0,
        // 2. الأرض (4-7)
        -1.0,-0.4,0, 0.1,0.5,0.1,  1.0,-0.4,0, 0.1,0.5,0.1,  1.0,-1.0,0, 0.0,0.2,0.0, -1.0,-1.0,0, 0.0,0.2,0.0,
        // 3. جسم البيت (8-11)
        -0.3,-0.4,0, 0.6,0.4,0.2,  0.3,-0.4,0, 0.6,0.4,0.2,  0.3,-0.8,0, 0.5,0.3,0.1, -0.3,-0.8,0, 0.5,0.3,0.1,
        // 4. السقف (12-14)
        -0.35,-0.4,0, 0.8,0.1,0.1, 0.35,-0.4,0, 0.8,0.1,0.1, 0.0, 0.0,0, 0.9,0.2,0.2,
        // 5. الباب (15-18)
        -0.15,-0.8,0, 0.3,0.1,0.0, -0.05,-0.8,0, 0.3,0.1,0.0, -0.05,-0.55,0, 0.3,0.1,0.0, -0.15,-0.55,0, 0.3,0.1,0.0,
        // 6. الشباك (19-22)
        0.05,-0.5,0, 0.7,0.9,1.0,  0.2,-0.5,0, 0.7,0.9,1.0,  0.2,-0.65,0, 0.7,0.9,1.0,  0.05,-0.65,0, 0.7,0.9,1.0,
        // 7. جسم السيارة (23-26)
        -0.15,-0.85,0, 0,0,0.6,    0.15,-0.85,0, 0,0,0.6,    0.15,-0.92,0, 0,0,0.6,   -0.15,-0.92,0, 0,0,0.6
    };

    int sunStart = verts.size() / 6;
    createCircle(verts, 0.0f, 0.0f, 0.12f, 1.0f, 0.9f, 0.0f); // الشمس
    int wheelStart = verts.size() / 6;
    createCircle(verts, -0.08f, -0.92f, 0.03f, 0.1f, 0.1f, 0.1f); // عجلة 1
    createCircle(verts, 0.08f, -0.92f, 0.03f, 0.1f, 0.1f, 0.1f);  // عجلة 2

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
    glBindVertexArray(VAO); glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) moveCar = true;
        if (moveCar) { carPosX += 0.002f; if (carPosX > 1.2f) carPosX = -1.2f; }

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(prog);

        float currentTime = (float)glfwGetTime();
        glUniform1f(glGetUniformLocation(prog, "uTime"), currentTime);
        glUniform1f(glGetUniformLocation(prog, "carOffset"), carPosX);
        int skyLoc = glGetUniformLocation(prog, "isSky");
        int typeLoc = glGetUniformLocation(prog, "objType");

        glBindVertexArray(VAO);

        // 1. رسم السماء (لتغيير اللون)
        glUniform1i(skyLoc, true); glUniform1i(typeLoc, 0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // 2. رسم الشمس (خلف الأرض والبيت)
        glUniform1i(skyLoc, false); glUniform1i(typeLoc, 1);
        glDrawArrays(GL_TRIANGLE_FAN, sunStart, 32);

        // 3. رسم الأجسام الثابتة (الأرض، البيت، السقف، الباب، الشباك)
        glUniform1i(typeLoc, 0);
        glDrawArrays(GL_TRIANGLE_FAN, 4, 4);  // أرض
        glDrawArrays(GL_TRIANGLE_FAN, 8, 4);  // بيت
        glDrawArrays(GL_TRIANGLES, 12, 3);    // سقف
        glDrawArrays(GL_TRIANGLE_FAN, 15, 4); // باب
        glDrawArrays(GL_TRIANGLE_FAN, 19, 4); // شباك

        // 4. رسم السيارة (فوق الأرض)
        glUniform1i(typeLoc, 2);
        glDrawArrays(GL_TRIANGLE_FAN, 23, 4);
        glDrawArrays(GL_TRIANGLE_FAN, wheelStart, 32);
        glDrawArrays(GL_TRIANGLE_FAN, wheelStart + 32, 32);

        glfwSwapBuffers(window); glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO); glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
