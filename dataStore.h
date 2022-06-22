//
// Created by qza2468 on 22-6-22.
//

#ifndef UNTITLED8_DATASTORE_H
#define UNTITLED8_DATASTORE_H

#include "crow.h"
#include "login.h"
#include <string>

crow::response store_value_route(const crow::request &req);
crow::response get_info_route(const crow::request &req);
crow::response get_value_route(const crow::request &req);


#endif //UNTITLED8_DATASTORE_H
