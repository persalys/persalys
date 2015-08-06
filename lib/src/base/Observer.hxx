// Observer.hxx

#ifndef OTGUI_OBSERVER_H
#define OTGUI_OBSERVER_H

#include <string>

namespace OTGUI {

class Observable;  
class Observer
{
public:
  Observer();

  virtual ~Observer() {};

  virtual void update(Observable * source, const std::string & message) = 0;
};
}
#endif