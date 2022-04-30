cmake_minimum_required(VERSION 3.10)

IF(POLICY CMP0012)
    CMAKE_POLICY(SET CMP0012 NEW)
ENDIF()

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
message("-- CAMKE_BUILD_GENERATOR : ${CAMKE_BUILD_GENERATOR}") # 输出从cmake运行传过来的buildGenerator参数是什么，如x86和x64等，这个参数是自定义的，如果不传默认编译64位(我当前系统64位)

find_package(glfw3)
find_package(Boost)
find_package(assimp)

include_directories("/home/ren/Require/glad/include")
include_directories("thirdclass" "inc" "src")

file(GLOB project_file thirdclass/glad.c thirdclass/stb_image.cpp src/main.cpp) # 需要把glad.c也包含进来
add_executable(${PROJECT_NAME} ${project_file}) #这里一定要把构成可执行文件的所有源文件都添加进去，否则就会出错！！！

target_link_libraries(${PROJECT_NAME}
    -lglfw3
    -lGL
    -lGLU
    -lglut
    -lm
    -lXrandr
    -lXi
    -lX11
    -lpthread
    -ldl
    -lXinerama
    -lXcursor
    )