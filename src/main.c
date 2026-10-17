#include "vulkan.h"
#include "vulkan/vulkan_core.h"

int main(void) {
    Vulkan *vulkan = calloc(1, sizeof(*vulkan));
    createInstance(vulkan);
    createPhysicalDevice(vulkan);
    createDevice(vulkan);
    createSwapChain(vulkan);
    createPipeline(vulkan);
    createCommandBuffer(vulkan);
    createVertexBuffer(vulkan);
    createIndexBuffer(vulkan);
    createUniformBuffers(vulkan);

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
            vkQueueSubmit(vulkan->graphicsQueue, 1, &submitInfo, vulkan->fences[vulkan->frameIndex]);
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
        vulkan->result = vkQueuePresentKHR(vulkan->graphicsQueue, &presentInfo);
        assert(vulkan->result == VK_SUCCESS);
        vulkan->frameIndex = (vulkan->frameIndex + 1) % FLIGHT_FRAMES;
        glfwPollEvents();
    }
    printf("\033[36m-----> Program Exited Success\033[0m\n");
    return 0;
}
