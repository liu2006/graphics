#include "vulkan.h"

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if ((key == GLFW_KEY_ESCAPE ||
	 (key == GLFW_KEY_C && mods == GLFW_MOD_CONTROL)) &&
	action == GLFW_PRESS) {
	glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void add_extensions(Extensions *extensions, const char *extension) {
    if (extensions->count >= extensions->capacity) {
	extensions->capacity =
            extensions->capacity ? extensions->capacity * 2 : 1;
	extensions->items = realloc(extensions->items, extensions->capacity * sizeof(*extensions->items));
    }
    extensions->items[extensions->count++] = extension;
}

void add_vertex(Vertices *vertices, Vertex vertex) {
    if (vertices->count >= vertices->capacity)
	vertices->capacity = vertices->capacity ? vertices->capacity * 2 : 1;
    vertices->items = realloc(vertices->items, vertices->capacity * sizeof(*vertices->items));
    vertices->items[vertices->count++] = vertex;
}
