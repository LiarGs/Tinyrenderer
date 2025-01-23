project(Assignment4)
# 选择 Visual Studio 生成器
set(CMAKE_GENERATOR "Visual Studio 16 2022")  # 根据你的版本调整

# 这行使用file(GLOB ...)命令查找当前目录下所有的.h和.cpp文件，并将它们列表赋值给变量SOURCES。
file(GLOB ADDITIONAL_SOURCES "*.h" "*.cpp")
# 添加更多的文件到 SOURCES 变量中
list(APPEND SOURCES ${ADDITIONAL_SOURCES})

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE TBB::tbb)

# SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/PROFILE")