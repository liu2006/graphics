#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define FLIGHT_FRAMES 2

#define DETERMINE_MSAA_SAMPLE(proprties)\
({\
    VkSampleCountFlags _samples = properties.limits.framebufferColorSampleCounts &\
	properties.limits.framebufferDepthSampleCounts;\
    VkSampleCountFlagBits _result;\
    if (_samples & VK_SAMPLE_COUNT_64_BIT)\
	_result = VK_SAMPLE_COUNT_64_BIT;\
    else if (_samples & VK_SAMPLE_COUNT_32_BIT)\
	_result = VK_SAMPLE_COUNT_32_BIT;\
    else if (_samples & VK_SAMPLE_COUNT_16_BIT)\
	_result = VK_SAMPLE_COUNT_16_BIT;\
    else if (_samples & VK_SAMPLE_COUNT_8_BIT)\
	_result = VK_SAMPLE_COUNT_8_BIT;\
    else if (_samples & VK_SAMPLE_COUNT_4_BIT)\
	_result = VK_SAMPLE_COUNT_4_BIT;\
    else if (_samples & VK_SAMPLE_COUNT_2_BIT)\
	_result = VK_SAMPLE_COUNT_2_BIT;\
    else\
	_result = VK_SAMPLE_COUNT_1_BIT;\
    _result;\
})\

#define CHECK_EXTENSIONS(requiredExtensionCount, requiredExtensions, extensionCount, extensionProperties, supportedCount)\
do {\
    for (uint32_t i = 0; i < requiredExtensionCount; i++) {\
	for (uint32_t j = 0; j < extensionCount; j++) {\
            if (strcmp(requiredExtensions[i],\
                       extensionProperties[j].extensionName) == 0) {\
		supportedCount++;\
		break;\
            } else\
		continue;\
	}\
    }\
} while (0)

#define APPEND_VECTOR(vector, value)\
do {\
    if (vector.count >= vector.capacity) {\
	vector.capacity =\
            vector.capacity ? vector.capacity * 2 : 1;\
	vector.items = realloc(vector.items, vector.capacity * sizeof(*vector.items));\
    }\
    vector.items[vector.count++] = value;\
} while(0)\

typedef struct {
    float pos[2];
    float color[3];
} Vertex;

typedef struct {
    Vertex *items;
    size_t count;
    size_t capacity;
} Vertices;

typedef struct {
    uint32_t value;
} Index;

typedef struct {
    Index *items;
    size_t count;
    size_t capacity;
} Indices;

typedef struct {
    const char **items;
    size_t count;
    size_t capacity;
} Extensions;

typedef struct {
    GLFWwindow *window;
    Extensions requiredDeviceExtensions;
    VkResult result;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    uint32_t graphicsIndex;
    VkDevice device;
    VkQueue graphicsQueue;
    VkSurfaceFormatKHR *surfaceFormats;
    uint32_t formatIndex;
    VkExtent2D swapChainextent;
    uint32_t imageCount;
    VkImage *swapChainImages;
    uint32_t imageIndex;
    VkImage msaaImage;
    VkDeviceMemory msaaImageMemory;
    VkImageView msaaImageView;
    VkImageView *imageViews;
    VkSampleCountFlagBits msaaSample;
    VkSwapchainKHR swapChain;
    long int spirvSize;
    uint32_t *spirvBuffer;
    VkViewport viewport;
    VkRect2D scissor;
    VkPipeline graphicsPipeline;
    Vertices vertices; 
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    Indices indices;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    VkCommandPool commandPool;
    VkCommandBuffer *commandBuffers;
    uint32_t commandBufferCount;
    VkSemaphore *presentSemaphores;
    VkSemaphore *renderSemaphores;
    VkFence *fences;
    uint32_t frameIndex;
} Vulkan;

void createInstance(Vulkan *vulkan);
void createPhysicalDevice(Vulkan *vulkan);
void createDevice(Vulkan *vulkan);
void createSwapChain(Vulkan *vulkan);
void createPipeline(Vulkan *vulkan);
void createVertexBuffer(Vulkan *vulkan);
void createIndexBuffer(Vulkan *vulkan);
void createCommandBuffer(Vulkan *vulkan);
void recordCommandBuffer(Vulkan *vulkan);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
