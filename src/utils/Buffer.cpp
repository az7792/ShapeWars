#include "utils/Buffer.h"
#include <cstring>

Buffer::Buffer(size_t initialSize)
    : buffer_(initialSize), writeIndex_(0), writeSize_(initialSize),
      readIndex_(0), readSize_(0)
{
}

Buffer::~Buffer() {}

size_t Buffer::readableBytes() const {
    return readSize_;
}

void Buffer::clear()
{
    readIndex_ = writeIndex_ = 0;
    readSize_ =  0;
    writeSize_ = buffer_.size();
}


void Buffer::ensureWritable(size_t len)
{
    if (len == 0)
    {
        return;
    }
    if (writeSize_ < len) // 需要扩容
    {
        /*________read________write_________*/
        if (writeIndex_ >= readIndex_ && writeSize_ > 0)
        {
            buffer_.resize(2 * std::max(buffer_.size(), readSize_ + len));
            writeSize_ = buffer_.size() - readSize_; // 更新扩容后的空闲区域大小
        }
        else /*________write_________read_________*/
        {
            size_t readLen = buffer_.size() - readIndex_;
            size_t oldReadIndex = readIndex_;

            buffer_.resize(2 * std::max(buffer_.size(), readSize_ + len));
            readIndex_ = buffer_.size() - readLen;   // 新readIndex_
            writeSize_ = buffer_.size() - readSize_; // 更新扩容后的空闲区域大小

            // 将readIndex_后面的数据重新移动到扩容后的尾部
            std::memmove(buffer_.data() + readIndex_, buffer_.data() + oldReadIndex, readLen);
        }
    }
}

void Buffer::append(const char *data, size_t len)
{
    if (!data || len == 0)
    {
        return;
    }

    ensureWritable(len);

    /*________read________write_________*/
    if (writeIndex_ >= readIndex_ && writeSize_ > 0)
    {
        size_t len1 = buffer_.size() - writeIndex_; // write右边剩余空间
        if (len1 >= len)                            // 直接写入
        {
            std::memcpy(buffer_.data() + writeIndex_, data, len);
            writeIndex_ = (writeIndex_ + len) % buffer_.size();
        }
        else // 先写入write右边剩余空间，再写入左边剩余空间
        {
            std::memcpy(buffer_.data() + writeIndex_, data, len1);
            std::memcpy(buffer_.data(), data + len1, len - len1);
            writeIndex_ = len - len1;
        }
    }
    else /*________write_________read_________*/
    {
        std::memcpy(buffer_.data() + writeIndex_, data, len);
        writeIndex_ += len;
    }
    writeSize_ -= len;
    readSize_ += len;
}

void Buffer::append(const std::string &str)
{
    append(str.data(), str.size());
}

std::vector<iovec> Buffer::getWriteIovec()
{
    std::vector<iovec> iovs(2, {nullptr, 0});
    /*________read________write_________*/
    if (writeIndex_ >= readIndex_ && writeSize_ > 0)
    {
        // R
        iovs[0].iov_base = buffer_.data() + writeIndex_;
        iovs[0].iov_len = buffer_.size() - writeIndex_;
        // L
        iovs[1].iov_base = buffer_.data();
        iovs[1].iov_len = readIndex_;
    }
    else /*________write_________read_________*/
    {
        iovs[0].iov_base = buffer_.data() + writeIndex_;
        iovs[0].iov_len = writeSize_;
    }

    if (iovs.back().iov_len == 0) // 处理读写指针在0位置并且buf为空的情况
        iovs.pop_back();
    return iovs;
}

std::string Buffer::readAllAsString()
{
    return readAsString(readSize_);
}

std::string Buffer::readAsString(size_t len)
{
    len = std::min(len, readSize_);
    std::string str(len, '\0');
    read(str.data(), len);
    return str;
}

int Buffer::read(char *data, size_t len)
{
    if (len == 0 || data == nullptr) // 不必要读取或不可读取
        return 0;

    len = std::min(len, readSize_);
    for (size_t i = 0; i < len; i++)
    {
        data[i] = buffer_[readIndex_];
        readIndex_ = (readIndex_ + 1) % buffer_.size();
    }
    readSize_ -= len;
    writeSize_ += len;
    return len;
}

int Buffer::readFd(int fd)
{
    std::vector<iovec> iovs = getWriteIovec();
    int n = ::readv(fd, iovs.data(), iovs.size());
    if (n >= 0)
    {
        readSize_ += n;
        writeSize_ -= n;
        writeIndex_ = (writeIndex_ + n) % buffer_.size();
    }
    return n;
}