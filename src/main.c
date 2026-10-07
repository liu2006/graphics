#include "vulkan.h"
#include "vulkan/vulkan_core.h"

int main(void) {
    Vulkan *vulkan = malloc(sizeof(*vulkan));
    createInstance(vulkan);
    createPhysicalDevice(vulkan);
    createDevice(vulkan);
    createSwapChain(vulkan);
    createPipeline(vulkan);

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
        .commandBufferCount = 1};

    vulkan->result = vkAllocateCommandBuffers(
					      vulkan->device, &commandBufferAllocateInfo, vulkan->commandBuffers);

    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0};
    vulkan->result = vkCreateSemaphore(vulkan->device, &semaphoreInfo, NULL,
                                       &vulkan->presentSemaphore);
    assert(vulkan->result == VK_SUCCESS);
    vulkan->result = vkCreateSemaphore(vulkan->device, &semaphoreInfo, NULL,
                                       &vulkan->renderSemaphore);
    assert(vulkan->result == VK_SUCCESS);
    
    VkFenceCreateInfo fenceInfo = {
	.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = NULL,
	.flags = VK_FENCE_CREATE_SIGNALED_BIT
    };
    vulkan->result = vkCreateFence(vulkan->device, &fenceInfo, NULL,
                                   &vulkan->fence);
    assert(vulkan->result == VK_SUCCESS);
    
    while (!glfwWindowShouldClose(vulkan->window)) {

        vulkan->result = vkWaitForFences(vulkan->device, 1, &vulkan->fence,
                                         VK_TRUE, UINT64_MAX);
        assert(vulkan->result == VK_SUCCESS);
        vulkan->result = vkResetFences(vulkan->device, 1, &vulkan->fence);
        assert(vulkan->result == VK_SUCCESS);

        vulkan->result = vkAcquireNextImageKHR(
            vulkan->device, vulkan->swapChain, UINT64_MAX,
            vulkan->presentSemaphore, VK_NULL_HANDLE, &vulkan->imageIndex);
        
        recordCommandBuffer(vulkan);
        assert(vulkan->result == VK_SUCCESS);

	VkPipelineStageFlagBits waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submitInfo = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                   .pNext = NULL,
                                   .waitSemaphoreCount = 1,
                                   .pWaitSemaphores = &vulkan->presentSemaphore,
                                   .pWaitDstStageMask = &waitDstStageMask,
                                   .commandBufferCount = 1,
                                   .pCommandBuffers = vulkan->commandBuffers,
                                   .signalSemaphoreCount = 1,
                                   .pSignalSemaphores =
                                       &vulkan->renderSemaphore};
        vulkan->result =
            vkQueueSubmit(vulkan->queue, 1, &submitInfo, vulkan->fence);
        assert(vulkan->result == VK_SUCCESS);
        
        VkPresentInfoKHR presentInfo = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = NULL,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &vulkan->renderSemaphore,
            .swapchainCount = 1,
            .pSwapchains = &vulkan->swapChain,
            .pImageIndices = &vulkan->imageIndex,
            .pResults = NULL,
        };
        vulkan->result = vkQueuePresentKHR(vulkan->queue, &presentInfo);
        assert(vulkan->result == VK_SUCCESS);
        glfwPollEvents();
    }
    return 0;
}
