// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include <vector>
#include <string>
#include <sys/uio.h>
#include <type_traits> // std::disjunction

/**
 * @brief 缓冲区类
 */
class Buffer
{
private:
     std::vector<char> buffer_; // 缓冲区
     size_t writeIndex_;        // 写索引
     size_t writeSize_;         // 可写的空闲区域大小

     size_t readIndex_; // 读索引
     size_t readSize_;  // 可读的有效数据大小

     // 限制允许的类型
     template <typename T>
     using is_supported_type = std::disjunction<
         std::is_same<T, char>,
         std::is_same<T, float>,
         std::is_same<T, double>,
         std::is_same<T, int8_t>,
         std::is_same<T, uint8_t>,
         std::is_same<T, int16_t>,
         std::is_same<T, uint16_t>,
         std::is_same<T, int32_t>,
         std::is_same<T, uint32_t>,
         std::is_same<T, int64_t>,
         std::is_same<T, uint64_t>>;

public:
     Buffer(size_t initialSize = 1024);
     ~Buffer();

     /// @brief 获取缓冲区可读大小
     size_t readableBytes() const;

     /// @brief 清空缓冲区
     void clear();

     /// @brief 扩容以确保能再容纳len字节的数据，如果可写空间本来就够，则不扩容
     void ensureWritable(size_t len);

     /// 追加数据到缓冲区
     void append(const char *data, size_t len);

     /// 追加字符串到缓冲区
     void append(const std::string &str);

     /// @brief 获取buffer的可写区域的iovec结构体数组
     std::vector<iovec> getWriteIovec();

     /// 读取所有数据
     std::string readAllAsString();

     /// 读取指定长度的数据
     /// @return 返回实际读取到的字符串，可能不足len字节
     std::string readAsString(size_t len);

     /**
      * 读取指定长度的数据到data中
      * @return 返回实际读取的字节数
      */
     int read(char *data, size_t len);

     /**
      * 读取指定长度的数据到Buffer中
      * @return 返回实际读取的字节数
      */
     int read(Buffer &buf, size_t len);

     /// @brief 获取缓冲区第pos个可读字节的指针
     /// @note 从0开始计数
     char *operator[](size_t pos);

     /**
      * 从文件描述符读取数据到缓冲区
      */
     int readFd(int fd);

     /**
      * @brief 从缓冲区读取一个值(小端序)
      * 支持的类型包括：char、float、double、int8、uint8、int16、uint16、int32、uint32、int64、uint64
      * @tparam T 值的类型
      * @return T 读取到的值
      * @warning 按字节读取数据
      */
     template <typename T>
     T readValue()
     {
          static_assert(is_supported_type<T>::value, "无效类型");
          T value = 0;
          read(reinterpret_cast<char *>(&value), sizeof(value));
          return value;
     }

     /**
      * @brief 向缓冲区写入一个值(小端序)
      * 支持的类型包括：char、float、double、int8、uint8、int16、uint16、int32、uint32、int64、uint64
      * @tparam T 值的类型
      * @param value 要写入的值
      * @warning 此函数按字节写入数据
      */
     template <typename T>
     void writeValue(T value)
     {
          static_assert(is_supported_type<T>::value, "无效类型");
          append(reinterpret_cast<const char *>(&value), sizeof(value));
     }
};
