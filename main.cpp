#include <tuple>
#include "crow.h"

#include "login.h"
#include "config_qza.h"
#include "file.h"


zdb::ConnectionPool *SQLpool;
std::map<std::string, NameExpire> token_store;
std::mutex token_store_lock;

crow::response hello() {
    crow::json::wvalue aaa = {{"falskdj", "lfjdas"}};
    aaa["aaa"] = {{"fjla", "flajfalk"}};
    return aaa;
}


int main() {
    crow::SimpleApp app;

    SQLpool = new zdb::ConnectionPool(DEBUG_SQL_PATH);
    SQLpool->start();

    CROW_ROUTE(app, "/")(hello);
    CROW_ROUTE(app, "/api/login").methods(crow::HTTPMethod::POST)(login_route);
    CROW_ROUTE(app, "/api/createuser").methods(crow::HTTPMethod::POST)(create_user_route);
    CROW_ROUTE(app, "/api/logout").methods(crow::HTTPMethod::POST)(logout_route);
    CROW_ROUTE(app, "/api/listTokens").methods(crow::HTTPMethod::GET)(list_tokens_route);
    CROW_ROUTE(app, "/api/alterUser").methods(crow::HTTPMethod::POST)(alter_user_route);
    CROW_ROUTE(app, "/api/listUsers").methods(crow::HTTPMethod::GET)(list_users_route);

    CROW_ROUTE(app, "/api/upload").methods(crow::HTTPMethod::POST)(create_file_route);
    CROW_ROUTE(app, "/api/delete").methods(crow::HTTPMethod::POST)(unlink_route);
    CROW_ROUTE(app, "/api/rename").methods(crow::HTTPMethod::POST)(rename_file_route);
    CROW_ROUTE(app, "/api/download").methods(crow::HTTPMethod::POST)(download_file_route);
    CROW_ROUTE(app, "/api/listdir").methods(crow::HTTPMethod::GET)(list_dir_route);


    app.port(18888).multithreaded().run();
}