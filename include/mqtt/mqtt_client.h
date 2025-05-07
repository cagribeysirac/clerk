// Copyright 2025 Cagribey

#ifndef INCLUDE_MQTT_CLIENT_H_
#define INCLUDE_MQTT_CLIENT_H_

#include <chrono>
#include <csignal>
#include <iostream>
#include <string>

#include "db_conn.h"
#include "mqtt/client.h"

namespace mqtt {

class MqttClient : public mqtt::callback {
 private:
    mqtt::async_client* async_client;
    mqtt::connect_options conn_opts;
    mqtt::message_ptr pubmsg;
    mqtt::delivery_token_ptr token;
    const std::string BROKER_ADDRESS;
    const std::string CLIENT_ID;

    db::Database db;                    // Database object
    volatile sig_atomic_t* stop_flag_;  // Pointer to stop flag

 public:
    MqttClient(const std::string& broker_address, const std::string& client_id,
               const db::db_connection_params& db_conn_info, volatile sig_atomic_t* stop_flag);
    ~MqttClient();

    void connect();
    void disconnect();
    void subscribe(const std::string& topic, int qos = 0);
    void publish(const std::string& topic, const std::string& payload, int qos = 0,
                 bool retained = false);
    void loop();

    // Callback methods
    void connected(const std::string& cause) override;
    void connection_lost(const std::string& cause) override;
    void message_arrived(mqtt::const_message_ptr msg) override;
    void delivery_complete([[maybe_unused]] mqtt::delivery_token_ptr token) override;
};

}  // namespace mqtt

#endif  // INCLUDE_MQTT_CLIENT_H_
