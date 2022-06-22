//
// Created by qza2468 on 22-6-22.
//
// a `BASE64` store service!!! only store no delete and modify, do things in frontend!
//

#include "dataStore.h"
#include "Path_QZA.h"
#include "file.h"

using namespace std;

crow::response store_value_route(const crow::request &req) {
    string username = check_token(req).name;
    if (username.empty()) {
        return {401, "you are not a user, but hello!"};
    }
    string comment = check_header(req, "comment-q");
    if (comment.empty() or (not Path_QZA::validateBase64(comment))) {
        return {404, "error base64 encoding"};
    }

    string value = req.body;
    if (value.empty() or (not Path_QZA::validateBase64(value))) {
        return {404, "error base64 encoding"};
    }

    auto conn = SQLpool->getConnection();
    conn.execute("INSERT INTO vvv(username, value_q, comment_q) VALUES(?, ?, ?)",
                 username.c_str(), value.c_str(), comment.c_str());

    return {200, "ok"};
}

crow::response get_info_route(const crow::request &req) {
    string username = check_token(req).name;
    if (username.empty()) {
        return {401, "you are not a user, but hello!"};
    }

    auto conn = SQLpool->getConnection();
    auto res = conn.executeQuery("SELECT comment_q FROM vvv WHERE username = ?", username.c_str());

    vector<crow::json::wvalue> l;
    while (res.next()) {
        l.emplace_back(res.getString("comment_q"));
    }

    return crow::json::wvalue(l);
}

crow::response get_value_route(const crow::request &req) {
    string username = check_token(req).name;
    if (username.empty()) {
        return {401, "you are not a user, but hello!"};
    }

    string comment = check_header(req, "comment-q");
    if (comment.empty() or (not Path_QZA::validateBase64(comment))) {
        return {404, "error base64 encoding"};
    }

    auto conn = SQLpool->getConnection();
    auto res = conn.executeQuery("SELECT * FROM vvv WHERE username = ? AND comment_q = ?",
                                 username.c_str(), comment);

    vector<crow::json::wvalue> l;
    while (res.next()) {
        crow::json::wvalue item;
        item["comment"] = res.getString("comment_q");
        item["username"] = res.getString("username");
        item["value"] = res.getString("value_q");

        l.push_back(item);
    }

    return crow::json::wvalue(l);
}
