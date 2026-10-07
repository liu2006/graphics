#include "vulkan.h"
#include "vulkan/vulkan_core.h"

void createSwapChain(Vulkan *vulkan) {
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vulkan->result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
							       vulkan->physicalDevice, vulkan->surface, &surfaceCapabilities);
    assert(vulkan->result == VK_SUCCESS);
    uint32_t surfaceFormatCount;
    vulkan->result = vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan->physicalDevice, vulkan->surface,
							  &surfaceFormatCount, NULL);
    assert(vulkan->result == VK_SUCCESS);
    vulkan->surfaceFormats =
        malloc(sizeof(*vulkan->surfaceFormats) * surfaceFormatCount);
    vulkan->result = vkGetPhysicalDeviceSurfaceFormatsKHR(
							  vulkan->physicalDevice, vulkan->surface, &surfaceFormatCount, vulkan->surfaceFormats);
    assert(vulkan->result == VK_SUCCESS);

    vulkan->formatIndex = 0;
    for (uint32_t i = 0; i < surfaceFormatCount; ++i) {
      if (vulkan->surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
          vulkan->surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                vulkan->formatIndex = i;
                break;
      } else if (vulkan->surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB) {
          vulkan->formatIndex = i;
      }
    }
    uint32_t presentModeCount;
    vulkan->result = vkGetPhysicalDeviceSurfacePresentModesKHR(
							       vulkan->physicalDevice, vulkan->surface, &presentModeCount, NULL);
    assert(vulkan->result == VK_SUCCESS);
    VkPresentModeKHR *presentModes =
        malloc(sizeof(*presentModes) * presentModeCount);
    
    vulkan->result = vkGetPhysicalDeviceSurfacePresentModesKHR(
        vulkan->physicalDevice, vulkan->surface, &presentModeCount,
        presentModes);
    assert(vulkan->result == VK_SUCCESS);

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    bool supportedMailbox;
    for (uint32_t i = 0; i < presentModeCount; ++i) {
        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            supportedMailbox = true;
            printf("\033[34m- Usage [ mailbox ] Present Mode\033[0m\n");
            break;
        }
    }
    if (!supportedMailbox)
	printf("\033[33m- Usage [ fifo ] Present Mode\033[0m\n");
    int width, height;
    glfwGetFramebufferSize(vulkan->window, &width, &height);
    vulkan->extent.width = width;
    vulkan->extent.height = height;
    if (surfaceCapabilities.minImageCount >= 2)
	vulkan->imageCount = 2; else
	vulkan->imageCount = surfaceCapabilities.minImageCount;
    printf("\033[35m- Swap Chain Image Count: %u\033[0m\n", vulkan->imageCount);

    VkSwapchainCreateInfoKHR swapChainInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .surface = vulkan->surface,
        .minImageCount = vulkan->imageCount,
        .imageFormat = vulkan->surfaceFormats[vulkan->formatIndex].format,
        .imageColorSpace = vulkan->surfaceFormats[vulkan->formatIndex].colorSpace,
        .imageExtent = vulkan->extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &vulkan->graphicsIndex,
        .preTransform = surfaceCapabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = true,
        .oldSwapchain = VK_NULL_HANDLE};

    vulkan->result = vkCreateSwapchainKHR(vulkan->device, &swapChainInfo, NULL, &vulkan->swapChain);
    assert(vulkan->result == VK_SUCCESS);
    printf("\033[32m-----> Success Create Swap Chain\033[0m\n");

    vulkan->result = vkGetSwapchainImagesKHR(vulkan->device, vulkan->swapChain,
                                             &vulkan->imageCount, NULL);
    assert(vulkan->result == VK_SUCCESS);
    vulkan->swapChainImages = malloc(sizeof(*vulkan->swapChainImages) * vulkan->imageCount);
    vulkan->result = vkGetSwapchainImagesKHR(vulkan->device, vulkan->swapChain, &vulkan->imageCount, vulkan->swapChainImages);
    assert(vulkan->result == VK_SUCCESS);
    
    vulkan->imageViews = malloc(sizeof(*vulkan->imageViews) * vulkan->imageCount);
    VkImageViewCreateInfo imageViewCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = vulkan->surfaceFormats[vulkan->formatIndex].format,
        .components = {.r = VK_COMPONENT_SWIZZLE_R,
                       .g = VK_COMPONENT_SWIZZLE_G,
                       .b = VK_COMPONENT_SWIZZLE_B,
                       .a = VK_COMPONENT_SWIZZLE_A},
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                             .baseMipLevel = 0,
                             .levelCount = 1,
                             .baseArrayLayer = 0,
                             .layerCount = 1}};

    for (uint32_t i = 0; i < vulkan->imageCount; ++i) {
	imageViewCreateInfo.image = vulkan->swapChainImages[i];
	vulkan->result = vkCreateImageView(vulkan->device, &imageViewCreateInfo,
					   NULL, &vulkan->imageViews[i]);
	assert(vulkan->result == VK_SUCCESS);
    }

    VkImageCreateInfo msaaImageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = vulkan->surfaceFormats[vulkan->formatIndex].format,
        .extent = {.width = vulkan->extent.width,
                   .height = vulkan->extent.height,
                   .depth = 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_8_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED};

    vulkan->result =
        vkCreateImage(vulkan->device, &msaaImageInfo, NULL, &vulkan->msaaImage);
    assert(vulkan->result == VK_SUCCESS);
    
    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(vulkan->device, vulkan->msaaImage,
                                 &memoryRequirements);
    
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(vulkan->physicalDevice,
                                        &memoryProperties);
    uint32_t memoryTypeIndex;
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
      if (memoryProperties.memoryTypes[i].propertyFlags &
          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
          memoryTypeIndex = i;
          break;
      } 
    }
    assert(memoryProperties.memoryTypes[memoryTypeIndex].propertyFlags &
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex,
    };
    vulkan->result = vkAllocateMemory(vulkan->device, &allocateInfo,
                                      NULL, &vulkan->msaaImageMemory);
    assert(vulkan->result == VK_SUCCESS);
    
    vulkan->result = vkBindImageMemory(vulkan->device, vulkan->msaaImage,
                                       vulkan->msaaImageMemory, 0);
    assert(vulkan->result == VK_SUCCESS);
    VkImageViewCreateInfo msaaImageViewInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .image = vulkan->msaaImage,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = vulkan->surfaceFormats[vulkan->formatIndex].format,
        .components = {.r = VK_COMPONENT_SWIZZLE_R,
                       .g = VK_COMPONENT_SWIZZLE_G,
                       .b = VK_COMPONENT_SWIZZLE_B,
                       .a = VK_COMPONENT_SWIZZLE_A},
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                             .baseMipLevel = 0,
                             .levelCount = 1,
                             .baseArrayLayer = 0,
                             .layerCount = 1}};
    
    vulkan->result = vkCreateImageView(vulkan->device, &msaaImageViewInfo, NULL,
                                       &vulkan->msaaImageView);
    assert(vulkan->result == VK_SUCCESS);
    free(presentModes);
}
