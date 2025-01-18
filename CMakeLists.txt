cmake_minimum_required (VERSION 3.5)
# project(tinyrenderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP)
if(OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories(src
                    src/Test
                    src/Texture
                    src/Shader
                    src/Scene
                    # 添加其他子目录
)
# 递归查找src目录及其子目录下的所有头文件和cpp文件
file(GLOB_RECURSE SOURCES "src/*.h" "src/*.cpp")

find_package(OpenCV REQUIRED)

add_subdirectory(Assignment1)
add_subdirectory(Assignment2)
add_subdirectory(Assignment3)
add_subdirectory(Assignment4)
