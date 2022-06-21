//
// Created by qza2468 on 22-6-21.
//

#include "Path_QZA.h"

using namespace std;

// translate s as a Path. translate '.', '..'
Path_QZA::Path_QZA(const string &s) {
    size_t pos_last = 0;
    vector<string> path_need_process;
    while (true) {
        auto pos = s.find('/', pos_last);
        if (pos == std::string::npos) {
            pos = s.size();
        }

        if (pos_last != pos) {
            path_need_process.emplace_back(s, pos_last, pos - pos_last);
        }

        if (pos == s.size()) {
            break;
        }

        pos_last = pos + 1;
    }

    for (auto & path_need_proces : path_need_process) {
        if (path_need_proces.empty() or path_need_proces == ".") {
            continue;
        } else if (path_need_proces == "..") {
            if (not paths.empty())
                paths.pop_back();
        } else {
            paths.emplace_back(path_need_proces);
        }
    }
}

// return string of path.
std::string Path_QZA::to_str() {
    std::string res;
    if (paths.empty()) {
        return "/";
    }
    for (const auto &item: paths) {
        res += "/" + item;
    }
    return res;
}

// validateBase64URL_safe if it's base64 encode.
bool Path_QZA::validateBase64(const string &s) {
    for (const auto &item:s) {
        if (not ((item >= 'A' and item <= 'Z')
                 or (item >= 'a' and item <= 'z')
                 or (item >= '0' and item <= '9')
                 or (item == '+' or item == '-')
                 or (item == '/' or item == '_')
                 or item == '='
        )) {
            return false;
        }
    }

    return true;
}

// validateBase64URL_safe if s consists of alpha, number and punctuation.
bool Path_QZA::validateBase64URL_safe(const string &s) {
    for (const auto &item: s) {
        if (not (isalnum(item) or item == '-' or item == '_' or item == '=' or item == '/'))
            return false;
    }

    return true;
}

Path_QZA &Path_QZA::operator+=(const Path_QZA &path1) {
    paths.insert(paths.end(), path1.paths.begin(), path1.paths.end());
    return *this;
}
