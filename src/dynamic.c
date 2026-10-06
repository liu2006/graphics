#include "vulkan.h"


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
