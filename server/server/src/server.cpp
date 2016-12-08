
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

#include <chrono>
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
    new std::thread(&server::sendMessages, this);						// Start message queue processing
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
  std::cout << "Client socket did something." << std::endl;

  // Get contents
  std::vector<char> contents;
  int bytes_read = socket->read(&contents);
  
  // If socket closed
  if(bytes_read == 0)
  {
    // TODO: Deregister username
    
    // Find socket in client socket array that closed; remove it and break
    for (auto iter = sockets.begin(); iter != sockets.end(); iter++)
    {
      if (socket->fd() == (*iter)->fd())
      {
	waiter->removeSocket(socket);
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
    return_t response = visit(parsedMsg);
    
    // Print return message
    boost::property_tree::write_json(std::cout, response->toJson(), true);	// DEBUG
    
    
    // Get type of request and type of message HAX HAX HAX
    msg::response::error* x = dynamic_cast<msg::response::error*>(response.get());
    
    
    // If error, send message back to socket
    if (x != nullptr)
    {
      // Convert message to string
      const std::string responseSerialized = msg::json::to(response->toJson());
      
      // Respond with bad username
      socket->write(responseSerialized);
    }
    // If no error, send message to everyone
    else
    {
      std::lock_guard<std::mutex> lock(messageQueueLock);
      messageQueue.push_back(response);
    }
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

void server::sendMessages()
{
  std::cout << "sendMessages start" << std::endl;
  while(!killed)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));	// Slow this thread down a bit to stop the cout pollution
    
    std::lock_guard<std::mutex> lock(messageQueueLock);
    
    // If pending messages to send
    if (messageQueue.size() > 0)
    {
      // Send all queued messages to all connected sockets
      for(auto message : messageQueue)
      {
	// Convert message to string
	const std::string responseSerialized = se3313::msg::json::to(message->toJson());
	
	// Write message to all sockets
	for (auto socket: sockets)
	{
	  socket->write(responseSerialized);
	}
      }
      
      // Clear queue
      messageQueue.clear();
    }
  }
}


 /// Called when a login message is passed
server::return_t server::visitLogin(const msg::request::login& login/* request */ )
{
  // Check to make sure username is not in use
  bool not_in_use = true;
  
  for (auto username : usernames)
  {
    if (login.sender() == username)
    {
      not_in_use = false;
      break;
    }
  }
  
  // If username not in use, add it to the list and notify everyone
  if(not_in_use)
  {
    usernames.push_back(login.sender());
    return std::shared_ptr<msg::response::login>(new msg::response::login(login.sender()));
  }
  else
  {
    // Return an error
    return std::shared_ptr<msg::response::error>(
      new msg::response::error(std::chrono::system_clock::now(),
			       "@server",
			       "@server",
			       msg::ErrorCode::USER_NAME_IN_USE,
			       "Username in use."));
  }
}

/// Called when a message is passed
server::return_t server::visitMessage(const msg::request::message& msg/* request */ )
{
  return std::shared_ptr<msg::response::message>(new msg::response::message(msg.sender(), msg.content()));
}
