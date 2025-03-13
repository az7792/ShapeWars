// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#include "utils/SparseMap.h"
#include <unordered_set>
#include <map>
#include <iostream>
#include <ctime>
#include <cassert>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;
int main()
{
     // 测试SparseSet
     int len = 10000;
     SparseMap<string> s(len);
     std::map<int, string> uset;
     srand(time(NULL));
     // 随机len次操作(插入、查找、删除)
     for (int i = 0; i < len; i++)
     {
          int op = rand() % 3;
          if (op == 0)
          {
               int v = rand() % (2 * len);
               s.insert(v, to_string(v + 100));
               uset.emplace(v, to_string(v + 100));
          }
          else if (op == 1)
          {
               int v = rand() % len;
               s.erase(v);
               uset.erase(v);
          }
          else if (op == 2)
          {
               int v = rand() % len;
               bool ok = (s.find(v) == (uset.find(v) != uset.end()));
               assert(ok);
               if (s.find(v))
               {
                    string s1 = s[v];
                    string s2 = uset[v];
                    assert(s1 == s2);
               }
          }
     }
     for (auto v : s)
     {
          cout << v.first << ":" << v.second << endl;
     }
     s.insert(1, "0");
     cout << s[1] << endl;
     s[1] = "123";
     cout << s[1] << endl;
     s[1] = "456";
     cout << s[1] << endl;
     return 0;
}