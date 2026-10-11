#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <assert.h>
#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

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
    Vertices vertices; 
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
    uint32_t flight_frames;
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
void append_extensions(Extensions *extensions, const char *extension);
void append_vertex(Vertices *vertices, Vertex *vertex);
void append_index(Indices *indices, Index *index);

