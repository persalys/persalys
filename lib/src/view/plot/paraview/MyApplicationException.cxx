#include "otgui/MyApplicationException.hxx"

MyApplicationException::MyApplicationException(const char *msg):_msg(msg)
{
}

MyApplicationException::MyApplicationException(const std::string& msg):_msg(msg)
{
}

const char *MyApplicationException::what() const throw()
{
  return _msg.c_str();
}
