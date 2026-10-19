#include "vulkan/vulkan_core.h"
#include <vulkan.h>


void createDescriptorSetLayout(Vulkan *vulkan)
{
    VkDescriptorSetLayoutBinding uboLayoutBinding = {.binding = 1,
                                                     .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                     .descriptorCount = 1,
                                                     .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                                                     .pImmutableSamplers = NULL};
    VkDescriptorSetLayoutCreateInfo layoutInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                                                  .pNext = NULL,
                                                  .flags = 0,
                                                  .bindingCount = 0,
                                                  .pBindings = &uboLayoutBinding};
    vkCreateDescriptorSetLayout(vulkan->device, &layoutInfo, NULL, &vulkan->descriptorLayout);
    
}
