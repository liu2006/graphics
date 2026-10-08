#include "vulkan.h"

void createInstance(Vulkan *vulkan) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_REFRESH_RATE, 165);
    vulkan->window = glfwCreateWindow(1920, 1200, "glfw", NULL, NULL);
    assert(vulkan->window);
    glfwSetKeyCallback(vulkan->window, key_callback);
    printf("\033[32m-----> Success Created Window\033[0m\n");

    uint32_t glfwExtensionCount;
    const char **glfwExtensions =
        glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    uint32_t extensionCount;
    vulkan->result =
        vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
    assert(vulkan->result == VK_SUCCESS);
    VkExtensionProperties *extensionProperties =
        malloc(sizeof(*extensionProperties) * extensionCount);
    vulkan->result = vkEnumerateInstanceExtensionProperties(
							    NULL, &extensionCount, extensionProperties);
    assert(vulkan->result == VK_SUCCESS);

    uint32_t supportedCount = 0;
    for (uint32_t i = 0; i < glfwExtensionCount; ++i) {                       
	for (uint32_t j = 0; j < extensionCount; ++j) {
          if (strcmp(glfwExtensions[i],
                     extensionProperties[j].extensionName) == 0) {
              supportedCount += 1;
              break;
          } else
              continue;
        }
    }
    assert(supportedCount == glfwExtensionCount);

    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = "vulkan",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_4,
    };
    VkInstanceCreateInfo instanceInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
	.flags = 0,
  	.pApplicationInfo = &appInfo,
	.enabledExtensionCount = glfwExtensionCount,
	.ppEnabledExtensionNames = glfwExtensions};

    vulkan->result = vkCreateInstance(&instanceInfo, NULL, &vulkan->instance);
    assert(vulkan->result == VK_SUCCESS);
    printf("\033[32m-----> Success Created Instance\033[0m\n");
    vulkan->result = glfwCreateWindowSurface(vulkan->instance, vulkan->window, NULL,
                                             &vulkan->surface);
    assert(vulkan->result == VK_SUCCESS);
    free(extensionProperties);
}
