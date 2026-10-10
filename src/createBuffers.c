#include "vulkan.h"
#include "vulkan/vulkan_core.h"

void createBuffers(Vulkan *vulkan) {
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VkBufferCreateInfo stagingBufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
	.pNext = NULL,
	.flags = 0,
	.size = sizeof(*vulkan->vertices.items) * vulkan->vertices.count,
	.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	.queueFamilyIndexCount = 0,
	.pQueueFamilyIndices = NULL,
    };
    vulkan->result = vkCreateBuffer(vulkan->device, &stagingBufferInfo, NULL,
                                    &stagingBuffer);
    assert(vulkan->result == VK_SUCCESS);
    
    VkMemoryRequirements stagingMemoryRequirements;
    vkGetBufferMemoryRequirements(vulkan->device, stagingBuffer,
                                  &stagingMemoryRequirements);
    VkPhysicalDeviceMemoryProperties stagingMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(vulkan->physicalDevice,
                                        &stagingMemoryProperties);
    uint32_t stagingMemoryTypeIndex;
    for (uint32_t i = 0; i < stagingMemoryProperties.memoryTypeCount; i++) {
	if (!(stagingMemoryRequirements.memoryTypeBits & (1 << i))) continue;
        if (!(stagingMemoryProperties.memoryTypes[i].propertyFlags &
              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
            continue;
        if (!(stagingMemoryProperties.memoryTypes[i].propertyFlags &
              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
            continue;
        stagingMemoryTypeIndex = i;
	break;
    }

    VkMemoryAllocateInfo stagingAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = stagingMemoryRequirements.size,
        .memoryTypeIndex = stagingMemoryTypeIndex};

    vulkan->result = vkAllocateMemory(vulkan->device, &stagingAllocateInfo, NULL,
                                      &stagingBufferMemory);
    assert(vulkan->result == VK_SUCCESS);
    vulkan->result = vkBindBufferMemory(vulkan->device, stagingBuffer,
                                        stagingBufferMemory, 0);
    assert(vulkan->result == VK_SUCCESS);

    void *dataStaging;
    vulkan->result =
        vkMapMemory(vulkan->device, stagingBufferMemory, 0,
                    stagingMemoryRequirements.size, 0, &dataStaging);
    assert(vulkan->result == VK_SUCCESS);
    
    memcpy(dataStaging, vulkan->vertices.items, stagingBufferInfo.size);
    vkUnmapMemory(vulkan->device, stagingBufferMemory);


    VkBufferCreateInfo vertexBufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .size = sizeof(*vulkan->vertices.items) * vulkan->vertices.count,
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL,
    };
    vulkan->result = vkCreateBuffer(vulkan->device, &vertexBufferInfo, NULL,
                                    &vulkan->vertexBuffer);
    assert(vulkan->result == VK_SUCCESS);
    
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(vulkan->device, vulkan->vertexBuffer,
                                  &memoryRequirements);
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(vulkan->physicalDevice,
                                        &memoryProperties);
    uint32_t memoryTypeIndex;
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
	if (!(memoryRequirements.memoryTypeBits & (1 << i))) continue;
        if (!(memoryProperties.memoryTypes[i].propertyFlags &
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
            continue;
        memoryTypeIndex = i;
	break;
    }
    assert(memoryProperties.memoryTypes[memoryTypeIndex].propertyFlags &
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex};

    vulkan->result = vkAllocateMemory(vulkan->device, &memoryAllocateInfo, NULL,
                                      &vulkan->vertexBufferMemory);
    assert(vulkan->result == VK_SUCCESS);
    vulkan->result = vkBindBufferMemory(vulkan->device, vulkan->vertexBuffer,
                                        vulkan->vertexBufferMemory, 0);
    assert(vulkan->result == VK_SUCCESS);

    VkCommandBuffer commandCopyBuffer;
    VkCommandBufferAllocateInfo commandBufferallocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = vulkan->commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1};

    vulkan->result = vkAllocateCommandBuffers(
					      vulkan->device, &commandBufferallocateInfo, &commandCopyBuffer);
    assert(vulkan->result == VK_SUCCESS);
     VkCommandBufferBeginInfo beginInfo = {
	 .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
         .pNext = NULL,
	 .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	 .pInheritanceInfo = NULL,
    };
    vulkan->result = vkBeginCommandBuffer(commandCopyBuffer, &beginInfo);

    VkBufferCopy bufferCopy = {
        .srcOffset = 0, .dstOffset = 0, .size = stagingBufferInfo.size};
    vkCmdCopyBuffer(commandCopyBuffer, stagingBuffer, vulkan->vertexBuffer, 1,
                    &bufferCopy);
    vulkan->result = vkEndCommandBuffer(commandCopyBuffer);

    VkFence fence;
    VkFenceCreateInfo fenceInfo = {.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                   .pNext = NULL,
                                   .flags = 0};
    vulkan->result = vkCreateFence(vulkan->device, &fenceInfo, NULL,
                                   &fence);
    assert(vulkan->result == VK_SUCCESS);

    VkSubmitInfo submitInfo = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                               .pNext = NULL,
                               .waitSemaphoreCount = 0,
                               .pWaitSemaphores = NULL,
                               .pWaitDstStageMask = NULL,
                               .commandBufferCount = 1,
                               .pCommandBuffers = &commandCopyBuffer,
                               .signalSemaphoreCount = 0,
                               .pSignalSemaphores = NULL};
    vulkan->result = vkQueueSubmit(vulkan->graphicsQueue, 1, &submitInfo, fence);
    assert(vulkan->result == VK_SUCCESS);
    vulkan->result = vkWaitForFences(vulkan->device, 1, &fence, VK_TRUE, UINT64_MAX);
    assert(vulkan->result == VK_SUCCESS);
}
