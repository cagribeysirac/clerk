// Copyright 2025 Cagribey

#include <iostream>

#include "mqtt_client.h"

namespace {  // Use anonymous namespace instead of global variables
std::string MQTT_BROKER_ADDRESS = "mqtt://127.0.0.1:1883";
std::string MQTT_TOPIC = "local/sim/#";
struct db_connection_info  // Use struct to hold database connection info
{
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

}  // namespace

// Print help message
void print_help() {
    std::cout << "Usage: Clerk [options]\n"
              << "Options:\n"
              << "  -h, --help       Show this help message\n"
              << "  -b, --broker     MQTT broker address (default: mqtt://127.0.0.1:1883)\n"
              << "  -t, --topic      MQTT topic to subscribe to (default: local/sim/#)\n"
              << "  -d, --dbname     Database name (default: postgres)\n"
              << "  -u, --dbuser     Database user (default: postgres)\n"
              << "  -W, --dbpassword Database password (default: password)\n"
              << "  -a, --dbhostaddr Database host address (default:127.0.0.1)\n"
              << "  -p, --dbport     Database port (default: 5432)\n";
}

// Get arguments from command line
std::map<std::string, std::string> parse_args(int argc, const char* const argv[]) {
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
        } else if (arg == "-d" || arg == "--dbname") {
            if (i + 1 < argc) {
                args["dbname"] = argv[++i];
            }
        } else if (arg == "-u" || arg == "--dbuser") {
            if (i + 1 < argc) {
                args["dbuser"] = argv[++i];
            }
        } else if (arg == "-W" || arg == "--dbpassword") {
            if (i + 1 < argc) {
                args["dbpassword"] = argv[++i];
            }
        } else if (arg == "-a" || arg == "--dbhostaddr") {
            if (i + 1 < argc) {
                args["dbhostaddr"] = argv[++i];
            }
        } else if (arg == "-p" || arg == "--dbport") {
            if (i + 1 < argc) {
                args["dbport"] = argv[++i];
            }
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            print_help();
            exit(1);
        }
    }
    return args;
}

volatile sig_atomic_t stop_signal = 0;

// Signal handler for SIGINT
void signal_handler(int signum) {
    std::cout << "Received signal " << signum << ", stopping...\n";
    stop_signal = 1;
}

int main(int argc, const char* const argv[]) {
    std::cout << "Hello, I'm Clerk!" << std::endl;

    // Initialize Database connection info
    db_connection_info db_connection_info;

    // Parse command line arguments
    auto args = parse_args(argc, argv);

    // Update values based on command line arguments
    if (args.find("broker") != args.end()) {
        MQTT_BROKER_ADDRESS = args["broker"];
    }
    if (args.find("topic") != args.end()) {
        MQTT_TOPIC = args["topic"];
    }
    if (args.find("dbname") != args.end()) {
        db_connection_info.dbname = args["dbname"];
    }
    if (args.find("dbuser") != args.end()) {
        db_connection_info.user = args["dbuser"];
    }
    if (args.find("dbpassword") != args.end()) {
        db_connection_info.password = args["dbpassword"];
    }
    if (args.find("dbhostaddr") != args.end()) {
        db_connection_info.hostaddr = args["dbhostaddr"];
    }
    if (args.find("dbport") != args.end()) {
        db_connection_info.port = args["dbport"];
    }
    std::signal(SIGINT, signal_handler);

    // Create and connect the MQTT client
    MqttClient mqtt_client(MQTT_BROKER_ADDRESS, "clerk", db_connection_info.to_string(),
                           &stop_signal);
    mqtt_client.connect();
    mqtt_client.subscribe(MQTT_TOPIC);
    mqtt_client.loop();
    return 0;
}
