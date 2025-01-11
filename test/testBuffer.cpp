#include "utils/Buffer.h"
#include <bits/stdc++.h>
using namespace std;

int main()
{
     srand(time(NULL));
     deque<char> q;
     Buffer b(10);
     /*-------------------测试模板---------------------*/
     for (int i = 0; i < 1000000; i++)
     {
          b.writeValue<int8_t>(127);
          b.writeValue<int16_t>(32767);
          b.writeValue<int32_t>(2147483647);
          b.writeValue<int64_t>(9223372036854775807);
          b.writeValue<uint8_t>(255);
          b.writeValue<uint16_t>(65535);
          b.writeValue<uint32_t>(4294967295);
          b.writeValue<uint64_t>(18446744073709551615ull);
          b.writeValue<float>(3.1415926f);
          b.writeValue<double>(2.718281828459045);
          b.writeValue<char>('a');
     }
     for (int i = 0; i < 1000000; i++)
     {
          int8_t i8 = b.readValue<int8_t>();
          assert(i8 == 127);
          int16_t i16 = b.readValue<int16_t>();
          assert(i16 == 32767);
          int32_t i32 = b.readValue<int32_t>();
          assert(i32 == 2147483647);
          int64_t i64 = b.readValue<int64_t>();
          assert(i64 == 9223372036854775807ll);
          uint8_t ui8 = b.readValue<uint8_t>();
          assert(ui8 == 255);
          uint16_t ui16 = b.readValue<uint16_t>();
          assert(ui16 == 65535);
          uint32_t ui32 = b.readValue<uint32_t>();
          assert(ui32 == 4294967295);
          uint64_t ui64 = b.readValue<uint64_t>();
          assert(ui64 == 18446744073709551615ull);
          float f = b.readValue<float>();
          assert(abs(f - 3.1415926f) < 1e-6);
          double d = b.readValue<double>();
          assert(abs(d - 2.718281828459045) < 1e-10);
          char c = b.readValue<char>();
          assert(c == 'a');
     }
     /*-------------------测试append和read-------------------------*/
     for (int i = 0; i < 500000; ++i)
     {
          int op = rand() % 2;
          if (op == 0)
          {
               int num = rand() % 10;
               for (int j = 0; j < num; ++j)
               {
                    string tmp = to_string(rand() % 100000000);
                    b.append(tmp);
                    // cout << 0 << " " << tmp << " " << b.writeIndex_ << " " << b.readIndex_ << " " << b.writeSize_ << " " << b.readSize_ << " " << b.buffer_.size() << endl;
                    for (auto c : tmp)
                    {
                         q.push_back(c);
                    }
               }
          }
          else if (op == 1)
          {
               if (q.size() == 0)
                    continue;
               int num = rand() % q.size();
               string s1, s2(num, '\0');
               s1 = b.readAsString(num);
               // cout << 1 << " " << num << " " << b.writeIndex_ << " " << b.readIndex_ << " " << b.writeSize_ << " " << b.readSize_ << " " << b.buffer_.size() << endl;
               for (int j = 0; j < num; ++j)
               {
                    s2[j] = q.front();
                    q.pop_front();
               }
               // cout << s1 << " " << s2 << endl;
               assert(s1 == s2);
          }
     }
     // cout << b.buffer_.size() << endl;
     return 0;
}
