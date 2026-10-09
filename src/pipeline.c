#include "vulkan.h"

void createPipeline(Vulkan *vulkan) {
    FILE *file = fopen("../shaders/slang.spv", "rb");
    assert(file != NULL); 
    printf("\033[32m-----> Success Opened Shader File\033[0m\n");
    fseek(file, 0, SEEK_END);
    vulkan->spirvSize = ftell(file);
    assert(vulkan->spirvSize);
    printf("\033[35m- Shader File Size: %ld Byte\033[0m\n", vulkan->spirvSize);
    fseek(file, 0, SEEK_SET);
    vulkan->spirvBuffer = malloc((size_t)vulkan->spirvSize);
    assert(vulkan->spirvBuffer);
    fread(vulkan->spirvBuffer, sizeof(uint32_t), vulkan->spirvSize / sizeof(uint32_t), file);
    fclose(file);
    printf("\033[32m-----> Success Loaded Shader File\033[0m\n");

    VkShaderModule shaderModule;
    VkShaderModuleCreateInfo shaderModuleInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .codeSize = (size_t)vulkan->spirvSize,
        .pCode = vulkan->spirvBuffer};
    vulkan->result = vkCreateShaderModule(vulkan->device, &shaderModuleInfo,
                                          NULL, &shaderModule);
    assert(vulkan->result == VK_SUCCESS);

    VkPipelineShaderStageCreateInfo vertexShaderStageInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = shaderModule,
        .pName = "vertexMain",
        .pSpecializationInfo = NULL};

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = shaderModule,
        .pName = "fragMain",
        .pSpecializationInfo = NULL};
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertexShaderStageInfo,
                                                      fragShaderStageInfo};

    VkPipelineViewportStateCreateInfo viewportCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = NULL,
        .scissorCount = 1,
        .pScissors = NULL};
    
    VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT,
                                      VK_DYNAMIC_STATE_SCISSOR};
    
    VkPipelineDynamicStateCreateInfo dynamicStateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]),
        .pDynamicStates = dynamicStates};

    add_vertex(&vulkan->vertices,
               (Vertex){.pos = {0.0f, -0.5f}, .color = {1.0f, 0.0f, 0.0f}});
    add_vertex(&vulkan->vertices,
               (Vertex){.pos = {0.5f, 0.5f}, .color = {0.0f, 1.0f, 0.0f}});
    add_vertex(&vulkan->vertices,
               (Vertex){.pos = {-0.5f, 0.5f}, .color = {0.0f, 0.0f, 1.0f}});
    VkVertexInputBindingDescription bindingDescription = {
	.binding = 0,
	.stride = sizeof(Vertex),
	.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    VkVertexInputAttributeDescription attributeDescription[2] = {
        {.location = 0,
         .binding = 0,
         .format = VK_FORMAT_R32G32_SFLOAT,
         .offset = offsetof(Vertex, pos)},
        {.location = 1,
         .binding = 0,
         .format = VK_FORMAT_R32G32B32_SFLOAT,
         .offset = offsetof(Vertex, color)}};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &bindingDescription,
        .vertexAttributeDescriptionCount = 2,
        .pVertexAttributeDescriptions = attributeDescription};
    
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE};

    VkPipelineRasterizationStateCreateInfo rasterizationInfo = {
        .sType =
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0f};

    VkPipelineMultisampleStateCreateInfo multisampleInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .rasterizationSamples = vulkan->msaaSample,
        .sampleShadingEnable = VK_TRUE,
        .minSampleShading = 1.0f,
        .pSampleMask = NULL,
        .alphaToCoverageEnable = VK_TRUE,
        .alphaToOneEnable = VK_FALSE};

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};

    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment,
        .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}};

    VkPipelineLayout pipelineLayout;
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = 0,
        .pSetLayouts = NULL,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL};

    vulkan->result = vkCreatePipelineLayout(vulkan->device, &pipelineLayoutInfo,
                                            NULL, &pipelineLayout);
    assert(vulkan->result == VK_SUCCESS);

    VkPipelineRenderingCreateInfo renderingInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext = NULL,
        .viewMask = 0,
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &vulkan->surfaceFormats[vulkan->formatIndex].format,
        .depthAttachmentFormat = VK_FORMAT_UNDEFINED,
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED};

    VkGraphicsPipelineCreateInfo graphicsPipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &renderingInfo,
        .flags = 0,
        .stageCount = sizeof(shaderStages) / sizeof(shaderStages[0]),
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssemblyInfo,
        .pTessellationState = NULL,
        .pViewportState = &viewportCreateInfo,
        .pRasterizationState = &rasterizationInfo,
        .pMultisampleState = &multisampleInfo,
        .pDepthStencilState = NULL,
        .pColorBlendState = &colorBlendCreateInfo,
        .pDynamicState = &dynamicStateInfo,
        .layout = pipelineLayout,
        .renderPass = NULL,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    vulkan->result = vkCreateGraphicsPipelines(vulkan->device, VK_NULL_HANDLE,
                                               1, &graphicsPipelineInfo, NULL,
                                               &vulkan->graphicsPipeline);
    assert(vulkan->result == VK_SUCCESS);
    printf("\033[32m-----> Success Created Pipeline\033[0m\n");
}
 
