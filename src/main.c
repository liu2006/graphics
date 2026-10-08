#include "vulkan.h"
#include "vulkan/vulkan_core.h"

int main(void) {
    Vulkan *vulkan = malloc(sizeof(*vulkan));
    createInstance(vulkan);
    createPhysicalDevice(vulkan);
    createDevice(vulkan);
    createSwapChain(vulkan);
    createPipeline(vulkan);

    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
	.pNext = NULL,
	.flags = 0,
	.size = sizeof(*vulkan->vertices.items) * vulkan->vertices.count,
	.usage = VK_BUFFER_USAGE_2_VERTEX_BUFFER_BIT,
	.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	.queueFamilyIndexCount = 0,
	.pQueueFamilyIndices = NULL,
    };
    vulkan->result = vkCreateBuffer(vulkan->device, &bufferInfo, NULL,
                                    &vulkan->vertexBuffer);
    assert(vulkan->result == VK_SUCCESS);
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(vulkan->device, vulkan->vertexBuffer,
                                  &memoryRequirements);
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(vulkan->physicalDevice,
                                        &memoryProperties);
    uint32_t memoryTypeIndex;
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
      if (memoryProperties.memoryTypes[i].propertyFlags &
          (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
          memoryTypeIndex = i;
	  break;
      }
    }
    assert(memoryProperties.memoryTypes[memoryTypeIndex].propertyFlags &
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VkMemoryAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex};

    vulkan->result = vkAllocateMemory(vulkan->device, &allocateInfo, NULL,
                                      &vulkan->vertexBufferMemory);
    assert(vulkan->result == VK_SUCCESS);
    vulkan->result = vkBindBufferMemory(vulkan->device, vulkan->vertexBuffer,
                                        vulkan->vertexBufferMemory, 0);
    assert(vulkan->result == VK_SUCCESS);

    void *data;
    vulkan->result = vkMapMemory(vulkan->device, vulkan->vertexBufferMemory, 0,
                                 memoryRequirements.size, 0, &data);
    memcpy(data, vulkan->vertices.items, bufferInfo.size);
    vkUnmapMemory(vulkan->device, vulkan->vertexBufferMemory);


    VkCommandPoolCreateInfo commandPoolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = vulkan->graphicsIndex};
    vulkan->result = vkCreateCommandPool(vulkan->device, &commandPoolInfo, NULL,
                                         &vulkan->commandPool);
    assert(vulkan->result == VK_SUCCESS);
    vulkan->commandBufferCount = 1;
    vulkan->commandBuffers =
	malloc(sizeof(*vulkan->commandBuffers) * vulkan->commandBufferCount);
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = vulkan->commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 2};
    vulkan->result = vkAllocateCommandBuffers( 
					      vulkan->device, &commandBufferAllocateInfo, vulkan->commandBuffers);
    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0};
    VkFenceCreateInfo fenceInfo = {
	.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = NULL,
	.flags = VK_FENCE_CREATE_SIGNALED_BIT
    };
    vulkan->presentSemaphores =
        malloc(sizeof(*vulkan->presentSemaphores) * vulkan->imageCount);
    vulkan->renderSemaphores =
        malloc(sizeof(*vulkan->renderSemaphores) * vulkan->imageCount);
    vulkan->fences =
        malloc(sizeof(*vulkan->fences) * vulkan->imageCount);

    for (uint32_t i = 0; i < vulkan->imageCount; ++i) {
	vulkan->result = vkCreateSemaphore(vulkan->device, &semaphoreInfo, NULL,
					   &vulkan->presentSemaphores[i]);
	assert(vulkan->result == VK_SUCCESS);
	vulkan->result = vkCreateSemaphore(vulkan->device, &semaphoreInfo, NULL,
					   &vulkan->renderSemaphores[i]);
        assert(vulkan->result == VK_SUCCESS);
        vulkan->result = vkCreateFence(vulkan->device, &fenceInfo, NULL,
                                       &vulkan->fences[i]);
    }
    assert(vulkan->result == VK_SUCCESS);
    
    vulkan->frameIndex = 0;
    while (!glfwWindowShouldClose(vulkan->window)) {
        vulkan->result = vkWaitForFences(vulkan->device, 1, &vulkan->fences[vulkan->frameIndex],
                                         VK_TRUE, UINT64_MAX);
        assert(vulkan->result == VK_SUCCESS);
        vulkan->result = vkResetFences(vulkan->device, 1, &vulkan->fences[vulkan->frameIndex]);
        assert(vulkan->result == VK_SUCCESS);
        vulkan->result =
            vkAcquireNextImageKHR(vulkan->device, vulkan->swapChain, UINT64_MAX,
                                  vulkan->presentSemaphores[vulkan->frameIndex],
                                  VK_NULL_HANDLE, &vulkan->imageIndex);
        assert(vulkan->result == VK_SUCCESS);
        vulkan->result =
            vkResetCommandBuffer(vulkan->commandBuffers[vulkan->frameIndex], 0);
        assert(vulkan->result == VK_SUCCESS);
        recordCommandBuffer(vulkan);

	VkPipelineStageFlagBits waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submitInfo = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                   .pNext = NULL,
                                   .waitSemaphoreCount = 1,
                                   .pWaitSemaphores = &vulkan->presentSemaphores[vulkan->frameIndex],
                                   .pWaitDstStageMask = &waitDstStageMask,
                                   .commandBufferCount = 1,
                                   .pCommandBuffers = &vulkan->commandBuffers[vulkan->frameIndex],
                                   .signalSemaphoreCount = 1,
                                   .pSignalSemaphores =
                                       &vulkan->renderSemaphores[vulkan->imageIndex]};
        vulkan->result =
            vkQueueSubmit(vulkan->queue, 1, &submitInfo, vulkan->fences[vulkan->frameIndex]);
        assert(vulkan->result == VK_SUCCESS);
        
        VkPresentInfoKHR presentInfo = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = NULL,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &vulkan->renderSemaphores[vulkan->imageIndex],
            .swapchainCount = 1,
            .pSwapchains = &vulkan->swapChain,
            .pImageIndices = &vulkan->imageIndex,
            .pResults = NULL,
        };
        vulkan->result = vkQueuePresentKHR(vulkan->queue, &presentInfo);
        assert(vulkan->result == VK_SUCCESS);
        vulkan->frameIndex = (vulkan->frameIndex + 1) % vulkan->imageCount;
        glfwPollEvents();
    }
    printf("\033[36m-----> Program Exited Success\033[0m\n");
    return 0;
}
