#include "db/db_conn.h"

#include <iostream>

namespace db {

std::string db_connection_params::to_string() const {
    return "dbname=" + dbname + " user=" + user + " password=" + password +
           " hostaddr=" + hostaddr + " port=" + port + " target_session_attrs=read-write";
}

Database::Database(const db_connection_params& db_info_) : db_info(db_info_), conn(nullptr) {}

Database::~Database() {
    if (conn) {
        if (conn->is_open()) {
            conn->close();
        }
        delete conn;
    }
}

void Database::connect() {
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

void Database::disconnect() {
    try {
        if (conn && conn->is_open()) {
            std::cout << "Disconnecting from database..." << std::endl;
            conn->close();
        }
    } catch (const std::exception& e) {
        std::cerr << "Database disconnection error: " << e.what() << std::endl;
    }
}

void Database::execute_query(const std::string& query) {
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

}  // namespace db
