#include "vulkan.h"

int main(void) {
    Vulkan *vulkan = calloc(1, sizeof(*vulkan));
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_REFRESH_RATE, 165);
    vulkan->window = glfwCreateWindow(800, 600, "glfw", NULL, NULL);
    assert(vulkan->window);
    printf("\033[32m-----> Success Create Window\033[0m\n");
    
    createInstance(vulkan);
    vulkan->result = glfwCreateWindowSurface(vulkan->instance, vulkan->window, NULL,
                                             &vulkan->surface);
    assert(vulkan->result == VK_SUCCESS);
    createPhysicalDevice(vulkan);
    createDevice(vulkan);
    createSwapChain(vulkan);
    createPipeline(vulkan);
    
    while (glfwWindowShouldClose(vulkan->window)) {
	glfwPollEvents();
    }
    
    return 0;
}
