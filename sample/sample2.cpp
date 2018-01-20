#include "ini_loader.h"
#include <cstdio>
#include <list>
#include <vector>


#define CONFIG_FILE "test.ini"

int main() {
    util::config::ini_loader cfg_loader;

    cfg_loader.load_file(CONFIG_FILE);


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

        cfg_loader.dump_to("d.c.b.a.e.f1", t2);                   // 字符串
        cfg_loader.dump_to("d.c.b.a.e.f1", t3.begin(), t3.end()); // 字符串迭代器
        cfg_loader.dump_to("d.c.b.a.e.f2", t1);                   // 字符串
        cfg_loader.dump_to("d.c.b.a.e.f2", t1 + 16, t1 + 32);     // 字符串指针迭代器
        cfg_loader.dump_to("a.b.c3", t4);                         // 带不可打印字符的字符串
        cfg_loader.dump_to("d.c.b.a.e.f3", t5);                   // 字符串 - Section使用.分割层级

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
            printf("t2[%d] = %s\n", (int)index++, (*iter) ? "true" : "false");
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