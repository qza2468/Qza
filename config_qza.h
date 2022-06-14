//
// Created by qza2468 on 22-6-12.
//

#ifndef UNTITLED8_CONFIG_QZA_H
#define UNTITLED8_CONFIG_QZA_H

#include <tuple>
#include <string>
#include <map>
#include <thread>
#include <mutex>
#include <zdb/zdbpp.h>
#include "login.h"

#define SQL_PATH "mysql://qza2468:2468802225@localhost:3306/mysql"
#define DEBUG_SQL_PATH "sqlite:///home/qza2468/CLionProjects/untitled8/aaa.db"
#define USERS_SQL_TABLE_NAME "user_wow"
#define CRYPTO_SALT "2468802225"
#define TOKEN_LEN 64
#define BASE_DIR /home/qza2468/test/

#define TOKEN_TIMEOUT_DEFAULT 48 * 60 * 60


extern zdb::ConnectionPool *SQLpool;
extern std::map<std::string, NameExpire> token_store;
extern std::mutex token_store_lock;
#endif //UNTITLED8_CONFIG_QZA_H
