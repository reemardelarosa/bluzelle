#ifndef BLUZELLE_DAEMON_H
#define BLUZELLE_DAEMON_H

#include "DaemonInfo.h"

#include <string>
#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

class Daemon
{
    DaemonInfo *daemon_info_;
public:
    Daemon()
    {
        daemon_info_ = &DaemonInfo::get_instance();
        boost::uuids::basic_random_generator<boost::mt19937> gen;
        const boost::uuids::uuid node_id{gen()};
        daemon_info_->set_value(
            "node_id",
            boost::lexical_cast<std::string>(node_id)
        );
    }

    ~Daemon() = default;

    std::string node_id()
    {
        return DaemonInfo::get_instance().get_value<std::string>("node_id");
    }

    std::string ethereum_address()
    {
        return DaemonInfo::get_instance().get_value<std::string>("ethereum_address");
    }

    uint16_t get_port()
    {
        return DaemonInfo::get_instance().get_value<uint16_t>("port");
    }

    uint16_t get_token_balance()
    {
        return 0;
    }

    std::string get_info()
    {
        std::stringstream ss;
        ss << "Running node with ID: "
           << node_id() << "\n"
           << " Ethereum Address ID: "
           << ethereum_address() << "\n"
           << "             on port: " << get_port() << "\n"
           << "       Token Balance: " << get_token_balance() << " BLZ\n";
        return ss.str();
    }


};


#endif //BLUZELLE_DAEMON_H
