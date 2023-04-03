#ifndef UTILS
#define UTILS

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <regex>
#include <functional>

namespace utils{
    template<typename T>
    int match_first(const T& x, const std::vector<T>& y) {
        int m = -1;
        int i = -1;
        for (T y_elem : y) {
            i += 1;
            if (x == y_elem) {
                m = i;
                break;
            }
        }
        return(m);
    } 

    template<typename T>
    bool is_in(T &x, std::vector<T> &y) {
        return(match(x, y) != -999);
    }

    template<typename T>
    std::vector<T> vector_subset(
        std::vector<T> &x, 
        std::vector<int> &keep_indices
    ) {
        std::vector<T> x_subset(keep_indices.size());
        for (int i = 0; i < keep_indices.size(); i++) {
            int keep_idx = keep_indices[i];
            x_subset[i] = x[keep_idx];
        }
        return(x_subset);
    }

    template <typename... Args>
    bool re_detect(
        const std::string& x, 
        const std::regex& r,
        Args... args
    ) {
        std::smatch m;
        bool out = false;
        if (std::regex_search(x, m, r, args...)) {
            out = true;
        }
        return(out);
    }

    template <typename... Args>
    std::string re_extract(
        const std::string& x,
        const std::regex& r,
        Args... args
    ) {
        std::smatch m;
        std::string out = "";
        if (std::regex_search(x, m, r, args...)) {
            out = m.str();
        }
        return(out);
    }
    
    template <typename... Args>
    std::string re_extract_group(
        const std::string& x,
        const std::regex& r,
        const int n,
        Args... args
    ) {
        std::smatch m;
        std::string out = "";
        if (std::regex_search(x, m, r, args...)) {
            out = m[n].str();
        }
        return(out);
    }


    template <typename... Args>
    std::string re_extract_last_group(
        const std::string& x,
        const std::regex& r,
        Args... args
    ) {
        std::smatch m;
        std::string out = "";
        if (std::regex_search(x, m, r, args...)) {
            out = m[m.size() - 1].str();
        }
        return(out);
    }

    std::string key_regex_string() {
        return(".+");
    }

    std::regex tag_set_to_regex(const std::vector<std::string>& tag_set) {
        std::string s = "(";
        for (std::string tag : tag_set) {
            s += "(" + tag + ")|";
        }
        s.pop_back(); // remove trailing |
        s += ")[ ]*(" + key_regex_string() + ")[ ]*$";
        std::regex r = std::regex(s);
        return(r);
    }

}

#endif // UTILS