libiniloader
============

libiniloader 是一个简洁的ini配置读取库，仅需要两个文件。


注意
------
本库读取ini的方式和标准ini有略微不同，主要区别如下：

1. 支持Section
2. Secion支持父子关系,即 [ A : B : C ] 语法
3. 支持多重父子关系,如 C.B.A = d
4. 支持字符串转义，其中以'包裹的不进行转义，以"包裹的可进行转义,如 C.B.A = "Hello \r\n World!\0"
5. 配置的Key名称不能包含引号('和")、点(.)、冒号(:)和方括号([])
6. 配置的Key名称区分大小写
7. #符号也将是做行注释，与 ; 等价
8. 支持解析时间长度（dump_to到\:\:util\:\:config\:\:duration_value类型），单位如下
  + 纳秒: ns, nanosecond, nanoseconds     如: 100ns
  + 微秒: us, microsecond, microseconds   如: 100us
  + 毫秒: ms, millisecond, milliseconds   如: 100ms
  + 秒: s, sec, second, seconds           如: 100s
  + 分: m, minute, minutes                如: 100m
  + 时: h, hour, hours                    如: 100h
  + 天: d, day, days                      如: 100d
  + 周: w, week, weeks                    如: 100w


使用方法
------
使用libiniloader非常简单，只需要包含头文件 ini_loader.h 并编译源文件 ini_loader.cpp 即可

其他文档详见 [ini_loader.h](ini_loader.h) 文件内注释

使用示例
------
ini文件：(test.ini)

```ini
a.b.c1 = 123  # 整数
a.b.c2 = 1.23 ; 小数

[a : b:c:d]
e.f1 = "123\0456" ; 可转义字符串

e.f2 = '123\0456'; 不可转义字符串

[d . c:b.a]
e.f3 = 123456789

[a]
b.c3 = 带空格    的 字符		串

arr = 1
arr = 2   
arr = 3   
arr = 		一坨屎
arr = 		/usr/local/gcc-4.8.2


bool = true
bool = false
bool = yes
bool = no
bool = enable
bool = disable
bool = 1
bool = 0
```

测试代码:
```cpp
#include <vector>
#include <list>
#include <cstdio>
#include "ini_loader.h"


int main(){
    util::config::ini_loader cfg_loader;

    cfg_loader.load_file("test.ini");


    // 转储整数
    {
        int t1 = 9999, t2 = 9999;
        cfg_loader.dump_to("a.b.c1", t1);
        cfg_loader.dump_to("a.b.c4", t2, true);
        printf("a.b.c1 = %d\na.b.c4 = %d\n", t1, t2);
    }

    // 转储浮点数
    {
        float t1 = 0.0;
        cfg_loader.dump_to("a.b.c2", t1);
        printf("a.b.c2 = %f\n", t1);
    }

    // 转储字符串
    {
        char t1[32] = {0};
        std::string t2, t3 = "0000000000000000";
        std::string t4, t5;

        cfg_loader.dump_to("d.c.b.a.e.f1", t2); // 字符串
        cfg_loader.dump_to("d.c.b.a.e.f1", t3.begin(), t3.end()); // 字符串迭代器
        cfg_loader.dump_to("d.c.b.a.e.f2", t1); // 字符串
        cfg_loader.dump_to("d.c.b.a.e.f2", t1 + 16, t1 + 32); // 字符串指针迭代器
        cfg_loader.dump_to("a.b.c3", t4); // 带不可打印字符的字符串
        cfg_loader.dump_to("d.c.b.a.e.f3", t5); // 字符串 - Section使用.分割层级

        printf("len(t2) = %d\nlen(t3) = %d\n", (int)t2.size(), (int)t3.size());
        printf("d.c.b.a.e.f2 = %s\n", t1);
        printf("d.c.b.a.e.f2 = %s(+16)\n", t1 + 16);
        printf("d.c.b.a.e.f3 = %s\n", t5.c_str());
        printf("a.b.c3 = %s\n", t4.c_str());
    }

    // 转储到 vector
    {
        std::vector<std::string> t1;
        std::list<bool> t2;
        cfg_loader.dump_to("a.arr", t1);
        cfg_loader.dump_to("a.bool", t2);

        for (size_t i = 0; i < t1.size(); ++i) {
            printf("t1[%d] = %s\n", (int)i, t1[i].c_str());
        }

        size_t index = 0;
        for (std::list<bool>::iterator iter = t2.begin(); iter != t2.end(); ++iter) {
            printf("t2[%d] = %s\n", (int) index ++, (*iter) ? "true" : "false");
        }

    }

    // 转储时间周期
    {
        util::config::duration_value dur;
        cfg_loader.dump_to("system.interval_ns", dur, true);
        printf("system.interval_ns: sec %lld, nsec: %lld\n", static_cast<long long>(dur.sec), static_cast<long long>(dur.nsec));
        cfg_loader.dump_to("system.interval_us", dur, true);
        printf("system.interval_us: sec %lld, nsec: %lld\n", static_cast<long long>(dur.sec), static_cast<long long>(dur.nsec));
        cfg_loader.dump_to("system.interval_ms", dur, true);
        printf("system.interval_ms: sec %lld, nsec: %lld\n", static_cast<long long>(dur.sec), static_cast<long long>(dur.nsec));
        cfg_loader.dump_to("system.interval_s", dur, true);
        printf("system.interval_s: sec %lld, nsec: %lld\n", static_cast<long long>(dur.sec), static_cast<long long>(dur.nsec));
        cfg_loader.dump_to("system.interval_m", dur, true);
        printf("system.interval_m: sec %lld, nsec: %lld\n", static_cast<long long>(dur.sec), static_cast<long long>(dur.nsec));
        cfg_loader.dump_to("system.interval_h", dur, true);
        printf("system.interval_h: sec %lld, nsec: %lld\n", static_cast<long long>(dur.sec), static_cast<long long>(dur.nsec));
        cfg_loader.dump_to("system.interval_d", dur, true);
        printf("system.interval_d: sec %lld, nsec: %lld\n", static_cast<long long>(dur.sec), static_cast<long long>(dur.nsec));
        cfg_loader.dump_to("system.interval_w", dur, true);
        printf("system.interval_w: sec %lld, nsec: %lld\n", static_cast<long long>(dur.sec), static_cast<long long>(dur.nsec));
    }

    return 0;
}
```

LICENSE
------
MIT License


联系作者
------
E-Mail: [owt5008137@live.com](mailto:owt5008137@live.com) | [admin@owent.net](mailto:admin@owent.net)

QQ: 493749449 (请注明 github-libiniloader)
