
#include <iostream>
#include <string>
#include <vector>

#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <networking/socket.hpp>
#include <networking/socket_server.hpp>

#include <msg/visitor.hpp>
#include <msg/error.hpp>
#include <msg/login.hpp>
#include <msg/json.hpp>

#include "server.hpp"

using namespace jsmith2;


namespace msg = se3313::msg;
namespace net = se3313::networking;

namespace pt  = boost::property_tree;

server::~server()
{
    std::cout << "Stopping the server.";
    
}

void server::start()
{
    std::cout << "Starting server on port: " << _serverPort << std::endl;
    
}

void server::stop()
{
    
}
