cmake_minimum_required(VERSION 3.20)
project(Voxelinity)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)


# Recover all src files
file(GLOB_RECURSE SRC
        "src/*.h"
        "src/*.cpp"
)

# Because ImGUI doesn't have a Cmake file...
file(GLOB IMGUI_SRC
        "libs/imgui/*.h"
        "libs/imgui/*.cpp"
        "libs/imgui/backends/imgui_impl_glfw*"
        "libs/imgui/backends/imgui_impl_opengl3*"
        "libs/imgui/misc/cpp/imgui_stdlib.*"
)

add_executable(Voxelinity ${SRC} ${IMGUI_SRC})

# Set definition for assets directory
target_compile_definitions(Voxelinity PRIVATE ASSET_DIRECTORY="../assets/")

# Find OpenGL
set(OpenGL_GL_PREFERENCE "GLVND") # Target modern OpenGL
find_package(OpenGL REQUIRED)

# Check if GLFW is installed on the system
if(LINUX)
    find_package(glfw3 QUIET)
    if(glfw3_FOUND)
        message("GLFW found on the system!")
    else()
        message(FATAL_ERROR "GLFW not found on the system, please install glfw-devel library")
        return()
    endif()
else()
    add_subdirectory(libs/glfw)
    target_include_directories(Voxelinity PRIVATE "libs/glfw/include/")
endif()

add_subdirectory(libs/glad)
add_subdirectory(libs/glm)
add_subdirectory(libs/json)

# Add include directories
target_include_directories(Voxelinity PRIVATE
        "src/"
        "libs/glad/include/"
        "libs/glm/"
        "libs/json/include/"
        "libs/imgui/"
        "libs/imgui/backends/"
        "libs/imgui/misc/cpp"
        "libs/fastnoiselite/"
)

# Link libraries to the project
target_link_libraries(Voxelinity PRIVATE ${OPENGL_LIBRARY} glfw glad nlohmann_json)
