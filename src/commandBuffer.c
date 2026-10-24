#include "vulkan.h"
#include "vulkan/vulkan_core.h"

void createCommandBuffer(Vulkan *vulkan)
{
    VkCommandPoolCreateInfo commandPoolInfo = {.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                                               .pNext = NULL,
                                               .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                                               .queueFamilyIndex = vulkan->graphicsIndex};
    vulkan->result = vkCreateCommandPool(vulkan->device, &commandPoolInfo, NULL, &vulkan->commandPool);
    assert(vulkan->result == VK_SUCCESS);
    vulkan->commandBufferCount = 1;
    vulkan->commandBuffers = malloc(sizeof(*vulkan->commandBuffers) * vulkan->commandBufferCount);
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                                             .pNext = NULL,
                                                             .commandPool = vulkan->commandPool,
                                                             .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                             .commandBufferCount = 2};
    vulkan->result = vkAllocateCommandBuffers(vulkan->device, &commandBufferAllocateInfo, vulkan->commandBuffers);
    VkSemaphoreCreateInfo semaphoreInfo = {.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, .pNext = NULL, .flags = 0};
    VkFenceCreateInfo fenceInfo = {.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .pNext = NULL, .flags = VK_FENCE_CREATE_SIGNALED_BIT};
    vulkan->frameIndex = 0;

    vulkan->presentSemaphores = malloc(sizeof(*vulkan->presentSemaphores) * FLIGHT_FRAMES);
    vulkan->renderSemaphores = malloc(sizeof(*vulkan->renderSemaphores) * vulkan->imageCount);
    vulkan->fences = malloc(sizeof(*vulkan->fences) * FLIGHT_FRAMES);

    for (uint32_t i = 0; i < vulkan->imageCount; i++) {
        vulkan->result = vkCreateSemaphore(vulkan->device, &semaphoreInfo, NULL, &vulkan->renderSemaphores[i]);
        assert(vulkan->result == VK_SUCCESS);
    }
    for (uint32_t i = 0; i < FLIGHT_FRAMES; i++) {
        vulkan->result = vkCreateFence(vulkan->device, &fenceInfo, NULL, &vulkan->fences[i]);
        assert(vulkan->result == VK_SUCCESS);
        vulkan->result = vkCreateSemaphore(vulkan->device, &semaphoreInfo, NULL, &vulkan->presentSemaphores[i]);
        assert(vulkan->result == VK_SUCCESS);
    }
}
