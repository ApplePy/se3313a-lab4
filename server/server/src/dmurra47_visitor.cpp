#include "../include/dmurra47_visitor.hpp"

using namespace dmurra47;

dmurra47_visitor::dmurra47_visitor(se3313::networking::flex_waiter* monitor)
{
  flexer = monitor;
}


void dmurra47_visitor::onSTDIN(const std::string& line)
{
  std::cout << "onSTDIN called. Contents: " << std::endl;	// TODO: Replace with proper contents
  std::cout << line << std::endl;

}

void dmurra47_visitor::onSocket(const se3313::networking::flex_waiter::socket_ptr_t )
{
  std::cout << "Client socket did something." << std::endl;	// TODO: Replace with proper contents

}

void dmurra47_visitor::onSocketServer(const std::shared_ptr<se3313::networking::socket_server> serverSocket)
{
  // New client connection
  auto newClient = serverSocket->accept();
  
  // TODO: process new client
  
  // Save client to flex_waiter
  flexer->addSocket(newClient);

}
