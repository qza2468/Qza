//
// Created by qza2468 on 22-6-14.
//

#include "file.h"

using namespace std;


crow::response create_file_route(const crow::request &req) {
    string username = check_token(req).name;
    if (username.empty()) {
        return {401, "you are not a user, but hello!"};
    }

    crow::response path_resp = check_path(req, "file-path", username);
    if (path_resp.code != 200) {
        return path_resp;
    }
    string filePathStr = path_resp.body;

    int f = open(filePathStr.c_str(), O_RDWR | O_CREAT | O_EXCL, 0777);
    if (f == -1) {
        return {404, string("open error:") + to_string(errno)};
    }

    size_t remain = req.body.size();
    int zero_times = 16;
    while (remain) {
        size_t res_code = write(f, req.body.data() + req.body.size() - remain, remain);
        if (res_code == -1 or (res_code == 0 and --zero_times == 0)) {
            unlink(filePathStr.c_str());
            close(f);
            return {404, "write error"};
        }

        remain -= res_code;
    }

    close(f);
    return {200, "ok"};
}

crow::response mkdir_route(const crow::request &req) {
    string username = check_token(req).name;
    if (username.empty()) {
        return {401, "you are not a user, but hello!"};
    }

    crow::response path_resp = check_path(req, "file-path", username);
    if (path_resp.code != 200) {
        return path_resp;
    }
    string filePathStr = path_resp.body;

    try {
        filesystem::create_directory(filePathStr);
    } catch (exception &e) {
        return {404, e.what()};
    }

    return {200, "ok"};
}

crow::response unlink_route(const crow::request &req) {
    string username = check_token(req).name;
    if (username.empty()) {
        return {401, "you are not a user, but hello!"};
    }

    bool dir_all = not check_header(req, "dir-all").empty();

    crow::response path_resp = check_path(req, "file-path", username);
    if (path_resp.code != 200) {
        return path_resp;
    }
    string filePathStr = path_resp.body;

    try {
        if (not dir_all) {
            filesystem::remove(filePathStr);
        } else {
            filesystem::remove_all(filePathStr);
        }
    } catch (const exception &e) {
        return {404, e.what()};
    }

    return {200, "ok"};
}

crow::response rename_file_route(const crow::request &req) {
    string username = check_token(req).name;
    if (username.empty()) {
        return {401, "you are not a user, but hello!"};
    }

    crow::response path_resp = check_path(req, "file-path", username);
    if (path_resp.code != 200) {
        return path_resp;
    }
    string filePathStr = path_resp.body;

    crow::response path_resp_new = check_path(req, "file-path-new", username);
    if (path_resp_new.code != 200) {
        return path_resp_new;
    }
    string filePathStr_new = path_resp_new.body;

    try {
        filesystem::rename(filePathStr, filePathStr_new);
    } catch (const exception &e) {
        return {404, e.what()};
    }

    return {200, "ok"};
}

void download_file_route(const crow::request &req, crow::response &resp) {
    string username = check_token(req).name;
    if (username.empty()) {
        resp.code = 401;
        resp.body = "you are not a user, but hello!";
        resp.end();
        return;
    }

    crow::response path_resp = check_path(req, "file-path", username);
    if (path_resp.code != 200) {
        resp.code = path_resp.code;
        resp.body = path_resp.body;

        resp.end();
        return;
    }
    string filePathStr = path_resp.body;

    resp.code = 200;
    resp.set_static_file_info_unsafe(filePathStr);
    resp.end();
}

crow::response list_dir_route(const crow::request &req) {
    string username = check_token(req).name;
    if (username.empty()) {
        return {401, "you are not a user, but hello!"};
    }

    crow::response path_resp = check_path(req, "file-path", username);
    if (path_resp.code != 200) {
        return path_resp;
    }
    string filePathStr = path_resp.body;

    vector<crow::json::wvalue> res_list;
    try {
        for (const auto &entry: filesystem::directory_iterator(filePathStr)) {
            if (not(entry.is_directory() or entry.is_regular_file())) {
                continue;
            }
            crow::json::wvalue item_dict;
            item_dict["name"] = entry.path().filename().string();
            item_dict["file-size"] = entry.is_regular_file() ? entry.file_size() : 4096;
            item_dict["type"] = entry.is_regular_file() ? "file" : (entry.is_directory() ? "directory" : "");
            res_list.emplace_back(item_dict);
        }
    } catch (const filesystem::filesystem_error &e) {
        return {404, e.what()};
    }

    crow::json::wvalue res_json;
    res_json["items"] = crow::json::wvalue(res_list);

    return res_json;
}

string check_header(const crow::request &req, const string &header_key) {
    auto iter = req.headers.find(header_key);
    if (iter != req.headers.end()) {
        return iter->second;
    }

    return {};
}

crow::response check_path(const crow::request &req, const string& header_key, const string &username) {
    Path_QZA user_path(BASE_DIR);

    string filePath_base64 = check_header(req, header_key);

    if (filePath_base64.empty()) {
        return {404, "file path should be specified"};
    }

    if (not Path_QZA::validateBase64(filePath_base64)) {
        return {404, "headers.path: wrong base64 encode"};
    }
    string filePathStr = crow::utility::base64decode(filePath_base64);
    if (filePathStr.empty()) {
        return {404, "headers.path: wrong base64 encode"};
    }
    if (not Path_QZA::validateBase64URL_safe(filePathStr)) {
        return {404, "filePath should only contain alpha, num and punctuation"};
    }

    user_path += Path_QZA(username);

    user_path += Path_QZA(filePathStr);

    return {200, user_path.to_str()};
}



