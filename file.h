//
// Created by qza2468 on 22-6-14.
//

#ifndef UNTITLED8_FILE_H
#define UNTITLED8_FILE_H

#include "login.h"
#include "config_qza.h"
#include "crow.h"
#include "Path_QZA.h"
#include <iostream>
#include <filesystem>



crow::response check_path(const crow::request &req,  const std::string& header_key, const std::string &username);
std::string check_header(const crow::request &req, const std::string &header_key);

crow::response create_file_route(const crow::request &req);
crow::response mkdir_route(const crow::request &req);

crow::response unlink_route(const crow::request &req);

crow::response rename_file_route(const crow::request &req);

void download_file_route(const crow::request &req, crow::response &resp);

crow::response list_dir_route(const crow::request &req);

#endif //UNTITLED8_FILE_H
