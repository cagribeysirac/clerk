#include "mqtt/mqtt_client.h"

namespace mqtt {

MqttClient::MqttClient(const std::string& broker_address, const std::string& client_id,
                       const db::db_connection_params& db_conn_info,
                       volatile sig_atomic_t* stop_flag)
    : BROKER_ADDRESS(broker_address),
      CLIENT_ID(client_id),
      db(db_conn_info),
      stop_flag_(stop_flag) {
    async_client = new mqtt::async_client(BROKER_ADDRESS, CLIENT_ID);
    async_client->set_callback(*this);
    conn_opts = mqtt::connect_options_builder().clean_session().finalize();
    conn_opts.set_keep_alive_interval(60);

    try {
        db.connect();  // Connect to the database
    } catch (const std::exception& e) {
        add_error(std::string(e.what()));
    }
}

MqttClient::~MqttClient() {
    if (async_client) {
        std::cout << "Stopping MQTT client..." << std::endl;
        async_client->stop_consuming();
        std::cout << "Disconnecting from MQTT broker..." << std::endl;
        try {
            async_client->disconnect()->wait();
        } catch (const mqtt::exception& exc) {
            std::cerr << "Error: " << exc.what() << std::endl;
        }
        delete async_client;
    }

    db.disconnect();  // Disconnect from the database
}

void MqttClient::connect() {
    try {
        async_client->connect(conn_opts)->wait();
    } catch (const mqtt::exception& exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
        add_error(std::string(exc.what()));
    }
}

void MqttClient::disconnect() {
    try {
        async_client->disconnect()->wait();
    } catch (const mqtt::exception& exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
    }
}

void MqttClient::subscribe(const std::string& topic, int qos) {
    try {
        std::cout << "Subscribing to topic: " << topic << std::endl;
        async_client->subscribe(topic, qos)->wait();
    } catch (const mqtt::exception& exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
        add_error(std::string(exc.what()));
    }
}

void MqttClient::publish(const std::string& topic, const std::string& payload, int qos,
                         bool retained) {
    try {
        auto pubmsg = mqtt::make_message(topic, payload);
        pubmsg->set_qos(qos);
        pubmsg->set_retained(retained);
        token = async_client->publish(pubmsg);

        token->wait_for(std::chrono::seconds(5));  // Add timeout to prevent hanging

        if (!token->is_complete()) {
            std::cerr << "Message delivery timeout" << std::endl;
        }
    } catch (const mqtt::exception& exc) {
        std::cerr << "Error in publish: " << exc.what() << std::endl;
    }
}

void MqttClient::loop() {
    try {
        while (!(*stop_flag_)) {
            async_client->start_consuming();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in loop: " << e.what() << std::endl;
    }
}

void MqttClient::connected(const std::string& cause) {
    std::cout << "Connected to broker: " << cause << std::endl;
}

void MqttClient::connection_lost(const std::string& cause) {
    std::cout << "Connection lost: " << cause << std::endl;
}

void MqttClient::message_arrived(mqtt::const_message_ptr msg) {
    std::cout << "Message arrived: " << msg->to_string() << std::endl;
}

void MqttClient::delivery_complete([[maybe_unused]] mqtt::delivery_token_ptr token) {
    std::cout << "Message delivered successfully" << std::endl;
}

// Error handling methods
void MqttClient::add_error(const std::string& error) {
    errors.push_back(error);
}

void MqttClient::clear_errors() {
    errors.clear();
}

void MqttClient::print_errors() const {
    std::cout << errors.size() << " errors occurred:\n";
    std::cout << "------------------------------\n";
    for (const auto& error : errors) {
        std::cerr << "Error: " << error << std::endl;
    }
    std::cout << "------------------------------\n";
}

const std::vector<std::string>& MqttClient::get_errors() {
    return errors;
}

}  // namespace mqtt
