cmake_minimum_required(VERSION 3.22.1)

project(server)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置构建类型为 Debug
set(CMAKE_BUILD_TYPE Debug)

# 查找源文件
file(GLOB_RECURSE SRC src/*.cpp)
set(SRC ${SRC} main.cpp)

# 设置可执行文件输出目录为上一级目录
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/.)
# 创建可执行目标
add_executable(${PROJECT_NAME} ${SRC})

# 设置编译选项
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -g)


# 设置头文件目录
target_include_directories(${PROJECT_NAME} PRIVATE include)

# 设置库路径（静态库路径）
link_directories(./lib)

# 链接 Box2D 库
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/libbox2d.a)
