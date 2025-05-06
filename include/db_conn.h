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

    std::string to_string() const {
        return "dbname=" + dbname + " user=" + user + " password=" + password +
               " hostaddr=" + hostaddr + " port=" + port + " target_session_attrs=read-write";
    };
};
}  // namespace db

class Database {
 private:
    db::db_connection_params db_info;
    pqxx::connection* conn;  // Use pointer to manage connection object

    void execute_query(const std::string& query) {
        try {
            if (!conn || !conn->is_open()) {
                throw std::runtime_error("Database connection is not open.");
            }

            pqxx::work txn(*conn);
            txn.exec(query);
            txn.commit();
        } catch (const std::exception& e) {
            std::cerr << "Query execution error: " << e.what() << std::endl;
        }
    }

 public:
    Database(const db::db_connection_params& db_info_) : db_info(db_info_), conn(nullptr) {}

    void connect() {
        try {
            if (conn && conn->is_open()) {
                std::cout << "Database connection is already open: " << conn->dbname() << std::endl;
                return;
            }

            if (conn) {
                // Delete the old connection if it exists
                delete conn;
            }

            // Create a new connection
            conn = new pqxx::connection(db_info.to_string());
            if (conn->is_open()) {
                std::cout << "Connected to database successfully: " << conn->dbname() << std::endl;
            } else {
                std::cerr << "Failed to connect to database" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Database connection error: " << e.what() << std::endl;
        }
    }

    void disconnect() {
        try {
            if (conn && conn->is_open()) {
                std::cout << "Disconnecting from database..." << std::endl;
                conn->close();
            }
        } catch (const std::exception& e) {
            std::cerr << "Database disconnection error: " << e.what() << std::endl;
        }
    }

    ~Database() {
        if (conn) {
            if (conn->is_open()) {
                conn->close();
            }
            delete conn;
        }
    }
};

#endif  // INCLUDE_DB_CONN_H_
