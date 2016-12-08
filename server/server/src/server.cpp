
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

using namespace dmurra47;


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
    
    server_socket = std::shared_ptr<net::socket_server>(new net::socket_server(_serverPort));	// Create server socket
    
    waiter = std::shared_ptr<flex_waiter>(new flex_waiter(server_socket));			// Take server socket and put in flexWaiter
    listen();
}

void server::stop()
{
    killed = true;	// Signal end of program
    
    // Close all sockets
    for (auto sock : sockets) {
      sock->close();
    }
    sockets.empty();
    server_socket->close();
    
    //waiter->kill();	// Stop the listen loop
}

void server::listen()
{
  try
  {
    while(!killed)
    {
      int timeout = 1; // One second
      waiter->wait(this->shared_from_this(), std::chrono::milliseconds(std::chrono::seconds(timeout)));
    }
  }
  catch (std::runtime_error err)
  {
  }
}

void server::onSTDIN(const std::string& line)
{
  std::cout << "onSTDIN called. Contents: ";
  std::cout << line << std::endl;
  
  // Ignore unless we're stopping
  if (line == "exit")
    stop();
}

void server::onSocket(const se3313::networking::flex_waiter::socket_ptr_t socket)
{
  std::cout << "Client socket did something." << std::endl;	// TODO: Replace with proper contents

  // Get contents
  std::vector<char> contents;
  int bytes_read = socket->read(&contents);
  
  // If socket closed
  if(bytes_read == 0)
  {
    // Find socket in client socket array that closed; remove it and break
    for (auto iter = sockets.begin(); iter != sockets.end(); iter++)
    {
      if (socket->fd() == (*iter)->fd())
      {
	sockets.erase(iter);
	break;
      }
    }
  }
  else
  {
    // Construct message as string
    const std::string msg(contents.begin(), contents.end());
    
    // Parse message
    boost::property_tree::ptree parsedMsg = msg::json::from(msg);
    
    // Visit message
    visit(parsedMsg);
  }
  
}

void server::onSocketServer(const std::shared_ptr<se3313::networking::socket_server> serverSocket)
{
  // New client connection
  auto newClient = serverSocket->accept();
  
  // Save new client
  sockets.push_back(newClient);
  
  // Save client to flex_waiter
  waiter->addSocket(newClient);

}


