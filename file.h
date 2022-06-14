//
// Created by qza2468 on 22-6-14.
//

#ifndef UNTITLED8_FILE_H
#define UNTITLED8_FILE_H

#include "login.h"
#include "config_qza.h"
#include "crow.h"
#include <filesystem>

class Path_QZA {
public:
    std::vector<std::string> paths;
    explicit Path_QZA(const std::string &s);
    Path_QZA() = default;
    std::string to_str();
    static bool validateBase64(const std::string &s);
    static bool validate(const std::string &s);
    Path_QZA &operator+= (const Path_QZA &path1);
};

crow::response check_path(const crow::request &req,  const std::string& header_key);

crow::response create_file_route(const crow::request &req);

crow::response unlink_route(const crow::request &req);

crow::response rename_file_route(const crow::request &req);

crow::response download_file_route(const crow::request &req);

crow::response list_dir_route(const crow::request &req);

#endif //UNTITLED8_FILE_H
