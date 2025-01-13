cmake_minimum_required(VERSION 3.22.1)

project(test)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置构建类型为 Debug
set(CMAKE_BUILD_TYPE Debug)

# 查找源文件
file(GLOB SRC ./../src/*.cpp ./../src/**/*.cpp)

# 设置测试文件的可执行文件输出目录
set(EXECUTABLE_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/build)

# 手动填写测试文件
set(TEST_FILES
    ./testWebsocketServer.cpp
    ./testTcpServer.cpp
    # 添加更多测试文件
)

# 将源文件编译成静态库
add_library(src_objects STATIC ${SRC})

# 设置静态库的头文件目录
target_include_directories(src_objects PUBLIC ./../include)

# 设置库路径（静态库路径）
link_directories(/../lib)

# 为每个测试文件创建单独的目标
foreach(TEST_FILE ${TEST_FILES})
    # 获取测试文件名（去掉路径和扩展名）
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    # 定义测试文件的可执行目标
    add_executable(${TEST_NAME} ${TEST_FILE})

    # 设置测试目标的编译选项
    target_compile_options(${TEST_NAME} PRIVATE -Wall -g)

    # 链接静态库
    target_link_libraries(${TEST_NAME} PRIVATE src_objects ${CMAKE_CURRENT_SOURCE_DIR}/../lib/libbox2d.a)
endforeach()
