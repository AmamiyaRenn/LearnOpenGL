cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME_STR learnGl1)
project(${PROJECT_NAME_STR} LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

if (CMAKE_HOST_WIN32)
    set(WINDOWS 1)
elseif(CMAKE_HOST_APPLE)
    set(MACOS 1)
elseif(CMAKE_HOST_UNIX)
    set(LINUX 1)
endif()
 
message("-- CMAKE_BUILD_TYPE : ${CMAKE_BUILD_TYPE}") # 输出从cmake运行传过来的buildType参数是什么，如Debug和Release等

find_package(glfw3)
find_package(Boost)

set(CMAKE_MODULE_PATH  "/home/ren/Require/assimp-5.1.4/cmake-modules")
find_package(assimp)

include_directories("/home/ren/Require/assimp-5.1.4/include")
include_directories("/home/ren/Require/glad/include")
include_directories("thirdclass" "inc" "src")

file(GLOB project_file thirdclass/glad.c thirdclass/stb_image.cpp src/main.cpp) # 需要把glad.c也包含进来
add_executable(${PROJECT_NAME} ${project_file}) #这里一定要把构成可执行文件的所有源文件都添加进去，否则就会出错！！！

target_link_libraries(${PROJECT_NAME}
    -lglfw3
    -lGL
    -lGLU
    # glut
    -lm
    -lXrandr
    -lXi
    -lX11
    -lpthread
    -ldl
    -lXinerama
    -lXcursor
    assimp
    )