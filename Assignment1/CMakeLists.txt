project(Assignment1)


# 这行使用file(GLOB ...)命令查找当前目录下所有的.h和.cpp文件，并将它们列表赋值给变量SOURCES。

list(APPEND SOURCES "main.cpp")
message("SOURCES: ${SOURCES}")
add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)