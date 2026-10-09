#include "vulkan.h"

void createDevice(Vulkan *vulkan) {
    uint32_t queueCount;
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = NULL,
        .queueFamilyIndex = vulkan->graphicsIndex,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority};

    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT dynamicState = {
        .sType =
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT,
        .pNext = NULL,
	.extendedDynamicState = true,
    };

    VkPhysicalDeviceVulkan13Features vulkan13Features = {
	.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
	.pNext = &dynamicState,
	.dynamicRendering = true};

    VkPhysicalDeviceVulkan11Features vulkan11Features = {
	.shaderDrawParameters = true,
	.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
	.pNext = &vulkan13Features};

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(vulkan->physicalDevice, &features);
    VkPhysicalDeviceFeatures2 deviceFeatures2 = {
	.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
	.pNext = &vulkan11Features,
	.features = features};

    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &deviceFeatures2,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueCreateInfo,
        .enabledExtensionCount = vulkan->requiredDeviceExtensions.count,
        .ppEnabledExtensionNames = vulkan->requiredDeviceExtensions.items,
        .pEnabledFeatures = NULL};

    vulkan->result = vkCreateDevice(vulkan->physicalDevice, &deviceCreateInfo, NULL, &vulkan->device);
    assert(vulkan->result == VK_SUCCESS);
    printf("\033[32m-----> Success Created Logic Device\033[0m\n");

    vkGetDeviceQueue(vulkan->device, vulkan->graphicsIndex, 0, &vulkan->graphicsQueue);
    printf("\033[32m-----> Success Created Graphics Queue\033[0m\n");

}
