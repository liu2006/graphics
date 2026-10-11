cmake_minimum_required(VERSION 4.4.3)
project(render)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(GLFW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw/")
set(VULKAN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/vulkan/x86_64/")

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
  src/main.c
)

add_library(myLib SHARED)
target_sources(myLib
  PRIVATE
  src/dynamic.c
  src/instance.c
  src/physicalDevice.c
  src/device.c
  src/swapChain.c
  src/pipeline.c
  src/commandBuffer.c
  src/recordCommand.c
  src/vertexBuffer.c
  src/indexBuffer.c

  PUBLIC
  FILE_SET HEADERS
  BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
  FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/vulkan.h"
)

include(FetchContent)
FetchContent_Declare(glfw SOURCE_DIR "${GLFW_DIR}")
FetchContent_MakeAvailable(glfw)

add_library(vulkanSDK INTERFACE)
target_link_directories(vulkanSDK INTERFACE "${VULKAN_DIR}/lib/")
target_include_directories(vulkanSDK INTERFACE "${VULKAN_DIR}/include/")

target_link_libraries(myLib PUBLIC glfw vulkanSDK vulkan)
target_link_libraries(${PROJECT_NAME} PRIVATE myLib)
