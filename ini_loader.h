/**
 * @file ini_loader.h
 * @brief ini解析器
 * Licensed under the MIT licenses.
 *
 * @note 与标准ini有略微不同，请注意
 * 1. 支持Section
 * 2. Secion支持父子关系,即 [ A : B : C ] 语法
 * 3. 支持多重父子关系,如 C.B.A = d
 * 4. 支持字符串转义，其中以'包裹的不进行转义，以"包裹的可进行转义,如 C.B.A = "Hello \r\n World!\0"
 * 5. 配置的Key名称不能包含引号('和")、点(.)、冒号(:)和方括号([])
 * 6. 配置的Key名称区分大小写
 * 7. #符号也将是做行注释，与 ; 等价
 *
 * @version 1.0
 * @author owentou, owt5008137@live.com
 * @date 2013年11月16日
 *
 * @history
 *   2014-07-14: 修正空值问题, 优化API
 *   2015-02-02: 修正字符串未配置会导致崩溃的BUG
 */

#ifndef _UTIL_CONFIG_INI_INILOADER_H_
#define _UTIL_CONFIG_INI_INILOADER_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <stdint.h>
#include <string>
#include <sstream>
#include <map>
#include <list>
#include <vector>

namespace util {
    namespace config {
        // ================= 错误码 =================
        enum EN_INILOADER_ERROR_CODE
        {
            EIEC_SUCCESS = 0,
            EIEC_OPENFILE = -1,
        };
        // ----------------- 错误码 -----------------

        // ================= 存储层 =================
        class ini_value
        {
        public:
            typedef std::map<std::string, ini_value> node_type;
        private:
            std::vector<std::string> _data;
            node_type _chirldren_nodes;


            template<typename _Tt>
            inline void clear_data(_Tt& data) const {}

            inline void clear_data(char*& data) const { data = NULL; }
            inline void clear_data(std::string& data) const { data.clear(); }
            inline void clear_data(bool& data) const { data = false; }
            inline void clear_data(char& data) const { data = 0; }
            inline void clear_data(short& data) const { data = 0; }
            inline void clear_data(int& data) const { data = 0; }
            inline void clear_data(long& data) const { data = 0; }
            inline void clear_data(long long& data) const { data = 0; }
            inline void clear_data(unsigned char& data) const { data = 0; }
            inline void clear_data(unsigned short& data) const { data = 0; }
            inline void clear_data(unsigned int& data) const { data = 0; }
            inline void clear_data(unsigned long& data) const { data = 0; }
            inline void clear_data(unsigned long long& data) const { data = 0; }

            template<typename _Tt>
            inline _Tt string2any(const std::string& data) const {
                _Tt ret;
                clear_data(ret);
                if (!data.empty()) {
                    std::stringstream s_stream;
                    s_stream.str(data);
                    s_stream >> ret;
                }
                return ret;
            }

        public:
            ini_value();

            void add(const std::string& val);
            void add(const char* begin, const char* end);

            // 节点操作
            bool empty() const; // like stl
            bool has_data() const; // like stl
            size_t size() const; // like stl
            void clear(); // like stl
            ini_value& operator[](const std::string key);
            node_type& get_children();
            const node_type& get_children() const;

            static const std::string& get_empty_string();

            // 数值转换操作
            template<typename _Tt>
            inline _Tt as(size_t index = 0) const {
                if (index <= _data.size()) {
                    return string2any<_Tt>(_data[index]);
                }

                return string2any<_Tt>(get_empty_string());
            }

            // 获取存储对象的字符串
            const std::string& as_cpp_string(size_t index = 0) const;

            char as_char(size_t index = 0) const;

            short as_short(size_t index = 0) const;

            int as_int(size_t index = 0) const;

            long as_long(size_t index = 0) const;

            long long as_longlong(size_t index = 0) const;

            double as_double(size_t index = 0) const;

            float as_float(size_t index = 0) const;

            const char* as_string(size_t index = 0) const;

            unsigned char as_uchar(size_t index = 0) const;

            unsigned short as_ushort(size_t index = 0) const;

            unsigned int as_uint(size_t index = 0) const;

            unsigned long as_ulong(size_t index = 0) const;

            unsigned long long as_ulonglong(size_t index = 0) const;

            int8_t as_int8(size_t index = 0) const;

            uint8_t as_uint8(size_t index = 0) const;

            int16_t as_int16(size_t index = 0) const;

            uint16_t as_uint16(size_t index = 0) const;

            int32_t as_int32(size_t index = 0) const;

            uint32_t as_uint32(size_t index = 0) const;

            int64_t as_int64(size_t index = 0) const;

            uint64_t as_uint64(size_t index = 0) const;
        };
        // ----------------- 存储层 -----------------

        // ================= 词法状态机 =================
        namespace analysis {
            // space
            struct spaces
            {
                static bool test_char(char c);
                bool test(const char* begin, const char* end);
                const char* parse(const char* begin, const char* end);
            };

            // comment
            struct comment
            {
                bool test(const char* begin, const char* end);
                const char* parse(const char* begin, const char* end);
            };

            // identify
            struct identify
            {
                const char* _begin_ptr;
                const char* _end_ptr;

                bool test(const char* begin, const char* end);
                const char* parse(const char* begin, const char* end);
            };

            // key
            struct key
            {
                typedef std::list<std::pair<const char*, const char*> > list_type;
                list_type _keys;

                bool test(const char* begin, const char* end);
                const char* parse(const char* begin, const char* end);
            };

            // section
            struct section
            {
                typedef std::list<std::pair<const char*, const char*> > list_type;
                list_type _keys;

                bool test(const char* begin, const char* end);
                const char* parse(const char* begin, const char* end);
            };

            // string
            struct string
            {
                static char _convert_map[1 << (sizeof(char) * 8)];
                void init_conver_map();

                std::string _value;

                bool test(const char* begin, const char* end);
                const char* parse(const char* begin, const char* end, bool enable_convert = false);
            };

            // value
            struct value
            {
                std::string _value;

                bool test(const char* begin, const char* end);
                const char* parse(const char* begin, const char* end);
            };

            // expression
            struct expression
            {
                key _key;
                value _value;

                bool test(const char* begin, const char* end);
                const char* parse(const char* begin, const char* end);
            };

            // sentence
            struct sentence
            {
                std::pair<bool, section> _sect;
                std::pair<bool, expression> _exp;

                bool test(const char* begin, const char* end);
                const char* parse(const char* begin, const char* end);
            };
        }
        // ----------------- 词法状态机 -----------------

        class ini_loader
        {
        private:
            ini_value _root_node; // root node
            ini_value* _current_node_ptr;

        public:
            ini_loader();
            ~ini_loader();

            /**
             * @brief 从流读取数据
             * @param in 输入流
             * @param is_append 是否是追加模式
             * @return 返回码
             * @see EN_INILOADER_ERROR_CODE
             */
            int load_stream(std::istream& in, bool is_append = false);

            /**
             * @brief 从文件取数据
             * @param file_path 输入文件
             * @param is_append 是否是追加模式
             * @return 返回码
             * @see EN_INILOADER_ERROR_CODE
             */
            int load_file(const char* file_path, bool is_append = false);

            /**
             * @brief 从文件取数据
             * @param file_path 输入文件
             * @param is_append 是否是追加模式
             * @return 返回码
             * @see EN_INILOADER_ERROR_CODE
             */
            int load_file(const std::string& file_path, bool is_append = false);

            /**
             * @brief 清空
             */
            void clear();

            /**
             * @brief 设置当前配置结构根节点路径
             * @param path 路径
             */
            void set_section(const std::string& path);

            /**
             * @brief 获取当前配置结构根节点
             * @return 当前配置结构根节点
             */
            ini_value& get_section();

            /**
             * @brief 获取当前配置结构根节点
             * @return 当前配置结构根节点
             */
            const ini_value& get_section() const;

            /**
             * @brief 获取根节点
             * @return 根节点
             */
            ini_value& get_root_node();

            /**
             * @brief 获取根节点
             * @return 根节点
             */
            const ini_value& get_root_node() const;

            /**
             * @brief 根据目录获取子节点
             * @param path 节点相对路径
             * @param father_ptr 父节点，设为空则相对于根节点
             * @return 子节点
             * @note 如果子节点不存在会创建空列表节点
             */
            ini_value& get_node(const std::string& path, ini_value* father_ptr = NULL);

            /**
             * @brief 根据子节点名称获取子节点
             * @param path 子节点名称（注意不是路径）
             * @param father_ptr 父节点，设为空则相对于根节点
             * @return 子节点
             * @note 如果子节点不存在会创建空列表节点
             */
            ini_value& get_child_node(const std::string& path, ini_value* father_ptr = NULL);

            // ========================= 单值容器转储 =========================

            /**
             * @brief 配置转储
             * @param path 配置路径
             * @param val 转储目标
             * @param is_force 是否是强制转储，强制在找不到路径对应的配置项时会尝试清空数据
             * @param index 转储索引，默认是第一个值
             */
            template<typename Ty>
            void dump_to(const std::string& path, Ty& val, bool is_force = false, size_t index = 0) {
                ini_value& cur_node = get_node(path);
                if (cur_node.has_data() || is_force) {
                    val = cur_node.as<Ty>(index);
                }
            }

            /**
             * @brief 配置转储
             * @param path 配置路径
             * @param val 转储目标
             * @param is_force 是否是强制转储，强制在找不到路径对应的配置项时会尝试清空数据
             * @param index 转储索引，默认是第一个值
             */
            void dump_to(const std::string& path, bool& val, bool is_force = false, size_t index = 0);

            /**
             * @brief 配置转储
             * @param path 配置路径
             * @param val 转储目标
             * @param is_force 是否是强制转储，强制在找不到路径对应的配置项时会尝试清空数据
             * @param index 转储索引，默认是第一个值
             */
            void dump_to(const std::string& path, std::string& val, bool is_force = false, size_t index = 0);

            /**
            * @brief 配置转储 - 字符串
            * @param path 配置路径
            * @param begin 转储目标起始地址
            * @param end 转储目标边界地址
            * @param is_force 是否是强制转储，强制在找不到路径对应的配置项时会尝试清空数据
            * @param index 转储索引，默认是第一个值
            */
            void dump_to(const std::string& path, char* begin, char* end, bool is_force = false, size_t index = 0);

            /**
             * @brief 配置转储 - 字符串
             * @param path 配置路径
             * @param val 转储目标
             * @param is_force 是否是强制转储，强制在找不到路径对应的配置项时会尝试清空数据
             * @param index 转储索引，默认是第一个值
             */
            template<size_t MAX_COUNT>
            void dump_to(const std::string& path, char(&val)[MAX_COUNT], bool is_force = false, size_t index = 0) {
                dump_to(path, val, val + MAX_COUNT, is_force, index);
            }

            // ========================= 多值容器转储 =========================
            /**
             * @brief 配置转储 - 容器
             * @param path 配置路径
             * @param val 转储目标
             * @param is_force 是否是强制转储，强制在找不到路径对应的配置项时会尝试清空数据
             * @note 容器足够大时，会尝试转储所有配置
             */
            template<typename Ty>
            void dump_to(const std::string& path, std::vector<Ty>& val, bool is_force = false) {
                if (is_force) {
                    val.clear();
                }

                ini_value& cur_node = get_node(path);
                for (size_t i = 0; i < cur_node.size(); ++i) {
                    val.push_back(Ty());
                    Ty& new_node = val.back();
                    dump_to(path, new_node, is_force, i);
                }
            }

            /**
             * @brief 配置转储 - 容器
             * @param path 配置路径
             * @param val 转储目标
             * @param is_force 是否是强制转储，强制在找不到路径对应的配置项时会尝试清空数据
             * @note 容器足够大时，会尝试转储所有配置
             */
            template<typename Ty>
            void dump_to(const std::string& path, std::list<Ty>& val, bool is_force = false) {
                if (is_force) {
                    val.clear();
                }

                ini_value& cur_node = get_node(path);
                for (size_t i = 0; i < cur_node.size(); ++i) {
                    val.push_back(Ty());
                    Ty& new_node = val.back();
                    dump_to(path, new_node, is_force, i);
                }
            }

            /**
             * @brief 配置转储 - 数组
             * @param path 配置路径
             * @param val 转储目标
             * @param is_force 是否是强制转储，强制在找不到路径对应的配置项时会尝试清空数据
             * @note 容器足够大时，会尝试转储所有配置
             */
            template<typename Ty, size_t MAX_COUNT>
            void dump_to(const std::string& path, Ty (&val)[MAX_COUNT], bool is_force = false) {
                ini_value& cur_node = get_node(path);
                for (size_t i = 0; i < cur_node.size() && i < MAX_COUNT; ++i) {
                    dump_to(path, val[i], is_force, i);
                }
            }

            /**
             * @brief 配置转储 - 迭代器
             * @param path 配置路径
             * @param begin 转储目标起始迭代器
             * @param end 转储目标边界迭代器
             * @param is_force 是否是强制转储，强制在找不到路径对应的配置项时会尝试清空数据
             * @note 容器足够大时，会尝试转储所有配置
             */
            template<typename TIter>
            void dump_to(const std::string& path, TIter begin, TIter end, bool is_force = false) {
                size_t index = 0;
                ini_value& cur_node = get_node(path);
                for (TIter i = begin; i != end && index < cur_node.size(); ++index, ++i) {
                    dump_to(path, *i, is_force, index);
                }
            }
        };
    }
}

#endif
