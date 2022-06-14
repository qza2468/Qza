//
// Created by qza2468 on 22-6-14.
//

#include "file.h"

using namespace std;

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

    for (size_t i = 0; i < path_need_process.size(); i++) {
        if (path_need_process[i].empty() or path_need_process[i] == ".") {
            continue;
        } else if (path_need_process[i] == "..") {
            if (not paths.empty())
                paths.pop_back();
        } else {
            paths.emplace_back(path_need_process[i]);
        }
    }
}

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

bool Path_QZA::validate(const string &s) {
    for (const auto &item: s) {
        if (not (isalnum(item) or ispunct(item)))
            return false;
    }

    return true;
}

Path_QZA &Path_QZA::operator+=(const Path_QZA &path1) {
    paths.insert(paths.end(), path1.paths.begin(), path1.paths.end());
    return *this;
}


crow::response create_file_route(const crow::request &req) {
    string username = check_token(req).name;
    if (username.empty()) {
        return {401, "you are not a user, but hello!"};
    }

    crow::response path_resp = check_path(req);
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

crow::response unlink_route(const crow::request &req) {
    string username = check_token(req).name;
    if (username.empty()) {
        return {401, "you are not a user, but hello!"};
    }

    crow::response path_resp = check_path(req);
    if (path_resp.code != 200) {
        return path_resp;
    }
    string filePathStr = path_resp.body;

    error_code errorCode;
    bool ok = filesystem::remove(filePathStr, errorCode);
    if (ok) {
        return {200, "ok"};
    } else {
        return {404, string("delete error: ") + errorCode.message()};
    }
}

crow::response check_path(const crow::request &req) {
    Path_QZA path("/home/qza2468/");

    string filePath_base64;
    {
        auto iter = req.headers.find("file-path");
        if (iter != req.headers.end()) {
            filePath_base64 = iter->second;
        }
    }

    if (filePath_base64.empty()) {
        return {404, "file path should be specified"};
    }

    if (not Path_QZA::validateBase64(filePath_base64)) {
        return {404, "headers.path: wrong base64 encode"};
    }
    string filePathStr = crow::utility::base64decode(filePath_base64);
    if (not Path_QZA::validate(filePathStr)) {
        return {404, "filePath should only contain alpha, num and punctuation"};
    }
    path += filePathStr;
    filePathStr = path.to_str();

    return {200, filePathStr};
}


