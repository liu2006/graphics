#include "vulkan.h"

void createUniformBuffers(Vulkan *vulkan) {
    vulkan->uniformBuffers =
	malloc(sizeof(*vulkan->uniformBuffers) * FLIGHT_FRAMES);
    vulkan->uniformBuffersMemory =
        malloc(sizeof(*vulkan->uniformBuffersMemory) * FLIGHT_FRAMES);
    vulkan->uniformBuffersMapped =
        malloc(sizeof(*vulkan->uniformBuffersMapped) * FLIGHT_FRAMES);
    
    for (uint32_t i = 0; i < FLIGHT_FRAMES; i++) {
	VkBufferCreateInfo bufferInfo = {
	    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
	    .pNext = NULL,
	    .flags = 0,
	    .size = sizeof(*vulkan->ubr.items),
	    .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	    .queueFamilyIndexCount = 0,
	    .pQueueFamilyIndices = NULL
	};
	vulkan->result = vkCreateBuffer(vulkan->device, &bufferInfo, NULL,
					&vulkan->uniformBuffers[i]);
	assert(vulkan->result == VK_SUCCESS);

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(vulkan->device, vulkan->uniformBuffers[i],
                                      &memoryRequirements);

	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(vulkan->physicalDevice, &memoryProperties);

	uint32_t memoryTypeIndex;
	for (uint32_t j = 0; j < memoryProperties.memoryTypeCount; j++) {
	    if (!(memoryRequirements.memoryTypeBits & (1 << j)))
		continue;
	    if (!(memoryProperties.memoryTypes[j].propertyFlags &
		  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
		continue;
	    if (!(memoryProperties.memoryTypes[j].propertyFlags &
		  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
		continue;
	    memoryTypeIndex = j;
	    break;
	}
	VkMemoryAllocateInfo allocateInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = NULL,
            .allocationSize = memoryRequirements.size,
            .memoryTypeIndex = memoryTypeIndex};
	vulkan->result = vkAllocateMemory(vulkan->device, &allocateInfo, NULL,
					  &vulkan->uniformBuffersMemory[i]);
	assert(vulkan->result == VK_SUCCESS);
	vulkan->result = vkBindBufferMemory(vulkan->device, vulkan->uniformBuffers[i],
                                            vulkan->uniformBuffersMemory[i], 0);
	assert(vulkan->result == VK_SUCCESS);

	vulkan->result = vkMapMemory(vulkan->device, vulkan->uniformBuffersMemory[i], 0,
                                     memoryRequirements.size, 0, &vulkan->uniformBuffersMapped[i]);
	assert(vulkan->result == VK_SUCCESS);
    }
}
