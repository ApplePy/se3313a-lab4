
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
#include "dmurra47_visitor.hpp"

using namespace dmurra47;


namespace msg = se3313::msg;
namespace net = se3313::networking;

namespace pt  = boost::property_tree;

server::~server()
{
    std::cout << "Stopping the server.";
    listenThread->join();
}

void server::start()
{
    std::cout << "Starting server on port: " << _serverPort << std::endl;
    waiter = flex(net::socket_server(_serverPort));				// Create server socket and put in flexWaiter
    visitor = visit(&waiter);								// Setup visitor class
    listenThread = new std::thread(&server::listen, this);			// Start thread to listen
}

void server::stop()
{
    killed = true;	// Signal end of program
    waiter.kill();	// Stop the listen loop
}

void server::listen()
{
  try
  {
    while(!killed)
    {
      int timeout = 1; // One second
      waiter.wait(&visitor, std::chrono::duration_cast<std::chrono::seconds>(timeout));
    }
  }
  catch (std::runtime_error err)
  {
  }
}

//void server::onSo
