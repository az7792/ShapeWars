#!/bin/bash

# 检查并创建 build 目录
if [ ! -d "build" ]; then
  echo "'build'文件夹未找到,正在创建..."
  mkdir build || { echo "'build'文件夹创建失败"; exit 1; }
fi

# 切换到 build 目录
cd build/

cmake ..

# 执行 make
make || { echo "Make 失败"; exit 1; }

# 返回上一级目录
cd ..

echo "测试文件编译完成"
