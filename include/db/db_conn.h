// Copyright 2025 Cagribey

#ifndef INCLUDE_DB_CONN_H_
#define INCLUDE_DB_CONN_H_

#include <iostream>
#include <pqxx/pqxx>
#include <string>

namespace db {                 // Use db namespace instead of global variables
struct db_connection_params {  // Use struct to hold database connection info
    std::string dbname = "postgres";
    std::string user = "postgres";
    std::string password = "password";
    std::string hostaddr = "127.0.0.1";
    std::string port = "5432";

    std::string to_string() const;
};

class Database {
 private:
    db::db_connection_params db_info;
    pqxx::connection* conn;  // Use pointer to manage connection object

    void execute_query(const std::string& query);

 public:
    Database(const db::db_connection_params& db_info_);
    ~Database();
    void connect();
    void disconnect();
};

}  // namespace db

#endif  // INCLUDE_DB_CONN_H_
