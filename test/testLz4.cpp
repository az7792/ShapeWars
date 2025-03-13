// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#include "lz4/lz4.h"
#include <iostream>
#include <string>
// valgrind --leak-check=full --show-leak-kinds=all ./server
int main()
{
     std::string src = "hello world asdadad asdadadasd\n";
     char dst[100];
     char compressed[100];
     int c_size = LZ4_compress_default(src.data(), dst, src.size(), 100);
     std::cout << "compressed size: " << c_size << std::endl;
     int d_size = LZ4_decompress_safe(dst, compressed, c_size, 100);
     compressed[d_size] = '\0';
     printf("%s", compressed);
     return 0;
}