#include <map>
#include <vector>
#include <string>
#include <initializer_list>
#include <fstream>
#include <sstream>
#include <type_traits>


#define JSON_TRUE "true"
#define JSON_FALSE "false"
#define JSON_NULL "null"

namespace JSONcpp {

    class json;

    enum class json_type {
        json_string,
        json_float,
        json_int,
        json_bool,
        json_list,
        json_object,
        json_null
    };

    const json_type null = json_type::json_null;


    inline bool firstAndLastMatch(std::string str, char first, char last);
    inline bool strContains(std::string str, char c);
    inline bool isNum(char c);
    inline bool isEmpty(char c);
    inline bool writeFile(const char* filename, std::string content);
    inline std::string readFile(const char* filename);
    inline json parse(std::string val);
    inline json read(const char* filename);
    inline bool write(const char* filename, json val, int tabSpace = 0);

    class json {

    public:
        json_type type;


        // Constructors
        json(json_type type = json_type::json_object)
            : type(type) {}
        json(std::string val)
            : type(json_type::json_string), s_val(val) {}
        json(const char* val)
            : type(json_type::json_string), s_val(val) {}
        json(double val)
            : type(json_type::json_float), f_val(val) {}
        json(float val)
            : type(json_type::json_float), f_val(val) {}
        json(int val)
            : type(json_type::json_int), i_val(val) {}
        json(bool val)
            : type(json_type::json_bool), b_val(val) {}
        json(std::vector<json> val)
            : type(json_type::json_list), l_val(val) {}
        json(std::initializer_list<json> val)
            : type(json_type::json_list), l_val(val) {
            operator=(val);
        }
        json(std::map<std::string, json> val)
            : type(json_type::json_object), o_val(val) {}


        //Operator overloading



        void operator=(std::string val) { type = json_type::json_string; s_val = val; }
        void operator=(const char* val) { type = json_type::json_string; s_val = val; }
        void operator=(double val) { type = json_type::json_float; f_val = val; }
        void operator=(float val) { type = json_type::json_float; f_val = val; }
        void operator=(int val) { type = json_type::json_int; i_val = val; }
        void operator=(bool val) { type = json_type::json_bool; b_val = val; }
        void operator=(std::vector<json> val) { type = json_type::json_list; l_val = val; }
        void operator=(std::map<std::string, json> val) { type = json_type::json_object; o_val = val; }
        void operator=(json_type newType) { type = newType; }


        void operator=(std::initializer_list<json> val) {
            bool isList = false;

            std::map<std::string, json> m_val;

            for (json j : val) {
                if (j.type == json_type::json_list &&
                    j.l_val.size() == 2 &&
                    j.l_val[0].type == json_type::json_string) {
                    m_val[j.l_val[0].val<std::string>()] = j.l_val[1];
                }
                else {
                    isList = true;
                    break;
                }
            }

            if (isList) {
                type = json_type::json_list;
                l_val = val;
            }
            else {
                type = json_type::json_object;
                o_val = m_val;
            }
        }


        friend bool operator==(const json& lhs, const json& rhs) {
            if (lhs.type != rhs.type) {
                return false;
            }

            switch (lhs.type) {
            case json_type::json_string:
                return lhs.s_val == rhs.s_val;
            case json_type::json_float:
                return lhs.f_val == rhs.f_val;
            case json_type::json_int:
                return lhs.i_val == rhs.i_val;
            case json_type::json_bool:
                return lhs.b_val == rhs.b_val;
            case json_type::json_list:
                return lhs.l_val == rhs.l_val;
            case json_type::json_object:
                return lhs.o_val == rhs.o_val;
            case json_type::json_null:
                return true; 
            }
        }


        json& operator+=(const json& val) {
            switch (type) {
            case json_type::json_string:

                if (val.type == json_type::json_string) {
                    s_val += val.s_val;
                }
                break;
            case json_type::json_float:
                if (val.type == json_type::json_float) {
                    f_val += val.f_val;
                }
                else if (val.type == json_type::json_int) {
                    f_val += val.i_val;
                }
                break;
            case json_type::json_int:
                if (val.type == json_type::json_int) {
                    i_val += val.i_val;
                }
                else if (val.type == json_type::json_float) {
                    i_val += val.f_val;
                }
                break;
            case json_type::json_list:
                l_val.push_back(val);
                break;
            };

            return *this;
        }
        friend json operator+(json& val1, json& val2) {
            val1 += val2;
            return val1;
        }

        void addAll(json list) {
            if (type == json_type::json_list && list.type == json_type::json_list) {
                for (json val : list.l_val) {
                    l_val.push_back(val);
                }
            }
        }

        bool eraseKey(std::string key) {
            if (type == json_type::json_object) {
                return o_val.erase(key) != 0;
            }
            else {
                return false;
            }
        }

        bool erase(json item) {
            if (type == json_type::json_list) {
                unsigned int size = l_val.size();
                for (int i = 0; i < size; i++) {
                    if (l_val[i] == item) {
                        l_val.erase(l_val.begin() + i);
                        return true;
                    }
                }
                return false;
            }
            else {
                return false;
            }
        }

        unsigned int length() {
            if (type == json_type::json_list) {
                return l_val.size();
            }
            else {
                return -1;
            }
        }


        json& operator[](int idx) { return l_val[idx]; }
        json& operator[](std::string key) { return o_val[key]; }

        template <typename T> T val() { return NULL; }
        template <> std::string val<std::string>() { return s_val; }
        template <> const char* val<const char*>() { return s_val.c_str(); }
        template <> double val<double>() {
            return type == json_type::json_float ? f_val : i_val;
        }
        template <> float val<float>() {
            return type == json_type::json_float ? f_val : i_val;
        }
        template <> int val<int>() {
            return type == json_type::json_int ? i_val : f_val;
        }
        template <> bool val<bool>() { return b_val; }

        std::string getType() {
            switch (type) {
            case json_type::json_string:
                return "string";
            case json_type::json_float:
                return "float";
            case json_type::json_int:
                return "int";
            case json_type::json_bool:
                return "bool";
            case json_type::json_list:
                return "list";
            case json_type::json_object:
                return "json object";
            case json_type::json_null:
                return "null";
            }
        }

    private:

        std::string s_val;
        float f_val;
        int i_val;
        bool b_val;
        std::vector<json> l_val;
        std::map<std::string, json> o_val;


    public:
        static std::vector<json> getObj(std::string str, int& i, bool breakAfterAdd = false, int last_i_offset = 1, bool endBracket = false) {
            std::vector<json> ret;

            char c;
            std::string val = "";
            int openCount = 0;
            bool quoteOpen = false;

            std::string openDelimeters = "[{";
            std::string closeDelimeters = "]}";

            for (; i < str.length() - last_i_offset; i++) {
                c = str[i];

                if (isEmpty(c)) {
                    if (quoteOpen) {
                        val += c;
                    }
                    continue;
                }

                if (str[i - 1] != '\\') {
                    if (c == '"') {
                        openCount += (quoteOpen ? -1 : 1);

                        quoteOpen = !quoteOpen;
                    }

                    if (strContains(openDelimeters, c)) {
                        openCount++;
                    }
                    else if (strContains(closeDelimeters, c)) {
                        openCount--;
                    }
                }

                if (openCount <= 0 && val != "" && (c == ',' || (endBracket && c == '}'))) {
                    if (endBracket && openCount == 0 && c == '}') {
                        val += '}';
                        i++;
                    }

                    ret.push_back(parse(val));

                    if (breakAfterAdd) {
                        return ret;
                    }

                    val = "";
                }
                else {
                    val += c;
                }
            }

            ret.push_back(parse(val));

            return ret;
        }

    private:
        std::string stringify(bool prettify, int tabSpace = 0, int initTabPos = 0, bool isDictVal = false) {
            std::stringstream ret;

            std::string tab;
            for (int i = 0; i < tabSpace; i++) { tab += ' '; }

            if (!isDictVal && prettify) {
                for (int i = 0; i < initTabPos; i++) { ret << tab; }
            }

            std::stringstream sec_stream;
            std::string sec_str;

            switch (type) {
            case json_type::json_string:
                ret << '"' << s_val << '"';
                break;
            case json_type::json_float:
                ret << f_val;
                break;
            case json_type::json_int:
                ret << i_val;
                break;
            case json_type::json_bool:
                ret << (b_val ? JSON_TRUE : JSON_FALSE);
                break;
            case json_type::json_list:
                if (prettify) {
                    ret << '[' << std::endl;
                }
                else {
                    ret << '[';
                }

                for (json data : l_val) {
                    if (prettify) {
                        sec_stream << data.stringify(true, tabSpace, initTabPos + 1) << ',' << std::endl;
                    }
                    else {
                        sec_stream << data.stringify(false) << ',';
                    }

                }
                sec_str = sec_stream.str();
                sec_str = sec_str.substr(0, sec_str.length() - (prettify ? 2 : 1));

                if (prettify) {
                    ret << sec_str << std::endl;
                    for (int i = 0; i < initTabPos; i++) { ret << tab; }
                }
                else {
                    ret << sec_str;
                }

                ret << ']';
                break;
            case json_type::json_object:

                if (prettify) {
                    ret << '{' << std::endl;
                }
                else {
                    ret << '{';
                }

                for (std::pair<std::string, json> pair : o_val) {

                    if (prettify) {
                        for (int i = 0; i < initTabPos + 1; i++) { sec_stream << tab; }
                        sec_stream << '"' << pair.first << "\": " << pair.second.stringify(true, tabSpace, initTabPos + 1, true) << ',' << std::endl;
                    }
                    else {
                        sec_stream << '"' << pair.first << "\":" << pair.second.stringify(false) << ',';
                    }
                }
                sec_str = sec_stream.str();
                sec_str = sec_str.substr(0, sec_str.length() - (prettify ? 2 : 1));

                if (prettify) {
                    ret << sec_str << std::endl;
                    for (int i = 0; i < initTabPos; i++) { ret << tab; }
                }
                else {
                    ret << sec_str;
                }

                ret << '}';
                break;
            case json_type::json_null:
                ret << JSON_NULL;
                break;
            }

            return ret.str();
        }

    public:
        std::string dump(int tabSpace = 0) {
            if (tabSpace == 0) {

                return stringify(false);
            }
            else {
                return stringify(true, tabSpace);
            }
        }

        bool dump(const char* filename, int tabSpace = 0) {
            return writeFile(filename, dump(tabSpace));
        }
    };


    inline bool firstAndLastMatch(std::string str, char first, char last) {
        return str[0] == first && str[str.length() - 1] == last;
    }

    inline bool strContains(std::string str, char c) {
        return str.find(c) != std::string::npos;
    }

    inline bool isNum(char c) {
        return strContains("0123456789.-", c);
    }

    inline bool isEmpty(char c) {
        return c == ' ' || c == '\n' || c == '\t';
    }

    inline bool writeFile(const char* filename, std::string content) {
        std::ofstream file(filename);

        if (file.is_open()) {
            file << content;
            file.close();
            return true;
        }

        return false;
    }

    inline std::string readFile(const char* filename) {
        std::ifstream file(filename);
        std::string ret = "";

        if (file.is_open()) {
            std::stringstream sstream;
            sstream << file.rdbuf();
            ret = sstream.str();
        }

        file.close();

        return ret;
    }

    inline json parse(std::string val) {
        char c = val[0];

        if (firstAndLastMatch(val, '"', '"')) {
            return json(val.substr(1, val.length() - 2));
        }
        else if (isNum(c)) {
            if (strContains(val, '.')) {
                return json(std::atof(val.c_str()));
            }
            else {
                return json(std::atoi(val.c_str()));
            }
        }
        else if (c == 't') {
            if (val == JSON_TRUE) {
                return json(true);
            }
        }
        else if (c == 'f') {
            if (val == JSON_FALSE) {
                return json(false);
            }
        }
        else if (c == 'n') {
            if (val == JSON_NULL) {
                return json(json_type::json_null);
            }
        }
        else if (firstAndLastMatch(val, '[', ']')) {
            int i = 1;

            return json::getObj(val, i);
        }
        else if (firstAndLastMatch(val, '{', '}')) {

            std::map<std::string, json> ret;

            int i = 1;

            std::string key = "";

            while (i < val.length()) {

                c = val[i];

                while (isEmpty(c) && i < val.length()) {
                    c = val[++i];
                }

                if (c != '"') {
                    break;
                }

                c = val[++i];

                for (; c != '"' && val[i - 1] != '\\'; c = val[++i]) {
                    key += c;
                }

                c = val[++i];

                while (isEmpty(c) || c == ':') {
                    c = val[++i];
                }

                std::vector<json> retv = json::getObj(val, i, true, 0, true);

                i++;

                ret[key] = retv[0];

                key = "";
            }

            return ret;
        }

        return json();
    }

    inline json read(const char* filename) {
        return parse(readFile(filename));
    }

    inline bool write(const char* filename, json val, int tabSpace) {
        return writeFile(filename, val.dump(tabSpace));
    }
}
