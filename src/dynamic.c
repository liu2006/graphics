#include "vulkan.h"

void auto_stack(Stack *stack, CleanupFunc func, void *arg) {
    if (stack->count >= stack->capacity) {
	stack->capacity = stack->capacity ? stack->capacity * 2 : 1;
	stack->items = realloc(stack->items, stack->capacity * sizeof(*stack->items));
    }
    stack->items[stack->count++] = (Cleanup){.func = func, .arg = arg};
}


void add_extensions(Extensions *extensions, const char *extension) {
    if (extensions->count >= extensions->capacity) {
      extensions->capacity =
          extensions->capacity ? extensions->capacity * 2 : 1;
      extensions->items = realloc(extensions->items, extensions->capacity * sizeof(*extensions->items));
    }
    extensions->items[extensions->count++] = extension;
}

void terminate(Vulkan *vulkan) {
    glfwDestroyWindow(vulkan->window);
    glfwTerminate();
    free(vulkan);
}

void cleanup_stack(Vulkan *vulkan) {
    while (vulkan->stack.count > 0) {
	Cleanup task = vulkan->stack.items[--vulkan->stack.count];
	task.func(task.arg);
    }
    free(vulkan->stack.items);
    vkDestroyPipeline(vulkan->device, vulkan->graphicsPipeline, NULL);
    vkDestroyPipelineLayout(vulkan->device, vulkan->pipelineLayout, NULL);    
    vkDestroyShaderModule(vulkan->device, vulkan->shaderModule, NULL);
    vkDestroySwapchainKHR(vulkan->device, vulkan->swapChain, NULL);
    vkDestroyDevice(vulkan->device, NULL);
    vkDestroySurfaceKHR(vulkan->instance, vulkan->surface, NULL);
    vkDestroyInstance(vulkan->instance, NULL);
}

