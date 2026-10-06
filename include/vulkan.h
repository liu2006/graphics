#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

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
    VkQueue queue;
    VkSurfaceFormatKHR *surfaceFormats;
    uint32_t formatIndex;
    VkExtent2D extent;
    uint32_t imageCount;
    VkImageView *imageViews;
    VkSwapchainKHR swapChain;
    VkShaderModule shaderModule;
    long int spirvSize;
    uint32_t *spirvBuffer;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
} Vulkan;

    
void createInstance(Vulkan *vulkan);
void createPhysicalDevice(Vulkan *vulkan);
void createDevice(Vulkan *vulkan);
void createSwapChain(Vulkan *vulkan);
void createPipeline(Vulkan *vulkan);
void add_extensions(Extensions *extensions, const char *extension);
