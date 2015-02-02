#include <string>
#include <cstdio>
#include <list>
#include "../ini_loader.h"

#define CONFIG_FILE "config.ini"

struct GameConf
{
    GameConf();
    struct ChannelConfig {
        size_t membus_buffer_length;
        size_t network_buffer_length;
        size_t vserver_buffer_length;
    };
    ChannelConfig channel;

    struct LogConfig {
        int level;
        bool auto_update_time;
        bool print_file;
        bool print_function;
        bool print_type;
        std::string print_time;

        int std_level_min;
        int std_level_max;

        int fs_level_min;
        int fs_level_max;
        std::string fs_path;
        std::string fs_suffix;
        size_t fs_file_number;
        size_t fs_file_size;
        bool fs_enable_buffer;
    };
    LogConfig log;

    struct VServerConfig {
        std::string main;
        uint32_t logic_frame_duration;
        int32_t logic_x;
        int32_t logic_x_offset;
        size_t logic_block_init_number;
    };
    VServerConfig vserver;

    bool init();
};

GameConf::GameConf() {
}

bool GameConf::init() {
    util::config::ini_loader conf_loader;

    conf_loader.load_file(CONFIG_FILE);

    // 通道配置
    {
        conf_loader.dump_to("channel.mem_bus.buffer.max_length", channel.membus_buffer_length);
        conf_loader.dump_to("channel.network.buffer.max_length", channel.network_buffer_length);
        conf_loader.dump_to("channel.vserver.buffer.max_length", channel.vserver_buffer_length);
    }


    // 默认资源和脚本目录配置
    {
        std::list<std::string> paths;
        conf_loader.dump_to("resource.res.dir", paths);

        paths.clear();
        conf_loader.dump_to("resource.script.client.dir", paths);

        paths.clear();
        conf_loader.dump_to("resource.script.client.cdir", paths);

        paths.clear();
        conf_loader.dump_to("resource.script.vserver.dir", paths);

        paths.clear();
        conf_loader.dump_to("resource.script.vserver.cdir", paths);

        conf_loader.dump_to("resource.script.vserver.main", vserver.main);
        conf_loader.dump_to("resource.script.vserver.logic_frame_duration", vserver.logic_frame_duration);
        conf_loader.dump_to("resource.script.vserver.logic_x", vserver.logic_x);
        conf_loader.dump_to("resource.script.vserver.logic_x_offset", vserver.logic_x_offset);
        conf_loader.dump_to("resource.script.vserver.logic_block_init_number", vserver.logic_block_init_number);
    }

    // 日志配置
    {
        conf_loader.dump_to("system.log.level", log.level);
        conf_loader.dump_to("system.log.auto_update_time", log.auto_update_time);
        conf_loader.dump_to("system.log.print_file", log.print_file);
        conf_loader.dump_to("system.log.print_function", log.print_function);
        conf_loader.dump_to("system.log.print_type", log.print_type);
        conf_loader.dump_to("system.log.print_time", log.print_time);

        conf_loader.dump_to("system.log.std.level.min", log.std_level_min);
        conf_loader.dump_to("system.log.std.level.max", log.std_level_max);

        conf_loader.dump_to("system.log.fs.level.min", log.fs_level_min);
        conf_loader.dump_to("system.log.fs.level.max", log.fs_level_max);
        conf_loader.dump_to("system.log.fs.path", log.fs_path);
        conf_loader.dump_to("system.log.fs.suffix", log.fs_suffix);
        conf_loader.dump_to("system.log.fs.file_number", log.fs_file_number);
        conf_loader.dump_to("system.log.fs.file_size", log.fs_file_size);
        conf_loader.dump_to("system.log.fs.enable_buffer", log.fs_enable_buffer);
    }

    return true;
}


int main() {
	GameConf gconf;
	
	printf("system.log.print_time: %s(%p)\n", gconf.log.print_time.c_str(), gconf.log.print_time.c_str());
	gconf.init();
	printf("system.log.print_time: %s(%p)\n", gconf.log.print_time.c_str(), gconf.log.print_time.c_str());
	return 0;
}