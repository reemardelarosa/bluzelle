#include <iostream>
#include <boost/lexical_cast.hpp>

#include "Node.h"
#include "DaemonInfo.h"

const std::string local_ip_address();


Node::Node(boost::asio::io_service& ios )
        : raft_(ios),
          server_ (ios,
                   local_ip_address().c_str(),
                   DaemonInfo::get_instance().get_value<unsigned short>("port"),
                   std::bind (&Raft::handle_request,
                              &raft_,
                              std::placeholders::_1))
{

}

void Node::run()
{
    raft_.run(); // Start RAFT.
    server_.run(); // Start accepting connections.
}

std::vector<NodeInfo> Node::get_peers() const
{
    std::vector<NodeInfo> v;
    // [todo] populate nodes vector.
    return v;
}

std::vector<MessageInfo> Node::get_messages() const
{
    std::vector<MessageInfo> v;
    // [todo] populate messages vector.
    return v;
}