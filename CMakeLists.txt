cmake_minimum_required(VERSION 3.23)
project(render LANGUAGES C)

include(GNUInstallDirs)
include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(GLFW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw")
set(VULKAN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/vulkan/x86_64")

add_library(renderCore SHARED
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
  src/uniformBuffers.c
)

target_include_directories(renderCore
  PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

target_compile_options(renderCore PRIVATE
  -Wall
  -Wextra
)

FetchContent_Declare(glfw SOURCE_DIR "${GLFW_DIR}")
FetchContent_MakeAvailable(glfw)

add_library(vulkanSDK INTERFACE)
target_link_directories(vulkanSDK INTERFACE "${VULKAN_DIR}/lib")
target_include_directories(vulkanSDK INTERFACE "${VULKAN_DIR}/include")
target_link_libraries(vulkanSDK INTERFACE vulkan)

target_link_libraries(renderCore PUBLIC glfw vulkanSDK m)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
  src/main.c
)
target_link_libraries(${PROJECT_NAME} PRIVATE renderCore)

install(TARGETS renderCore
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

install(TARGETS render
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

set_target_properties(render PROPERTIES
  INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/shaders/"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/shaders"
  FILES_MATCHING PATTERN "*.spv"
)
