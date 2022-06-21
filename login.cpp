//
// Created by qza2468 on 22-6-12.
//

#include "login.h"

#include "config_qza.h"
#include <openssl/sha.h>

#include <ctime>
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <string>
#include <filesystem>

#include "Path_QZA.h"

using namespace crow;
using namespace std;
using namespace zdb;

void sha256_string(const char *str, size_t size, char outputBuffer[65])
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    string salt(CRYPTO_SALT);
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str, size);
    SHA256_Update(&sha256, salt.data(), salt.size());
    SHA256_Final(hash, &sha256);
    int i = 0;
    for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;
}

string time2str(time_t timeT) {
    using namespace std;
    tm timepoint{};
    localtime_r(&timeT, &timepoint);
    char aaa[64];
    strftime(aaa, sizeof(aaa), "%Y:%m:%d#%H:%M:%S", &timepoint);

    return {aaa};
}

string crypt_str(const string &s) {
    char out[65];
    sha256_string(s.data(), s.size(), out);
    return {out, 64};
}

// thanks to https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
std::string gen_random(const int len) {
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return tmp_s;
}

crow::response login_route(const crow::request &req) {
    crow::json::rvalue body_json = json::load(req.body);
    string username(body_json["username"].s()), password(body_json["password"].s());
    UserInfo userinfo = find_user(username);
    if (userinfo.name.empty() or userinfo.pass_enc.empty() or userinfo.pass_enc != crypt_str(password)) {
        return {401, "you are not a user, but hello!"};
    }

    json::wvalue res_json;
    string token = gen_random(TOKEN_LEN);
    {
        std::lock_guard<std::mutex> lockGuard(token_store_lock);
        token_store[token] = NameExpire(username, time(NULL) + TOKEN_TIMEOUT_DEFAULT);
    }
    res_json["token"] = token;

    return res_json;
}

crow::response create_user_route(const crow::request &req) {
    NameExpire name_expire = check_token(req);
    if (name_expire.name != "qza2468") {
        return {401, "you are not qza2468, you don't have the priority"};
    }

    crow::json::rvalue body_json = json::load(req.body);
    string username(body_json["username"].s()), password(body_json["password"].s());
    if (username.empty() or password.empty()) {
        return {404, "username and password should not be empty"};
    }

    try {
        zdb::Connection con(SQLpool->getConnection());
        con.execute("INSERT INTO " USERS_SQL_TABLE_NAME " (username, password) VALUES(?, ?);", username.c_str(),
                    crypt_str(password).c_str());
    } catch (const zdb::sql_exception &exception) {
        return {404, exception.what()};
    }

    return {200, "ok"};
}

crow::response alter_user_route(const crow::request &req) {
    NameExpire name_expire = check_token(req);
    if (name_expire.name != "qza2468") {
        return {401, "you are not qza2468, you don't have the priority"};
    }

    crow::json::rvalue body_json = json::load(req.body);
    string username(body_json["username"].s()), password(body_json["password"].s());
    if (username.empty()) {
        return {404, "username should not be empty"};
    }

    if (password.empty()) {
        try {
            zdb::Connection con(SQLpool->getConnection());
            con.execute("DELETE FROM " USERS_SQL_TABLE_NAME " where username = ?", username.c_str());
        } catch (const zdb::sql_exception &exception) {
            return {404, exception.what()};
        }
    } else {
        try {
            zdb::Connection con(SQLpool->getConnection());
            con.execute("UPDATE " USERS_SQL_TABLE_NAME " SET password = ? where username = ?",
                        crypt_str(password).c_str(), username.c_str());
        } catch (const zdb::sql_exception &exception) {
            return {404, exception.what()};
        }
    }

    return {200, "ok"};
}

crow::response logout_route(const crow::request &req) {
    bool superUser = false;
    auto name_expire = check_token(req);
    if (name_expire.name.empty()) {
        return {401, "you are not a user, but hello!"};
    }

    if (name_expire.name == "qza2468") {
        superUser = true;
    }


    json::rvalue body_json = json::load(req.body);
    auto tokens_to_delete_tmp = body_json["tokens_to_delete"];
    auto tokens_to_delete = tokens_to_delete_tmp.lo();

    {
        lock_guard<std::mutex> LockGuard(token_store_lock);
        for (auto & i : tokens_to_delete) {
            auto iter = token_store.find(i.s());
            if (iter == token_store.end()) {
                continue;
            }

            if (iter->second.name == name_expire.name or superUser) {
                token_store.erase(i.s());
            }
        }
    }

    return {200, "ok"};
}

crow::response list_tokens_route(const crow::request &req) {
    bool all = false;
    auto user_expired = check_token(req);
    if (user_expired.name.empty()) {
        return {401, "you are not a user, but hello!"};
    }

    {
        auto iter = req.headers.find("all");
        if (iter != req.headers.end() and user_expired.name == "qza2468") {
            all = true;
        }
    }

    json::wvalue res_json;
    vector<json::wvalue> res_json_list;
    {
        lock_guard<std::mutex> lcokGuard(token_store_lock);
        for (const auto& i: token_store) {
            if (i.second.name == user_expired.name and not all) {
                res_json_list.emplace_back(vector<json::wvalue>({i.first, time2str(i.second.expire_time)}));
            } else if (all) {
                res_json_list.emplace_back(vector<json::wvalue>({i.first, time2str(i.second.expire_time), i.second.name}));
            }
        }
    }

    res_json["tokens"] = json::wvalue(res_json_list);

    return res_json;
}

crow::response list_users_route(const crow::request &req) {
    NameExpire name_expire = check_token(req);
    if (name_expire.name != "qza2468") {
        return {401, "you are not qza2468, you don't have the priority"};
    }

    vector<json::wvalue> list_json;
    json::wvalue res_json;
    try {
        zdb::Connection con(SQLpool->getConnection());
        auto res = con.executeQuery("SELECT * FROM " USERS_SQL_TABLE_NAME);
        while (res.next()) {
            list_json.emplace_back(res.getString("username"));
        }
    } catch (const sql_exception &e) {
        return {404, e.what()};
    }

    res_json["users"] = json::wvalue(list_json);

    return res_json;
}

// receive an token, return the name of the user. return empty str if not found.
NameExpire check_token(const string &token) {
    NameExpire name_expire;
    {
        std::lock_guard<std::mutex> lockGuard(token_store_lock);
        auto iter = token_store.find(token);
        // if not found token
        if (iter != token_store.end()) {
            name_expire = iter->second;
        } else {
            return {};
        }
    }

    // name is empty or token expired
    if (name_expire.name.empty() or difftime(name_expire.expire_time, time(NULL)) < 0) {
        lock_guard lockGuard(token_store_lock);
        token_store.erase(token);

        return {};
    }

    return name_expire;
}

NameExpire check_token(const crow::request &req) {
    auto iter = req.headers.find("token");
    if (iter == req.headers.end()) {
        return {};
    }
    return check_token(iter->second);
}

UserInfo find_user(const string &username) {
    string username_, password_;
    try {
        zdb::Connection con(SQLpool->getConnection());

        auto res = con.executeQuery("SELECT * FROM " USERS_SQL_TABLE_NAME " WHERE username = ?", username);

        if (not res.next()) {
            return {};
        }
        username_ = res.getString("username");
        password_ = res.getString("password");
    } catch (const sql_exception &e) {
        return {};
    }
    return {username_, password_};
}

void login_pre_run() {
    CROW_LOG_INFO << "creating directories for the users";
    Path_QZA basedir(BASE_DIR);

    auto con = SQLpool->getConnection();
    auto res = con.executeQuery("SELECT * FROM " USERS_SQL_TABLE_NAME);

    while (res.next()) {
        auto name = res.getString("username");
        Path_QZA user_dir = basedir;
        user_dir += Path_QZA(name);
        filesystem::create_directory(user_dir.to_str());
    }

    CROW_LOG_INFO << "directories created ok";
}

