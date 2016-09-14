#include <fstream>
#include <algorithm>
#include <cstring>
#include <cstddef>

#include "ini_loader.h"

namespace util {
    namespace config {
        ini_value::ini_value() {
        }

        void ini_value::add(const std::string& val) {
            _data.push_back(val);
        }

        void ini_value::add(const char* begin, const char* end) {
            _data.push_back(std::string(begin, end));
        }

        bool ini_value::empty() const {
            return _data.empty() && _chirldren_nodes.empty();
        }

        bool ini_value::has_data() const {
            return false == _data.empty();
        }

        size_t ini_value::size() const {
            return _data.size();
        }

        void ini_value::clear() {
            _data.clear();
            _chirldren_nodes.clear();
        }

        ini_value& ini_value::operator[](const std::string key) {
            return _chirldren_nodes[key];
        }

        ini_value::node_type& ini_value::get_children() {
            return _chirldren_nodes;
        }

        const ini_value::node_type& ini_value::get_children() const {
            return _chirldren_nodes;
        }

        const std::string& ini_value::get_empty_string() {
            static std::string empty_data;
            return empty_data;
        }

        const std::string& ini_value::as_cpp_string(size_t index) const
        {
            if (index < _data.size()) {
                return _data[index];
            }

            return get_empty_string();
        }

        char ini_value::as_char(size_t index) const
        {
            return as<char>(index);
        }

        short ini_value::as_short(size_t index) const
        {
            return as<short>(index);
        }

        int ini_value::as_int(size_t index) const
        {
            return as<int>(index);
        }

        long ini_value::as_long(size_t index) const
        {
            return as<long>(index);
        }

        long long ini_value::as_longlong(size_t index) const
        {
            return as<long long>(index);
        }

        double ini_value::as_double(size_t index) const
        {
            return as<double>(index);
        }

        float ini_value::as_float(size_t index) const
        {
            return as<float>(index);
        }

        const char* ini_value::as_string(size_t index) const
        {
            return as_cpp_string(index).c_str();
        }

        // ============ unsigned ============
        unsigned char ini_value::as_uchar(size_t index) const
        {
            return as<unsigned char>(index);
        }

        unsigned short ini_value::as_ushort(size_t index) const
        {
            return as<unsigned short>(index);
        }

        unsigned int ini_value::as_uint(size_t index) const
        {
            return as<unsigned int>(index);
        }

        unsigned long ini_value::as_ulong(size_t index) const
        {
            return as<unsigned long>(index);
        }

        unsigned long long ini_value::as_ulonglong(size_t index) const
        {
            return as<unsigned long long>(index);
        }

        int8_t ini_value::as_int8(size_t index) const
        {
            return static_cast<int8_t>(as_int(index));
        }

        uint8_t ini_value::as_uint8(size_t index) const
        {
            return static_cast<uint8_t>(as_uint(index));
        }

        int16_t ini_value::as_int16(size_t index) const
        {
            return as<int16_t>(index);
        }

        uint16_t ini_value::as_uint16(size_t index) const
        {
            return as<uint16_t>(index);
        }

        int32_t ini_value::as_int32(size_t index) const
        {
            return as<int32_t>(index);
        }

        uint32_t ini_value::as_uint32(size_t index) const
        {
            return as<uint32_t>(index);
        }

        int64_t ini_value::as_int64(size_t index) const
        {
            return as<int64_t>(index);
        }

        uint64_t ini_value::as_uint64(size_t index) const
        {
            return as<uint64_t>(index);
        }

        namespace analysis
        {
            // space
            bool spaces::test_char(char c) {
                return (c == ' ' || c == '\r' || c == '\n' || c == '\t');
            }

            bool spaces::test(const char* begin, const char* end) {
                return begin < end && test_char(*begin);
            }

            const char* spaces::parse(const char* begin, const char* end) {
                while (begin < end &&
                    test_char(*begin)
                    ) {
                    ++begin;
                }

                return begin;
            }

            // comment
            bool comment::test(const char* begin, const char* end) {
                return begin < end && ((*begin) == '#' || (*begin) == ';');
            }

            const char* comment::parse(const char* begin, const char* end) {
                if (false == test(begin, end)) {
                    return begin;
                }

                return end;
            }

            // identify
            bool identify::test(const char* begin, const char* end) {
                return begin < end;
            }

            const char* identify::parse(const char* begin, const char* end) {
                _begin_ptr = _end_ptr = begin;
                if (false == test(begin, end)) {
                    return begin;
                }

                while (begin < end && (*begin) != ':' && (*begin) != '.' && (*begin) != '=') {
                    _end_ptr = (++begin);
                }

                // trim right
                while (_end_ptr > _begin_ptr && spaces::test_char(*(_end_ptr - 1)))
                    --_end_ptr;

                return begin;
            }

            // key
            bool key::test(const char* begin, const char* end) {
                return begin < end;
            }

            const char* key::parse(const char* begin, const char* end) {
                while (begin < end) {
                    if (false == test(begin, end)) {
                        return begin;
                    }

                    identify idt;
                    begin = idt.parse(begin, end);
                    if (idt._begin_ptr >= idt._end_ptr) {
                        return begin;
                    }

                    // 提取key
                    _keys.push_back(std::make_pair(idt._begin_ptr, idt._end_ptr));

                    spaces spliter;
                    begin = spliter.parse(begin, end);

                    if (begin >= end || (*begin) != '.') {
                        return begin;
                    }

                    begin = spliter.parse(begin + 1, end);
                }

                return begin;
            }

            // section
            bool section::test(const char* begin, const char* end) {
                return begin < end && (*begin) == '[';
            }

            const char* section::parse(const char* begin, const char* end) {
                if (false == test(begin, end)) {
                    return begin;
                }

                ++begin;
                spaces spliter;
                bool push_front = true;
                while (begin < end) {
                    // trim left
                    begin = spliter.parse(begin, end);
                    const char* start = begin;
                    while (begin < end && (*begin) != ':' && (*begin) != '.' && (*begin) != ']') {
                        ++begin;
                    }
                    
                    char stop_char = begin < end ? (*begin): 0;
                    
                    
                    // trim right
                    while (begin > start && spaces::test_char(*(begin - 1))) {
                        --begin;
                    }

                    if (start < begin) {
                        // 提取key
                        if (push_front) {
                            _keys.push_front(std::make_pair(start, begin));
                        } else {
                            _keys.push_back(std::make_pair(start, begin));
                        }
                    }


                    begin = spliter.parse(begin, end);

                    if (begin >= end) {
                        break;
                    }
                    
                    // 略过结尾的 ] 字符
                    if ((*begin) == ']') {
                        ++begin;
                        break;
                    }
                    
                    if ('.' == stop_char) {
                        push_front = false;
                    } else if (':' == stop_char) {
                        push_front = true;
                    }

                    begin = spliter.parse(begin + 1, end);
                }

                return begin;
            }

            // string
            char string::_convert_map[1 << (sizeof(char) * 8)] = { 0 };

            void string::init_conver_map() {
                if (_convert_map[(int)'0']) {
                    return;
                }

                _convert_map[(int)'0'] = '\0';
                _convert_map[(int)'a'] = '\a';
                _convert_map[(int)'b'] = '\b';
                _convert_map[(int)'f'] = '\f';
                _convert_map[(int)'r'] = '\r';
                _convert_map[(int)'n'] = '\n';
                _convert_map[(int)'t'] = '\t';
                _convert_map[(int)'v'] = '\v';
                _convert_map[(int)'\\'] = '\\';
                _convert_map[(int)'\''] = '\'';
                _convert_map[(int)'\"'] = '\"';
            }

            bool string::test(const char* begin, const char* end) {
                return begin < end && ((*begin) == '\'' || (*begin) == '\"');
            }

            const char* string::parse(const char* begin, const char* end, bool enable_convert) {
                if (false == test(begin, end)) {
                    return begin;
                }

                init_conver_map();
                char quot = *(begin++);

                // 禁止转义字符串
                if (false == enable_convert) {
                    const char* start = begin;
                    while (begin < end && (*begin) != quot) {
                        ++begin;
                    }
                    _value.assign(start, begin);

                    // 封闭字符串
                    if (begin < end) {
                        ++begin;
                    }
                } else { // 允许转义的逻辑复杂一些
                    while (begin < end && (*begin) != quot) {
                        // 转义字符
                        if ((*begin) == '\\' && begin + 1 < end) {
                            ++begin;
                            _value += _convert_map[(int) *(begin++)];
                            continue;
                        }

                        // 普通字符
                        _value += *(begin++);
                    }

                    // 封闭字符串
                    if (begin < end) {
                        ++begin;
                    }
                }

                return begin;
            }

            // value
            bool value::test(const char* begin, const char* end) {
                return begin < end;
            }

            const char* value::parse(const char* begin, const char* end) {
                if (false == test(begin, end)) {
                    return begin;
                }

                // trim left
                spaces spliter;
                begin = spliter.parse(begin, end);

                string rule;
                comment com_s;
                while (begin < end) {

                    if (rule.test(begin, end)) {
                        rule._value.clear();
                        begin = rule.parse(begin, end, (*begin) == '\"');
                        _value += rule._value;
                        continue;
                    }

                    if (com_s.test(begin, end)) {
                        begin = com_s.parse(begin, end);
                        continue;
                    }

                    _value += *(begin++);
                }

                // trim right
                size_t len = _value.size();
                while (len > 0) {
                    if (false == spaces::test_char(_value[len - 1])) {
                        break;
                    }

                    --len;
                }

                _value = _value.substr(0, len);

                return begin;
            }

            // expression
            bool expression::test(const char* begin, const char* end) {
                return _key.test(begin, end);
            }

            const char* expression::parse(const char* begin, const char* end) {
                if (false == test(begin, end)) {
                    return begin;
                }

                spaces spliter;

                begin = _key.parse(begin, end);
                begin = spliter.parse(begin, end);

                if (begin >= end || (*begin) != '=') {
                    return begin;
                }

                begin = spliter.parse(begin + 1, end);

                return _value.parse(begin, end);
            }

            // sentence
            bool sentence::test(const char* begin, const char* end) {
                return begin < end;
            }

            const char* sentence::parse(const char* begin, const char* end) {
                _sect.first = false;
                _exp.first = false;

                if (false == test(begin, end)) {
                    return begin;
                }

                spaces spliter;
                begin = spliter.parse(begin, end);

                // 空语句
                if (begin >= end) {
                    return begin;
                }

                // 纯注释语句
                comment com_s;
                if (com_s.test(begin, end)) {
                    return com_s.parse(begin, end);
                }

                // section语句
                _sect.first = _sect.second.test(begin, end);
                if (_sect.first) {
                    return _sect.second.parse(begin, end);
                }

                // expression语句
                _exp.first = _exp.second.test(begin, end);
                if (_exp.first) {
                    return _exp.second.parse(begin, end);
                }

                return begin;
            }
        }

        ini_loader::ini_loader() {
            _current_node_ptr = &_root_node;
        }

        ini_loader::~ini_loader() {
        }

        int ini_loader::load_stream(std::istream& in, bool is_append) {

            if (false == is_append) {
                clear();
            }

            unsigned char test_bom[3] = {0};
            test_bom[0] = in.get();
            test_bom[1] = in.get();
            test_bom[2] = in.get();
            const unsigned char utf8_bom[3] = {0xef, 0xbb, 0xbf};
                 
            if (0 != memcmp(test_bom, utf8_bom, 3)) {
                in.seekg(-3, std::ios::cur);
            }

            std::string line;
            while (std::getline(in, line)) {
                analysis::sentence one_sentence;
                one_sentence.parse(line.c_str(), line.c_str() + line.size());

                // section 节点会改变当前配置区域
                if (one_sentence._sect.first) {
                    _current_node_ptr = &get_root_node();
                    analysis::section::list_type::iterator iter =
                        one_sentence._sect.second._keys.begin();
                    for (; iter != one_sentence._sect.second._keys.end();
                        ++iter) {
                        if (iter->first >= iter->second)
                        {
                            continue;
                        }

                        std::string key;
                        key.assign(iter->first, iter->second);
                        _current_node_ptr = &get_node(key,
                            _current_node_ptr);
                    }
                }

                // expression 节点为配置值
                if (one_sentence._exp.first) {
                    ini_value* opr_node = &get_section();
                    analysis::key::list_type::iterator iter =
                        one_sentence._exp.second._key._keys.begin();
                    for (; iter != one_sentence._exp.second._key._keys.end();
                        ++iter) {
                        if (iter->first >= iter->second) {
                            continue;
                        }

                        std::string key;
                        key.assign(iter->first, iter->second);
                        opr_node = &get_node(key, opr_node);
                    }

                    opr_node->add(one_sentence._exp.second._value._value);
                }
            }

            return EIEC_SUCCESS;
        }

        int ini_loader::load_file(const char* file_path, bool is_append) {
            if (NULL == file_path) {
                return EIEC_OPENFILE;
            }

            std::ifstream file_to_load;
            file_to_load.open(file_path, std::ios::in);
            if (false == file_to_load.is_open()) {
                return EIEC_OPENFILE;
            }

            if (false == is_append) {
                clear();
            }

            return load_stream(file_to_load, is_append);
        }

        int ini_loader::load_file(const std::string& file_path, bool is_append) {
            return load_file(file_path.c_str(), is_append);
        }

        void ini_loader::clear() {
            _current_node_ptr = &_root_node;
            _root_node.clear();
        }

        void ini_loader::set_section(const std::string& path) {
            _current_node_ptr = &get_node(path, &get_root_node());
        }

        ini_value& ini_loader::get_section() {
            return *_current_node_ptr;
        }

        const ini_value& ini_loader::get_section() const {
            return *_current_node_ptr;
        }


        ini_value& ini_loader::get_root_node() {
            return _root_node;
        }

        const ini_value& ini_loader::get_root_node() const {
            return _root_node;
        }

        ini_value& ini_loader::get_node(const std::string& path, ini_value* father_ptr) {
            if (NULL == father_ptr) {
                father_ptr = &_root_node;
            }

            analysis::key _keys;
            const char* begin = path.c_str();
            const char* end = begin + path.size();

            analysis::spaces spliter;
            begin = spliter.parse(begin, end);

            _keys.parse(begin, end);
            analysis::section::list_type::iterator iter = _keys._keys.begin();
            for (; iter != _keys._keys.end(); ++iter) {
                if (iter->first >= iter->second) {
                    continue;
                }

                std::string key;
                key.assign(iter->first, iter->second);
                father_ptr = &get_child_node(key, father_ptr);
            }

            return *father_ptr;
        }

        ini_value& ini_loader::get_child_node(const std::string& path, ini_value* father_ptr) {
            if (NULL == father_ptr) {
                father_ptr = &_root_node;
            }

            return (*father_ptr)[path];
        }

        void ini_loader::dump_to(const std::string& path, bool& val, bool is_force, size_t index) {
            ini_value& cur_node = get_node(path);

            if (false == cur_node.has_data() && false == is_force) {
                return;
            }

            // no data
            if (false == cur_node.has_data() && is_force) {
                val = false;
                return;
            }

            std::string trans = cur_node.as_cpp_string(index);
            std::transform(trans.begin(), trans.end(), trans.begin(), ::tolower);
            val = true;

            if ("0" == trans || "false" == trans || "no" == trans || "disable" == trans || "disabled" == trans || "" == trans) {
                val = false;
            }
        }

        void ini_loader::dump_to(const std::string& path, std::string& val, bool is_force, size_t index) {
            ini_value& cur_node = get_node(path);
            if (cur_node.has_data() || is_force) {
                val = cur_node.as_cpp_string(index);
            }
        }

        void ini_loader::dump_to(const std::string& path, char* begin, char* end, bool is_force, size_t index) {
            ini_value& cur_node = get_node(path);
            if (cur_node.has_data() || is_force) {
                const std::string& val = cur_node.as_cpp_string(index);
                memcpy(begin, val.c_str(), std::min<size_t>(end - begin, val.size()));
                if (static_cast<size_t>(end - begin) > val.size()) {
                    memset(begin + val.size(), 0, end - begin - val.size());
                }
            }
        }
    }
}
