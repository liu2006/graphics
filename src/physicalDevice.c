#include "vulkan.h"

void createPhysicalDevice(Vulkan *vulkan) {
    uint32_t physicalDeviceCount;
    vulkan->result = vkEnumeratePhysicalDevices(vulkan->instance, &physicalDeviceCount, NULL);
    assert(vulkan->result == VK_SUCCESS);

    VkPhysicalDevice *physicalDevices =
        malloc(sizeof(*physicalDevices) * physicalDeviceCount);
    vulkan->result = vkEnumeratePhysicalDevices(vulkan->instance, &physicalDeviceCount,
					physicalDevices);
    assert(vulkan->result == VK_SUCCESS);
    printf("\033[32m-----> Success Found Physical Device\033[0m\n");
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceProperties properties; 
    for (uint32_t i = 0; i < physicalDeviceCount; ++i) {
	vkGetPhysicalDeviceFeatures(physicalDevices[i], &features);
        vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);
        printf("\033[35m- [ %u ] Supported Physical Device: %s\033[0m\n", i + 1,
               properties.deviceName);
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            vulkan->physicalDevice = physicalDevices[i];
            break;
        }
        vulkan->physicalDevice = physicalDevices[i];
    }
    vkGetPhysicalDeviceProperties(vulkan->physicalDevice, &properties);
    printf("\033[34m- Select Physical Device [ %s ]\033[0m\n",
           properties.deviceName);
    VkSampleCountFlags sampleCount =
        properties.limits.framebufferColorSampleCounts &
            properties.limits.framebufferDepthSampleCounts;
    vulkan->msaaSamples = (sampleCount / 2) + (sampleCount % 2);
    printf("\033[34m- Maximum Supported Multiple Sample [ %u ] MSAA\033[0m\n",
           vulkan->msaaSamples);

    
    uint32_t queueFamilyPropertyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(vulkan->physicalDevice,
                                             &queueFamilyPropertyCount, NULL);
    VkQueueFamilyProperties *queueFamilyProperties =
        malloc(sizeof(*queueFamilyProperties) * queueFamilyPropertyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
					     vulkan->physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties);
    bool supportedGraphics;
    for (uint32_t i = 0; i < queueFamilyPropertyCount; ++i) {
	if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
	    supportedGraphics = true;
	    vulkan->graphicsIndex = i;
	    break;
	}
    }
    assert(supportedGraphics);
    printf("\033[32m-----> Queue Family Supported Graphics\033[0m\n");
    printf("\033[35m- Graphics Index: %u\033[0m\n", vulkan->graphicsIndex);

    uint32_t extensionPropertyCount;
    vulkan->result = vkEnumerateDeviceExtensionProperties(
						  vulkan->physicalDevice, NULL, &extensionPropertyCount, NULL);
    VkExtensionProperties *extensionProperties =
        malloc(sizeof(*extensionProperties) * extensionPropertyCount);
    vulkan->result = vkEnumerateDeviceExtensionProperties(
        vulkan->physicalDevice, NULL, &extensionPropertyCount,
        extensionProperties);
    assert(vulkan->result == VK_SUCCESS);
    add_extensions(&vulkan->requiredDeviceExtensions, "VK_KHR_swapchain");

    uint32_t supportedCount = 0;
    for (uint32_t i = 0; i < vulkan->requiredDeviceExtensions.count; ++i) {                       
	for (uint32_t j = 0; j < extensionPropertyCount; ++j) {
            if (strcmp(vulkan->requiredDeviceExtensions.items[i],
                       extensionProperties[j].extensionName) == 0) {
		supportedCount += 1;
		break;
            } else
		continue;
        }
    }
    assert(supportedCount == vulkan->requiredDeviceExtensions.count);
    printf("\033[32m---> Physical Device Supported Swap Chain\033[0m\n");
    free(extensionProperties);
    free(queueFamilyProperties);
    free(physicalDevices);
}
