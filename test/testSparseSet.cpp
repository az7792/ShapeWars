#include "utils/SparseSet.h"
#include <unordered_set>
#include <iostream>
#include <ctime>
#include <cassert>
#include <vector>
#include <algorithm>
using namespace std;
int main()
{
     // 测试SparseSet
     int len = 10000;
     SparseSet s(len);
     std::unordered_set<int> uset;
     srand(time(NULL));
     // 随机len次操作(插入、查找、删除)
     for (int i = 0; i < len; i++)
     {
          int op = rand() % 3;
          if (op == 0)
          {
               int v = rand() % len;
               s.insert(v);
               uset.insert(v);
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
          }
          vector<int> v1, v2;
          for (auto v : uset)
          {
               v1.push_back(v);
          }
          for (auto v : s.getDense())
          {
               v2.push_back(v);
          }
          sort(v1.begin(), v1.end());
          sort(v2.begin(), v2.end());
          assert(v1 == v2);
     }
     return 0;
}