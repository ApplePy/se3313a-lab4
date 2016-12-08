#ifndef DMURRA47_VISITOR_H
#define DMURRA47_VISITOR_H

#include <iostream>
#include <../../home/vagrant/Desktop/se3313-lab4/server/lib/include/networking/flex_waiter.hpp>

namespace dmurra47
{

class dmurra47_visitor : public se3313::networking::flex_waiter::activity_visitor
{
public:
  dmurra47_visitor(se3313::networking::flex_waiter* monitor);
  virtual void onSTDIN(const std::string& line);
  virtual void onSocket(const se3313::networking::flex_waiter::socket_ptr_t );
  virtual void onSocketServer(const std::shared_ptr<se3313::networking::socket_server>);
  
private:
  se3313::networking::flex_waiter* flexer;
};

}

#endif // DMURRA47_VISITOR_H
