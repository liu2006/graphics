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
    VkImage *swapChainImages;
    uint32_t imageIndex;
    VkImage msaaImage;
    VkDeviceMemory msaaImageMemory;
    VkImageView msaaImageView;
    VkImageView *imageViews;
    VkSampleCountFlagBits msaaSample;
    VkSwapchainKHR swapChain;
    VkShaderModule shaderModule;
    long int spirvSize;
    uint32_t *spirvBuffer;
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
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
void recordCommandBuffer(Vulkan *vulkan);
void add_extensions(Extensions *extensions, const char *extension);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
