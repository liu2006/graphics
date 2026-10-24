#include "vulkan.h"

void createPhysicalDevice(Vulkan *vulkan)
{
    uint32_t physicalDeviceCount;
    vulkan->result = vkEnumeratePhysicalDevices(vulkan->instance, &physicalDeviceCount, NULL);
    assert(vulkan->result == VK_SUCCESS);

    VkPhysicalDevice *physicalDevices = malloc(sizeof(*physicalDevices) * physicalDeviceCount);
    vulkan->result = vkEnumeratePhysicalDevices(vulkan->instance, &physicalDeviceCount, physicalDevices);
    assert(vulkan->result == VK_SUCCESS);
    printf("\033[32m-----> Success Found Physical Device\033[0m\n");
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceProperties properties;
    for (size_t i = 0; i < physicalDeviceCount; i++) {
        vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);
        printf("\033[35m- [ %u ] Supported Physical Device: %s\033[0m\n", (uint32_t)i + 1, properties.deviceName);
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            vulkan->physicalDevice = physicalDevices[i];
            break;
        }
        vulkan->physicalDevice = physicalDevices[i];
    }
    vkGetPhysicalDeviceProperties(vulkan->physicalDevice, &properties);
    vkGetPhysicalDeviceFeatures(vulkan->physicalDevice, &features);
    printf("\033[34m- Selected Physical Device [ %s ]\033[0m\n", properties.deviceName);

    vulkan->msaaSample = DETERMINE_MSAA_SAMPLE(properties);
    printf("\033[34m- Selected MSAA Sample [ %u x MSAA ]\033[0m\n", vulkan->msaaSample);

    uint32_t queueFamilyPropertyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(vulkan->physicalDevice, &queueFamilyPropertyCount, NULL);
    VkQueueFamilyProperties *queueFamilyProperties = malloc(sizeof(*queueFamilyProperties) * queueFamilyPropertyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vulkan->physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties);
    bool supportedGraphics = false;
    for (size_t i = 0; i < queueFamilyPropertyCount; i++) {
        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            supportedGraphics = true;
            vulkan->graphicsIndex = i;
            break;
        }
    }
    assert(supportedGraphics);
    printf("\033[32m-----> Supported Graphics Queue Family\033[0m\n");

    uint32_t extensionPropertyCount;
    vulkan->result = vkEnumerateDeviceExtensionProperties(vulkan->physicalDevice, NULL, &extensionPropertyCount, NULL);
    VkExtensionProperties *extensionProperties = malloc(sizeof(*extensionProperties) * extensionPropertyCount);
    vulkan->result = vkEnumerateDeviceExtensionProperties(vulkan->physicalDevice, NULL, &extensionPropertyCount, extensionProperties);
    assert(vulkan->result == VK_SUCCESS);
    APPEND_VECTOR(vulkan->requiredDeviceExtensions, "VK_KHR_swapchain");

    uint32_t supportedCount = 0;
    CHECK_EXTENSIONS(vulkan->requiredDeviceExtensions.count, vulkan->requiredDeviceExtensions.items, extensionPropertyCount,
                     extensionProperties, supportedCount);
    assert(supportedCount == vulkan->requiredDeviceExtensions.count);
    printf("\033[32m-----> Physical Device Supported Swap Chain\033[0m\n");
    free(extensionProperties);
    free(queueFamilyProperties);
    free(physicalDevices);
}
