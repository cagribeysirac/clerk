// Copyright 2025 Cagribey

#include <iostream>

#include "mqtt_client.h"

namespace {  // Use anonymous namespace instead of global variables
std::string MQTT_BROKER_ADDRESS = "mqtt://127.0.0.1:1883";
std::string MQTT_TOPIC = "local/sim/#";
}  // namespace

void print_help() {
    // Print help message
    std::cout << "Usage: Clerk [options]\n"
              << "Options:\n"
              << "  -h, --help       Show this help message\n"
              << "  -b, --broker     MQTT broker address (default: mqtt://127.0.0.1:1883)\n"
              << "  -t, --topic      MQTT topic to subscribe to (default: local/sim/#)\n";
}

std::map<std::string, std::string> parse_args(int argc, const char* const argv[]) {
    // Get arguments from command line
    std::map<std::string, std::string> args;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            print_help();
            exit(0);
        } else if (arg == "-b" || arg == "--broker") {
            if (i + 1 < argc) {
                args["broker"] = argv[++i];
            }
        } else if (arg == "-t" || arg == "--topic") {
            if (i + 1 < argc) {
                args["topic"] = argv[++i];
            }
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            print_help();
            exit(1);
        }
    }
    return args;
}

int main(int argc, const char* const argv[]) {
    // Say hello :)
    std::cout << "Hello, I'm Clerk!" << std::endl;

    // Parse command line arguments
    auto args = parse_args(argc, argv);

    // Update values based on command line arguments
    if (args.find("broker") != args.end()) {
        MQTT_BROKER_ADDRESS = args["broker"];
    }
    if (args.find("topic") != args.end()) {
        MQTT_TOPIC = args["topic"];
    }

    // Create and connect the MQTT client
    MqttClient mqtt_client(MQTT_BROKER_ADDRESS, "clerk");
    mqtt_client.connect();
    mqtt_client.subscribe(MQTT_TOPIC);
    mqtt_client.loop();
    return 0;
}
