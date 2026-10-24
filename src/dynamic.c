#include "vulkan.h"

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if ((key == GLFW_KEY_ESCAPE || (key == GLFW_KEY_C && mods == GLFW_MOD_CONTROL)) && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void updateUniformBuffer(Vulkan *vulkan)
{
    static struct timespec startTime;
    static bool initialize = false;
    if (!initialize) {
        clock_gettime(CLOCK_MONOTONIC, &startTime);
        initialize = true;
    }
    struct timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);

    float time = (float)(currentTime.tv_sec - startTime.tv_sec) + (float)(currentTime.tv_nsec - startTime.tv_nsec) / 1e9f;
    uniformBufferObject ubo = {0};    
    
    printf("%.3f\n", time);
}
