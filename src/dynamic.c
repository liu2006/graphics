#include "vulkan.h"

typedef struct {
    float x, y, z;
} Vector;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if ((key == GLFW_KEY_ESCAPE || (key == GLFW_KEY_C && mods == GLFW_MOD_CONTROL)) && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void rotationMatrix(float (*matrix)[4][4], Vector rotationAxis, float angle)
{
    float length = sqrtf(rotationAxis.x * rotationAxis.x + rotationAxis.y * rotationAxis.y + rotationAxis.z * rotationAxis.z);
    float x = rotationAxis.x / length;
    float y = rotationAxis.y / length;
    float z = rotationAxis.z / length;

    (*matrix)[0][0] = (1 - cosf(angle)) * x * x + cosf(angle);
    (*matrix)[0][1] = (1 - cosf(angle)) * x * y - sinf(angle) * z;
    (*matrix)[0][2] = (1 - cosf(angle)) * x * z + sinf(angle) * y;
    (*matrix)[0][3] = 0;

    (*matrix)[1][0] = (1 - cosf(angle)) * x * y + sinf(angle) * z;
    (*matrix)[1][1] = (1 - cosf(angle)) * y * y + cosf(angle);
    (*matrix)[1][2] = (1 - cosf(angle)) * y * z - sinf(angle) * x;
    (*matrix)[1][3] = 0;

    (*matrix)[2][0] = (1 - cosf(angle)) * x * z - sinf(angle) * y;
    (*matrix)[2][1] = (1 - cosf(angle)) * y * z + sinf(angle) * x;
    (*matrix)[2][2] = (1 - cosf(angle)) * z * z + cosf(angle);
    (*matrix)[2][3] = 0;
    
    (*matrix)[3][0] = 0;
    (*matrix)[3][1] = 0;
    (*matrix)[3][2] = 0;
    (*matrix)[3][3] = 1;
}

void normalization(Vector *v)
{
    float length = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
    v->x = v->x / length;
    v->y = v->y / length;
    v->z = v->z / length;
}

Vector cross(Vector *v1, Vector *v2)
{
    Vector result = {v1->y * v2->z - v1->z * v2->y, v1->z * v2->x - v1->x * v2->z, v1->x * v2->y - v1->y * v2->x};
    return result;
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
    Vector cameraRight;
    Vector cameraUp;
    Vector cameraForward;
    
    float angle = time * (PI / 2);
    Vector rotationAxis = {0.5f, 0.4f, 0.1f};
    rotationMatrix(&ubo.model, rotationAxis, angle);

    Vector eye = {-2.0f, -2.0f, 2.0f};
    Vector center = {0.0f, 0.0f, 0.0f};
    Vector up = {0.0f, -1.0f, 0.0f};

    cameraForward = (Vector){center.x - eye.x, center.y - eye.y, center.z - eye.z};
    normalization(&cameraForward);
    cameraRight = cross(&up, &cameraForward);
    normalization(&cameraRight);
    cameraUp = cross(&cameraForward, &cameraRight);
}
