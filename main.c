#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char const **items;
  size_t count;
  size_t capacity;
} String;

#define DYNAMIC_APPEND(vec, x)                                                 \
  do {                                                                         \
    if (vec.count >= vec.capacity) {                                           \
      if (vec.capacity == 0)                                                   \
        vec.capacity = 256;                                                    \
      else\
vec.capacity *= 2;                                                             \
      vec.items = realloc(vec.items, vec.capacity * sizeof(*vec.items));       \
    }                                                                          \
    vec.items[vec.count++] = x;                                                \
  } while (0)

#define CHECK_REQUIRED_EXTENSIONS(requiredCount, availableCount,               \
                                  requiredExtensions, availableExtensions,     \
                                  supportedCount)                              \
  do {                                                                         \
    for (uint32_t i = 0; i < availableCount; ++i) {                            \
      for (uint32_t j = 0; j < requiredCount; ++j) {                           \
        if (strcmp(availableExtensions[i].extensionName,                       \
                   requiredExtensions[j]) == 0) {                              \
          supportedCount += 1;                                                 \
          break;                                                               \
        } else                                                                 \
          continue;                                                            \
      }                                                                        \
    }                                                                          \
  } while (0)

int main(void) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "glfw", NULL, NULL);

    uint32_t glfwExtensionCount;
    const char **glfwExtensions =
	glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    uint32_t extensionCount;
    VkResult result;
    result = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
    VkExtensionProperties *extensionProperties = NULL;
    extensionProperties = realloc(extensionProperties,
                                  sizeof(*extensionProperties) * extensionCount);
    assert(extensionProperties);
    result = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount,
                                                    extensionProperties);
    assert(result == VK_SUCCESS);

    uint32_t supportedCount = 0;
    CHECK_REQUIRED_EXTENSIONS(glfwExtensionCount, extensionCount, glfwExtensions,
                              extensionProperties, supportedCount);
    assert(supportedCount == glfwExtensionCount);

    VkApplicationInfo appInfo = {
	.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
	.pApplicationName = "vulkan",
	.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
	.pEngineName = "No engine",
	.engineVersion = VK_MAKE_VERSION(1, 0, 0),
	.apiVersion = VK_API_VERSION_1_4,
    };

    VkInstanceCreateInfo instanceInfo = {
	.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
	.pApplicationInfo = &appInfo,
	.enabledExtensionCount = glfwExtensionCount,
	.ppEnabledExtensionNames = glfwExtensions};

    VkInstance instance;
    result = vkCreateInstance(&instanceInfo, NULL, &instance);
    assert(result == VK_SUCCESS);
    printf("\033[32m-----> Success Create Instance\033[0m\n");

    VkSurfaceKHR surface;
    result = glfwCreateWindowSurface(instance, window, NULL, &surface);
    assert(result == VK_SUCCESS);

    VkPhysicalDevice physicalDevice;
    uint32_t physicalDeviceCount;
    result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);
    assert(result == VK_SUCCESS);

    VkPhysicalDevice *physicalDevices =
	malloc(sizeof(*physicalDevices) * physicalDeviceCount);
    result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount,
					physicalDevices);
    assert(result == VK_SUCCESS);
    printf("\033[32m-----> Success Found Physical Device\033[0m\n");
    printf("\033[35m- Physical Device Count: %u\033[0m\n", physicalDeviceCount);
    VkPhysicalDeviceFeatures features = {0};
    VkPhysicalDeviceProperties properties = {0};
    for (uint32_t i = 0; i < physicalDeviceCount; ++i) {
	vkGetPhysicalDeviceFeatures(physicalDevices[i], &features);
        vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            physicalDevice = physicalDevices[i];
            printf("\033[35m- [ x ] Physical Device Name: %s\033[0m\n",
		   properties.deviceName);
            break;
        }
        physicalDevice = physicalDevices[i];
        printf("\033[35m- [ X ] Physical Device Name: %s\033[0m\n",
               properties.deviceName);
    }

    uint32_t queueFamilyPropertyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,
                                             &queueFamilyPropertyCount, NULL);
    VkQueueFamilyProperties *queueFamilyPropeties =
	malloc(sizeof(*queueFamilyPropeties) * queueFamilyPropertyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
					     physicalDevice, &queueFamilyPropertyCount, queueFamilyPropeties);

    bool supportedGraphics;
    uint32_t graphicsIndex;
    for (uint32_t i = 0; i < queueFamilyPropertyCount; ++i) {
	if (queueFamilyPropeties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
	    supportedGraphics = true;
	    graphicsIndex = i;
	    break;
	}
    }
    assert(supportedGraphics);
    printf("\033[32m-----> Queue Family Supported Graphics\033[0m\n");
    printf("\033[35m- Graphics Index: %u\033[0m\n", graphicsIndex);

    uint32_t deviceExtensionPropertyCount;
    result = vkEnumerateDeviceExtensionProperties(
						  physicalDevice, NULL, &deviceExtensionPropertyCount, NULL);
    extensionProperties =
	realloc(extensionProperties,
		sizeof(*extensionProperties) * deviceExtensionPropertyCount);

    result = vkEnumerateDeviceExtensionProperties(physicalDevice, NULL,
                                                  &deviceExtensionPropertyCount,
                                                  extensionProperties);

    assert(result == VK_SUCCESS);
    supportedCount = 0;
    const char *requiredDeviceExtensions[] = {"VK_KHR_swapchain"};
    uint32_t requiredDeviceExtensionCount =
	sizeof(requiredDeviceExtensions) / sizeof(requiredDeviceExtensions[0]);
    CHECK_REQUIRED_EXTENSIONS(
			      requiredDeviceExtensionCount, deviceExtensionPropertyCount,
			      requiredDeviceExtensions, extensionProperties, supportedCount);
    assert(supportedCount == requiredDeviceExtensionCount);
    printf("\033[32m---> Physical Device Supported Swap Chain\033[0m\n");

    VkDevice device;
    uint32_t queueCount;
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {
	.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
	.queueFamilyIndex = graphicsIndex,
	.queueCount = 1,
	.pQueuePriorities = &queuePriority};

    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT dynamicState = {
	.sType =
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT,
	.extendedDynamicState = true,
    };

    VkPhysicalDeviceVulkan13Features vulkan13Features = {
	.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
	.pNext = &dynamicState,
	.dynamicRendering = true};

    VkPhysicalDeviceVulkan11Features vulkan11Features = {
	.shaderDrawParameters = true,
	.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
	.pNext = &vulkan13Features};

    VkPhysicalDeviceFeatures devicefeatures = {0};

    VkPhysicalDeviceFeatures2 deviceFeatures2 = {
	.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
	.pNext = &vulkan11Features,
	.features = features};

    VkDeviceCreateInfo deviceCreateInfo = {
	.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
	.queueCreateInfoCount = 1,
	.pQueueCreateInfos = &queueCreateInfo,
	.enabledExtensionCount = requiredDeviceExtensionCount,
	.ppEnabledExtensionNames = requiredDeviceExtensions,
	.pEnabledFeatures = &features};

    result = vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &device);
    assert(result == VK_SUCCESS);
    printf("\033[32m---> Success Create Logic Device\033[0m\n");
    VkQueue queue;
    vkGetDeviceQueue(device, graphicsIndex, 0, &queue);
    printf("\033[32m---> Success Create Graphics Queue\033[0m\n");

    VkSurfaceCapabilitiesKHR surfaceCapabilities = {0};
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface,
                                                       &surfaceCapabilities);
    assert(result == VK_SUCCESS);
    uint32_t surfaceFormatCount;
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface,
                                                  &surfaceFormatCount, NULL);
    assert(result == VK_SUCCESS);
    VkSurfaceFormatKHR *surfaceFormats =
        malloc(sizeof(*surfaceFormats) * surfaceFormatCount);

    result = vkGetPhysicalDeviceSurfaceFormatsKHR(
						  physicalDevice, surface, &surfaceFormatCount, surfaceFormats);
    assert(result == VK_SUCCESS);
    uint32_t formatIndex;
    bool supportedSrgb, supportedNonlinear;
    for (uint32_t i = 0; i < surfaceFormatCount; ++i) {
	if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB) {
            supportedSrgb = true;
            if (surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
		supportedNonlinear = true;
                formatIndex = i;
                break;
            }
            formatIndex = i;
        }
    }
    if (supportedSrgb) {
	printf("\033[32m---> Supported SRGB Format\033[0m\n");
    } else {
	printf("\033[31m---> Not Supported SRGB Format\033[0m\n");
    }
    if (supportedNonlinear) {
 	printf("\033[32m---> Supported Nonlinear ColorSpace\033[0m\n");
    } else {
	printf("\033[31m---> Not Supported Nolinear ColorSpace\033[0m\n");
    }
    
    uint32_t presentModeCount;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface,
                                                       &presentModeCount, NULL);
    assert(result == VK_SUCCESS);        
    VkPresentModeKHR *presentModes =
        malloc(sizeof(*presentModes) * presentModeCount);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(
						       physicalDevice, surface, &presentModeCount, presentModes);

    assert(result == VK_SUCCESS);
    VkPresentModeKHR presentMode;
    bool supportedFifo, supportedMailbox;
    for (uint32_t i = 0; i < presentModeCount; ++i) {
	if (presentModes[i] == VK_PRESENT_MODE_FIFO_KHR) {
            presentMode = VK_PRESENT_MODE_FIFO_KHR;
            supportedFifo = true;
        }
        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	    supportedMailbox = true;
	    break;
        }
    }
    assert(supportedFifo || supportedMailbox);
    if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
	printf("\033[32m---> Usage Mailbox Present Mode\033[0m\n");
    else if (presentMode == VK_PRESENT_MODE_FIFO_KHR) {
        printf("\033[33m---> Usage Fifo Present Mode\033[0m\n");
    }
    
    int width, height;
    VkExtent2D extent;
    glfwGetFramebufferSize(window, &width, &height);
    extent.width = width;
    extent.height = height;

    VkSwapchainCreateInfoKHR swapChainInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .surface = surface,
        .minImageCount = surfaceCapabilities.minImageCount,
        .imageFormat = surfaceFormats[formatIndex].format,
        .imageColorSpace = surfaceFormats[formatIndex].colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &graphicsIndex,
        .preTransform = surfaceCapabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = true,
        .oldSwapchain = VK_NULL_HANDLE};
    
    VkSwapchainKHR swapChain;
    result = vkCreateSwapchainKHR(device, &swapChainInfo, NULL, &swapChain);
    assert(result == VK_SUCCESS);
    printf("\033[32m---> Success Create Swap Chain\033[0m\n");
    
    uint32_t imageCount;
    result = vkGetSwapchainImagesKHR(device, swapChain, &imageCount, NULL);
    VkImage *swapChainImages = malloc(sizeof(*swapChainImages) * imageCount);
    result = vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages);
    assert(result == VK_SUCCESS);
    printf("\033[35m- Swap Chain Image Count: %u\033[0m\n", imageCount);

    while (glfwWindowShouldClose(window)) {
	glfwPollEvents();
    }
    
    free(swapChainImages);
    free(surfaceFormats);
    free(queueFamilyPropeties);
    free(physicalDevices);
    free(extensionProperties);
    vkDestroySwapchainKHR(device, swapChain, NULL);
    vkDestroySurfaceKHR(instance, surface, NULL);
    vkDestroyDevice(device, NULL);
    vkDestroyInstance(instance, NULL);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
