#include "CommandLineOptions.h"
#include "Listener.h"
#include "Node.h"
#include "TokenBalance.hpp"
#include "WebSocketServer.h"

#include <boost/uuid/uuid_generators.hpp>
#include <boost/asio.hpp>
#include <iostream>


constexpr int s_uint_minimum_required_token_balance = 100;
constexpr char s_etherscan_api_token_envar_name[] = "ETHERSCAN_IO_API_TOKEN";


int
parse_command_line(
    int argc,
    char *argv[]
)
{
    CommandLineOptions options;

    if (!options.parse(argc, argv))
        {
        std::cout << options.get_last_error() << std::endl;
        return 1;
        }

    string address = options.get_address();
    if (!CommandLineOptions::is_valid_ethereum_address(address))
        {
        std::cout << address << " is not a valid Ethereum address." << std::endl;
        return 1;
        }
    DaemonInfo::get_instance().set_value("ethereum_address", address);

    uint port = options.get_port();
    if (!CommandLineOptions::is_valid_port(port))
        {
        std::cout << port
                  << " is not a valid port. Please pick a port in 49152 - 65535 range"
                  << std::endl;
        return 1;
        }
    DaemonInfo::get_instance().set_value("port", port);
    DaemonInfo::get_instance().set_value("name", "Node_running_on_port_" + std::to_string(port));
    return 0;
}


void
initialize_daemon()
{
    // TODO: Mehdi, put your config file work here.
    // If we don't have a node id in the config file, then generate one here
    // Mehdi, for now we just generate a new  node id everytime we restart the node,
    // but we should really check the config file first.
    boost::uuids::basic_random_generator<boost::mt19937> gen;
    const boost::uuids::uuid node_id{gen()};
    DaemonInfo::get_instance().set_value("node_id", boost::lexical_cast<std::string>(node_id));
}


int
check_token_balance()
{
    auto token = getenv(s_etherscan_api_token_envar_name);
    if (token == nullptr)
        {
        std::cout << "Environment variable " << s_etherscan_api_token_envar_name
                  << " containing etherscan.io API token must be set" << std::endl;
        return 1;
        }

    DaemonInfo &daemon_info = DaemonInfo::get_instance();

    uint64_t balance = get_token_balance(daemon_info.get_value<std::string>("ethereum_address"), token);
    if (balance < s_uint_minimum_required_token_balance)
        {
        std::cout << "Insufficient BZN token balance. The balance of "
                  << s_uint_minimum_required_token_balance << "BZN required to run node. Your balance is "
                  << balance << "BZN" << std::endl;
        return 1;
        }
    daemon_info.set_value("ropsten_token_balance", balance);
    return 0;
}


void
validate_node(
    int argc,
    char* argv[])
{
    if (0 != parse_command_line(argc, argv))
        {
        exit(-1);
        }

    if (0 != check_token_balance())
        {
        exit(-1);
        }
}


void
display_daemon_info()
{
    DaemonInfo &daemon_info = DaemonInfo::get_instance();
    auto port = daemon_info.get_value<unsigned short>("port");

    std::cout << "Running node with ID: " << daemon_info.get_value<std::string>("node_id") << "\n"
              << " Ethereum Address ID: " << daemon_info.get_value<std::string>("ethereum_address") << "\n"
              << "             on port: " << port << "\n"
              << "       Token Balance: " << daemon_info.get_value<unsigned short>("ropsten_token_balance") << " BLZ\n"
              << std::endl;
}


const std::string local_ip_address()
{
    std::string host_name = boost::asio::ip::host_name();
    std::cout << "host name: [" <<  host_name << "]" << std::endl;
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::resolver::query query(host_name, "");
    boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);

    boost::asio::ip::tcp::endpoint endpoint = it->endpoint();

    tcp::resolver::iterator end;

    std::cout << "\n";

    while(end != it)
        {
        std::cout << "[" << it->endpoint() << "]\n";
        ++it;
        }

    std::cout << std::endl;




    //std::cout << "ip: [" << endpoint.address().to_string() << "]" << std::endl;


    return endpoint.address().to_string();
}



void start_websocket_server()
{
    std::shared_ptr<Listener> listener;
    boost::thread websocket_thread
        (
            WebSocketServer
                (
                    local_ip_address().c_str(),
                    DaemonInfo::get_instance().get_value<unsigned short>("port") + 1000,
                    listener,
                    1
                )
        );
}


void start_node()
{
    boost::asio::io_service ios; // I/O service to use.
    boost::thread_group tg;
    Node this_node(ios);
    try
        {
        this_node.run();
        for (unsigned i = 0; i < boost::thread::hardware_concurrency(); ++i)
            {
            tg.create_thread(boost::bind(&boost::asio::io_service::run, &ios));
            }
        ios.run();
        }
    catch (std::exception &ex)
        {
        std::cout << ex.what() << std::endl;
        exit(1);
        }
}




int
main(int argc,
     char *argv[])
{
    // TODO Move this out of main
    initialize_daemon();
    validate_node(argc, argv);
    display_daemon_info();
    start_websocket_server();
    start_node();
    return 0;
}
