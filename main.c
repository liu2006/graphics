#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

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


typedef struct {
    uint32_t extensionCount;
    VkExtensionProperties *extensionProperties;
    VkResult result;
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    VkQueue queue;
} Vulkan;

void *createWindow(void *arg) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_REFRESH_RATE, 165);
    *(GLFWwindow **)(arg) = glfwCreateWindow(800, 600, "glfw", NULL, NULL);
    assert(arg);
    printf("\033[32m-----> Success Create Window\033[0m\n");
    return NULL;
}

void *createInstance(void *arg) {
    Vulkan *vulkan = arg;
    uint32_t glfwExtensionCount;
    const char **glfwExtensions =
	glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    vulkan->result = vkEnumerateInstanceExtensionProperties(
							    NULL, &vulkan->extensionCount, NULL);
    assert(vulkan->result == VK_SUCCESS);
    vulkan->extensionProperties =
        realloc(vulkan->extensionProperties,
                sizeof(*vulkan->extensionProperties) * vulkan->extensionCount);
    assert(vulkan->extensionProperties);
    vulkan->result = vkEnumerateInstanceExtensionProperties(
							    NULL, &vulkan->extensionCount, vulkan->extensionProperties);
    assert(vulkan->result == VK_SUCCESS);
    uint32_t supportedCount = 0;
    CHECK_REQUIRED_EXTENSIONS(glfwExtensionCount, vulkan->extensionCount, glfwExtensions,
                              vulkan->extensionProperties, supportedCount);
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

    vulkan->result = vkCreateInstance(&instanceInfo, NULL, &vulkan->instance);
    assert(vulkan->result == VK_SUCCESS);
    printf("\033[32m-----> Success Create Instance\033[0m\n");
    return NULL;
}

int main(void) {
    glfwInit();
    Vulkan *vulkan = malloc(sizeof(*vulkan));
    GLFWwindow *window;
    pthread_t windowThread;
    pthread_create(&windowThread, NULL, createWindow, &window);

    pthread_t instanceThread;
    pthread_create(&instanceThread, NULL, createInstance, vulkan);

    pthread_join(windowThread, NULL);
    pthread_join(instanceThread, NULL);
    vulkan->result = glfwCreateWindowSurface(vulkan->instance, window, NULL,
                                             &vulkan->surface);
    assert(vulkan->result == VK_SUCCESS);

    uint32_t physicalDeviceCount;
    vulkan->result = vkEnumeratePhysicalDevices(vulkan->instance, &physicalDeviceCount, NULL);
    assert(vulkan->result == VK_SUCCESS);

    VkPhysicalDevice *physicalDevices =
	malloc(sizeof(*physicalDevices) * physicalDeviceCount);
    vulkan->result = vkEnumeratePhysicalDevices(vulkan->instance, &physicalDeviceCount,
					physicalDevices);
    assert(vulkan->result == VK_SUCCESS);
    printf("\033[32m-----> Success Found Physical Device\033[0m\n");
    printf("\033[35m- Physical Device Count: %u\033[0m\n", physicalDeviceCount);
    VkPhysicalDeviceFeatures features = {0};
    VkPhysicalDeviceProperties properties = {0};
    for (uint32_t i = 0; i < physicalDeviceCount; ++i) {
	vkGetPhysicalDeviceFeatures(physicalDevices[i], &features);
        vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            vulkan->physicalDevice = physicalDevices[i];
            printf("\033[35m- [ x ] Physical Device Name: %s\033[0m\n",
		   properties.deviceName);
            break;
        }
        vulkan->physicalDevice = physicalDevices[i];
        printf("\033[35m- [ X ] Physical Device Name: %s\033[0m\n",
               properties.deviceName);
    }

    uint32_t queueFamilyPropertyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(vulkan->physicalDevice,
                                             &queueFamilyPropertyCount, NULL);
    VkQueueFamilyProperties *queueFamilyPropeties =
	malloc(sizeof(*queueFamilyPropeties) * queueFamilyPropertyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
					     vulkan->physicalDevice, &queueFamilyPropertyCount, queueFamilyPropeties);

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
    vulkan->result = vkEnumerateDeviceExtensionProperties(
						  vulkan->physicalDevice, NULL, &deviceExtensionPropertyCount, NULL);
    vulkan->extensionProperties =
	realloc(vulkan->extensionProperties,
		sizeof(*vulkan->extensionProperties) * deviceExtensionPropertyCount);

    vulkan->result = vkEnumerateDeviceExtensionProperties(vulkan->physicalDevice, NULL,
                                                  &deviceExtensionPropertyCount,
                                                  vulkan->extensionProperties);

    assert(vulkan->result == VK_SUCCESS);
    uint32_t supportedCount = 0;
    const char *requiredDeviceExtensions[] = {"VK_KHR_swapchain"};
    uint32_t requiredDeviceExtensionCount =
	sizeof(requiredDeviceExtensions) / sizeof(requiredDeviceExtensions[0]);
    CHECK_REQUIRED_EXTENSIONS(
			      requiredDeviceExtensionCount, deviceExtensionPropertyCount,
			      requiredDeviceExtensions, vulkan->extensionProperties, supportedCount);
    assert(supportedCount == requiredDeviceExtensionCount);
    printf("\033[32m---> Physical Device Supported Swap Chain\033[0m\n");


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

    vulkan->result = vkCreateDevice(vulkan->physicalDevice, &deviceCreateInfo, NULL, &vulkan->device);
    assert(vulkan->result == VK_SUCCESS);
    printf("\033[32m---> Success Create Logic Device\033[0m\n");

    vkGetDeviceQueue(vulkan->device, graphicsIndex, 0, &vulkan->queue);
    printf("\033[32m---> Success Create Graphics Queue\033[0m\n");

    VkSurfaceCapabilitiesKHR surfaceCapabilities = {0};
    vulkan->result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan->physicalDevice, vulkan->surface,
                                                       &surfaceCapabilities);
    assert(vulkan->result == VK_SUCCESS);
    uint32_t surfaceFormatCount;
    vulkan->result = vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan->physicalDevice, vulkan->surface,
                                                  &surfaceFormatCount, NULL);
    assert(vulkan->result == VK_SUCCESS);
    VkSurfaceFormatKHR *surfaceFormats =
        malloc(sizeof(*surfaceFormats) * surfaceFormatCount);

    vulkan->result = vkGetPhysicalDeviceSurfaceFormatsKHR(
							  vulkan->physicalDevice, vulkan->surface, &surfaceFormatCount, surfaceFormats);
    assert(vulkan->result == VK_SUCCESS);
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
    vulkan->result = vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan->physicalDevice, vulkan->surface,
                                                       &presentModeCount, NULL);
    assert(vulkan->result == VK_SUCCESS);        
    VkPresentModeKHR *presentModes =
        malloc(sizeof(*presentModes) * presentModeCount);
    vulkan->result = vkGetPhysicalDeviceSurfacePresentModesKHR(
						       vulkan->physicalDevice, vulkan->surface, &presentModeCount, presentModes);

    assert(vulkan->result == VK_SUCCESS);
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
        .surface = vulkan->surface,
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

    vulkan->result = vkCreateSwapchainKHR(vulkan->device, &swapChainInfo, NULL, &vulkan->swapChain);
    assert(vulkan->result == VK_SUCCESS);
    printf("\033[32m---> Success Create Swap Chain\033[0m\n");
    
    uint32_t imageCount;
    vulkan->result = vkGetSwapchainImagesKHR(vulkan->device, vulkan->swapChain,
                                             &imageCount, NULL);
    assert(vulkan->result == VK_SUCCESS);
    VkImage *swapChainImages = malloc(sizeof(*swapChainImages) * imageCount);
    vulkan->result = vkGetSwapchainImagesKHR(vulkan->device, vulkan->swapChain, &imageCount, swapChainImages);
    assert(vulkan->result == VK_SUCCESS);
    printf("\033[35m- Swap Chain Image Count: %u\033[0m\n", imageCount);

    VkImageView *imageViews = malloc(sizeof(*imageViews) * imageCount);
    VkImageViewCreateInfo imageViewCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = surfaceFormats[formatIndex].format,
        .components = {.r = VK_COMPONENT_SWIZZLE_R,
                       .g = VK_COMPONENT_SWIZZLE_G,
                       .b = VK_COMPONENT_SWIZZLE_B,
                       .a = VK_COMPONENT_SWIZZLE_A},
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                             .baseMipLevel = 0,
                             .levelCount = 1,
                             .baseArrayLayer = 0,
                             .layerCount = 1}};

    for (uint32_t i = 0; i < imageCount; ++i) {
	imageViewCreateInfo.image = swapChainImages[i];
    }

    vulkan->result = vkCreateImageView(vulkan->device, &imageViewCreateInfo,
                                       NULL, imageViews);
    assert(vulkan->result == VK_SUCCESS);
    printf("\033[32m-----> Success Create Image Views\033[0m\n");

    
    while (glfwWindowShouldClose(window)) {
	glfwPollEvents();
    }

    for (uint32_t i = 0; i < imageCount; ++i) {
	vkDestroyImageView(vulkan->device, imageViews[i], NULL);
    }
    free(imageViews);
    free(swapChainImages);
    vkDestroySwapchainKHR(vulkan->device, vulkan->swapChain, NULL);
    vkDestroySurfaceKHR(vulkan->instance, vulkan->surface, NULL);
    vkDestroyDevice(vulkan->device, NULL);
    vkDestroyInstance(vulkan->instance, NULL);
    free(surfaceFormats);
    free(queueFamilyPropeties);
    free(physicalDevices);
    free(vulkan->extensionProperties);
    free(vulkan);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
