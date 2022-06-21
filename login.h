//
// Created by qza2468 on 22-6-12.
//

#ifndef UNTITLED8_LOGIN_H
#define UNTITLED8_LOGIN_H

#include "crow.h"
#include <string>
#include <ctime>

class NameExpire {
public:
    std::string name;
    time_t expire_time{};
    NameExpire(const std::string &name_, time_t expire_time_): name(name_), expire_time(expire_time_) {};
    explicit NameExpire(const std::string &name_): name(name_), expire_time(time(NULL)) {};
    NameExpire() = default;
};

class UserInfo {
public:
    std::string name;
    std::string pass_enc;

    UserInfo(const std::string &name_, const std::string &pass_enc_): name(name_), pass_enc(pass_enc_) {};
    UserInfo() = default;
};

void login_pre_run();

crow::response login_route(const crow::request &req);
crow::response create_user_route(const crow::request &req);
crow::response logout_route(const crow::request &req);
crow::response list_tokens_route(const crow::request &req);
crow::response alter_user_route(const crow::request &req);
crow::response list_users_route(const crow::request &req);

NameExpire check_token(const std::string &token);
NameExpire check_token(const crow::request &req);
UserInfo find_user(const std::string &username);

#endif //UNTITLED8_LOGIN_H
