//
// Created by qza2468 on 22-6-21.
//

#ifndef UNTITLED8_PATH_QZA_H
#define UNTITLED8_PATH_QZA_H

#include <string>
#include <vector>

class Path_QZA {
public:
    std::vector<std::string> paths;
    explicit Path_QZA(const std::string &s);
    Path_QZA() = default;
    std::string to_str();
    static bool validateBase64(const std::string &s);
    static bool validateBase64URL_safe(const std::string &s);
    Path_QZA &operator+= (const Path_QZA &path1);
};


#endif //UNTITLED8_PATH_QZA_H
