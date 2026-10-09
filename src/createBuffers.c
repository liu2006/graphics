#include "vulkan.h"

void createBuffers(Vulkan *vulkan) {
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
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
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
}
