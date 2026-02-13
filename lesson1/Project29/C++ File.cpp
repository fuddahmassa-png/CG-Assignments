#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    // 1. تهيئة GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // 2. إنشاء نافذة
    GLFWwindow* window = glfwCreateWindow(800, 600, "Empty Window", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    // 3. جعل السياق (Context) مرتبط بالنافذة
    glfwMakeContextCurrent(window);

    // 4. تهيئة GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return -1;
    }

    // 5. الحلقة الرئيسية (Main Loop)
    while (!glfwWindowShouldClose(window)) {
        // تنظيف الشاشة (لون الخلفية)
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // تبديل الـ Buffers
        glfwSwapBuffers(window);

        // معالجة الأحداث (Events)
        glfwPollEvents();
    }

    // 6. إنهاء GLFW
    glfwTerminate();
    return 0;
}
