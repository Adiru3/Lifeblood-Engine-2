cmake_minimum_required(VERSION 3.10)
project(LifebloodEngine VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Includes
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/libs)
include_directories(${CMAKE_BINARY_DIR}/libs/glad/include)

# Output
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)



# Fetch Dependencies
include(FetchContent)

# GLFW
FetchContent_Declare(
    glfw
    URL https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.zip
)
FetchContent_MakeAvailable(glfw)

# GLAD
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/libs/glad/include/glad)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/libs/glad/include/KHR)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/libs/glad/src)

if(NOT EXISTS ${CMAKE_BINARY_DIR}/libs/glad/include/glad/glad.h)
    file(DOWNLOAD https://raw.githubusercontent.com/JoeyDeVries/LearnOpenGL/master/includes/glad/glad.h
         ${CMAKE_BINARY_DIR}/libs/glad/include/glad/glad.h)
endif()
if(NOT EXISTS ${CMAKE_BINARY_DIR}/libs/glad/include/KHR/khrplatform.h)
    file(DOWNLOAD https://raw.githubusercontent.com/JoeyDeVries/LearnOpenGL/master/includes/KHR/khrplatform.h
         ${CMAKE_BINARY_DIR}/libs/glad/include/KHR/khrplatform.h)
endif()
if(NOT EXISTS ${CMAKE_BINARY_DIR}/libs/glad/src/glad.c)
    file(DOWNLOAD https://raw.githubusercontent.com/JoeyDeVries/LearnOpenGL/master/src/glad.c
         ${CMAKE_BINARY_DIR}/libs/glad/src/glad.c)
endif()

# Miniaudio
FetchContent_Declare(
    miniaudio
    URL https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(miniaudio)

# STB Image
FetchContent_Declare(
    stb_image
    URL https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(stb_image)

FetchContent_Declare(
    stb_image_write
    URL https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(stb_image_write)

# STB Easy Font (For UI)
if(NOT EXISTS ${CMAKE_BINARY_DIR}/libs/stb_easy_font.h)
    file(DOWNLOAD https://raw.githubusercontent.com/nothings/stb/master/stb_easy_font.h
         ${CMAKE_BINARY_DIR}/libs/stb_easy_font.h)
endif()

# TinyObjLoader
FetchContent_Declare(
    tinyobjloader
    URL https://raw.githubusercontent.com/tinyobjloader/tinyobjloader/release/tiny_obj_loader.h
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(tinyobjloader)

# TinyGLTF (For Skeletal Animation)
FetchContent_Declare(
    tinygltf
    URL https://raw.githubusercontent.com/syoyo/tinygltf/master/tiny_gltf.h
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(tinygltf)

# nlohmann_json (Required by tinygltf)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(json)



include_directories(${CMAKE_BINARY_DIR}/libs) # Include dir for easy font
include_directories(${tinyobjloader_SOURCE_DIR})
include_directories(${tinygltf_SOURCE_DIR})
include_directories(${json_SOURCE_DIR})


# Includes for deps
include_directories(${miniaudio_SOURCE_DIR})
include_directories(${stb_image_SOURCE_DIR})
include_directories(${stb_image_write_SOURCE_DIR})

# Sources
file(GLOB_RECURSE SOURCES "src/*.cpp")
list(APPEND SOURCES "${CMAKE_BINARY_DIR}/libs/glad/src/glad.c")
# Ensure Particle System is included (GLOB should catch it, but good to check)
# Ensure Particle System is included (GLOB should catch it, but good to check)
# list(APPEND SOURCES "${CMAKE_BINARY_DIR}/libs/glad/src/glad.c") # Duplicated


add_executable(Lifeblood ${SOURCES})

target_link_libraries(Lifeblood PRIVATE glfw opengl32 dsound winmm ws2_32 crypt32) 

add_custom_command(TARGET Lifeblood POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/bin/assets
)
