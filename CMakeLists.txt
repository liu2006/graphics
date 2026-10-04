cmake_minimum_required(VERSION 4.4.3)
project(render)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE main.c)

set(GLFW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw/")
set(VULKAN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/vulkan/x86_64/")

include(FetchContent)
FetchContent_Declare(glfw SOURCE_DIR "${GLFW_DIR}")
FetchContent_MakeAvailable(glfw)

add_library(vulkanSDK SHARED IMPORTED)
target_link_directories(vulkanSDK INTERFACE "${VULKAN_DIR}/lib/")
target_include_directories(vulkanSDK INTERFACE "${VULKAN_DIR}/include/")

set_target_properties(vulkanSDK PROPERTIES
  IMPORTED_LOCATION "${VULKAN_DIR}/lib/VulkanLoader/lib/libvulkan.so"
)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw vulkanSDK)
