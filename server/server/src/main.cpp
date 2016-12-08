#include <string>
#include <iostream>
#include "server.hpp"

typedef se3313::networking::port_t port_t;

int main(int /*argc*/, char** /*argv*/)
{
  // Get port info
  std::cout << "dmurra47" << std::endl;
  std::cout << "What port would you like to connect to?";
  
  // TODO: Handle invalid input
  port_t port;
  std::cin >> port;
  
  // Create and start server
  std::shared_ptr<dmurra47::server> srv = std::make_shared<dmurra47::server>(port);
  srv->start();
  
  //Start console for "exit" command
  std::string inputText;
  do {
    std::cout << "> ";
    std::cin >> inputText;
  } while (inputText != "exit");
  
  // Stop server and exit.
  srv->stop();
  return 0;
}
