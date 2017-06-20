#ifndef __MYAPPLICATIONEXCEPTION_HXX__
#define __MYAPPLICATIONEXCEPTION_HXX__

#include <exception>
#include <string>

#include "otgui/OTGuiprivate.hxx"

class OTGUI_API MyApplicationException : public std::exception
{
public:
  MyApplicationException(const char *msg);
  MyApplicationException(const std::string& msg);
  const char *what() const throw();
  ~MyApplicationException() throw() { }
private:
  std::string _msg;
};

#endif
