// CORSHandler.hpp
#ifndef CORS_HANDLER_H
#define CORS_HANDLER_H

#include "crow.h"

struct CORS {
    void before_handle(crow::request& req, crow::response& res, crow::context& /*ctx*/) {
        // Allow all origins
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    }

    void after_handle(crow::request& /*req*/, crow::response& res, crow::context& /*ctx*/) {
        // Add headers after the response is processed
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    }
};

#endif // CORS_HANDLER_H