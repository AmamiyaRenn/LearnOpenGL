cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME learnGL)
project(${PROJECT_NAME} LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

if(CMAKE_HOST_WIN32)
    set(WINDOWS 1)
elseif(CMAKE_HOST_APPLE)
    set(MACOS 1)
elseif(CMAKE_HOST_UNIX)
    set(LINUX 1)
endif()

find_package(glfw3)
find_package(glm)
find_package(assimp)

include_directories(inc)
include_directories(thirdclass)

file(GLOB project_file thirdclass/glad/glad.c thirdclass/stb/stb_image.cpp src/main.cpp) # 需要把glad.c也包含进来
add_executable(${PROJECT_NAME} ${project_file}) # 这里一定要把构成可执行文件的所有源文件都添加进去，否则就会出错！！！

target_link_libraries(${PROJECT_NAME}
    glfw
    glm
    assimp
)