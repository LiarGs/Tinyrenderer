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

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
include_directories(Base)

file(GLOB SOURCES Base/*.h Base/*.cpp)
add_subdirectory(Assignment1)
# add_subdirectory(Assignment2)
# add_subdirectory(Assignment3)
# add_subdirectory(Assignment4)

# # 这行使用file(GLOB ...)命令查找当前目录下所有的.h和.cpp文件，并将它们列表赋值给变量SOURCES。



# add_executable(${PROJECT_NAME} ${SOURCES})
