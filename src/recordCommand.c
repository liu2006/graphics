#include "vulkan.h"

void recordCommandBuffer(Vulkan *vulkan) {
    VkCommandBufferBeginInfo commandBufferBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = 0,
        .pInheritanceInfo = NULL,
    };
    vulkan->result = vkBeginCommandBuffer(
					  vulkan->commandBuffers[vulkan->frameIndex], &commandBufferBeginInfo);
    assert(vulkan->result == VK_SUCCESS);

    VkImageMemoryBarrier2 barrierStart[2] = {
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .pNext = NULL,
            .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = vulkan->msaaImage,
            .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                 .baseMipLevel = 0,
                                 .levelCount = 1,
                                 .baseArrayLayer = 0,
                                 .layerCount = 1},
        },
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .pNext = NULL,
            .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_2_NONE,
            .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = vulkan->swapChainImages[vulkan->imageIndex],
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        },
    };
    VkDependencyInfo dependencyInfo = {.sType =
                                           VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                                       .pNext = NULL,
                                       .dependencyFlags = 0,
                                       .memoryBarrierCount = 0,
                                       .pMemoryBarriers = NULL,
                                       .bufferMemoryBarrierCount = 0,
                                       .pBufferMemoryBarriers = NULL,
                                       .imageMemoryBarrierCount = 2,
                                       .pImageMemoryBarriers = barrierStart};
    
    vkCmdPipelineBarrier2(vulkan->commandBuffers[vulkan->frameIndex], &dependencyInfo);

    VkRenderingAttachmentInfo renderingAttachmentInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext = NULL,
        .imageView = vulkan->msaaImageView,
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT,
        .resolveImageView = vulkan->imageViews[vulkan->imageIndex],
        .resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = {
	    .color = {0.1f, 0.1f, 0.1f, 1.0f},
        }};

    VkRenderingInfo renderingInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .pNext = NULL,
        .flags = 0,
        .renderArea = {.offset = {.x = 0, .y = 0}, .extent = vulkan->swapChainextent},
        .layerCount = 1,
        .viewMask = 0,
        .colorAttachmentCount = 1,
        .pColorAttachments = &renderingAttachmentInfo,
        .pDepthAttachment = NULL,
        .pStencilAttachment = NULL};

    vkCmdBeginRendering(vulkan->commandBuffers[vulkan->frameIndex], &renderingInfo);

    vkCmdBindPipeline(vulkan->commandBuffers[vulkan->frameIndex],
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      vulkan->graphicsPipeline);

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(vulkan->commandBuffers[vulkan->frameIndex], 0, 1,
                           &vulkan->vertexBuffer, &offset);
    vkCmdBindIndexBuffer(vulkan->commandBuffers[vulkan->frameIndex],
                         vulkan->indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vulkan->viewport = (VkViewport){.x = 0.0f,
                           .y = 0.0f,
                           .width = vulkan->swapChainextent.width,
                           .height = vulkan->swapChainextent.height,
                           .minDepth = 0.0f,
                           .maxDepth = 1.0f};
    vulkan->scissor =
        (VkRect2D){.offset = {.x = 0, .y = 0}, .extent = vulkan->swapChainextent};
    
    vkCmdSetViewport(vulkan->commandBuffers[vulkan->frameIndex], 0, 1, &vulkan->viewport);
    vkCmdSetScissor(vulkan->commandBuffers[vulkan->frameIndex], 0, 1,
                    &vulkan->scissor);

    vkCmdDrawIndexed(vulkan->commandBuffers[vulkan->frameIndex],
                     vulkan->indices.count, 1, 0, 0, 0);
    
    vkCmdEndRendering(vulkan->commandBuffers[vulkan->frameIndex]);

    VkImageMemoryBarrier2 barrierEnd = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext = NULL,
        .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
        .dstAccessMask =  VK_ACCESS_2_NONE,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = vulkan->swapChainImages[vulkan->imageIndex],
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			     .baseMipLevel = 0,
			     .levelCount = 1,
			     .baseArrayLayer = 0,
			     .layerCount = 1},
    };
    VkDependencyInfo dependencyInfo2 = {.sType =
        VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                                        .pNext = NULL,
                                        .dependencyFlags = 0,
                                        .memoryBarrierCount = 0,
                                        .pMemoryBarriers = NULL,
                                        .bufferMemoryBarrierCount = 0,
                                        .pBufferMemoryBarriers = NULL,
                                        .imageMemoryBarrierCount = 1,
                                        .pImageMemoryBarriers = &barrierEnd};
    
    vkCmdPipelineBarrier2(vulkan->commandBuffers[vulkan->frameIndex], &dependencyInfo2);
    vulkan->result = vkEndCommandBuffer(vulkan->commandBuffers[vulkan->frameIndex]);
    assert(vulkan->result == VK_SUCCESS);
}
