cmake_minimum_required(VERSION 3.10)
#set(CMAKE_TOOLCHAIN_FILE "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
# Set the project name and version
project(VI-RT)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "-g")

# Include directories and link libraries based on the operating system
if(WIN32)
    set(VCPKG_INSTALLED_PATH "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows")
    set(VCPKG_INCLUDE_DIR "${VCPKG_INSTALLED_PATH}/include")
    set(VCPKG_LIBRARY_DIR "${VCPKG_INSTALLED_PATH}/lib")

    include_directories(${VCPKG_INCLUDE_DIR})
    link_directories(${VCPKG_LIBRARY_DIR})

    find_package(OpenEXR CONFIG REQUIRED)
    find_package(libjpeg-turbo CONFIG REQUIRED)

    include_directories(${libjpeg-turbo_INCLUDE_DIRS})
    include_directories(${OpenEXR_INCLUDE_DIRS})
else() #Linux and Mac
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    find_package(GLEW REQUIRED)
endif()

# Add main executable
add_executable(main main.cpp)

# Add subdirectories
add_library(LibCamera Camera/camera.hpp Camera/perspective.cpp Camera/perspective.hpp)
add_library(LibImage Image/image.hpp Image/ImagePPM.cpp Image/ImagePPM.hpp)
add_library(LibPrimitive Primitive/BRDF/BRDF.hpp Primitive/BRDF/Phong.cpp Primitive/BRDF/Phong.hpp Primitive/Geometry/geometry.hpp Primitive/Geometry/mesh.hpp Primitive/Geometry/mesh.cpp Primitive/Geometry/geometry.hpp Primitive/Geometry/triangle.hpp Primitive/Geometry/triangle.cpp Primitive/BB.hpp Primitive/primitive.hpp)
add_library(LibRenderer Renderer/renderer.hpp Renderer/StandardRenderer.cpp Renderer/StandardRenderer.hpp)
add_library(LibScene Scene/scene.cpp Scene/scene.hpp)
add_library(LibShader Shader/AmbientShader/AmbientShader.cpp Shader/WhittedShader/WhittedShader.cpp Shader/DistributedShader/DistributedShader.cpp Shader/PathTracerShader/PathTracerShader.cpp Shader/AmbientShader/AmbientShader.hpp Shader/WhittedShader/WhittedShader.hpp Shader/DistributedShader/DistributedShader.hpp Shader/PathTracerShader/PathTracerShader.hpp Shader/shader.hpp)

# Link libraries
target_link_libraries(main LibCamera LibPrimitive LibScene LibShader LibRenderer LibImage OpenEXR::OpenEXR libjpeg-turbo::turbojpeg)

