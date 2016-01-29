// Observer.hxx

#ifndef OTGUI_OBSERVER_H
#define OTGUI_OBSERVER_H

#include "OTtypes.hxx"

namespace OTGUI {
class Observable;

class Observer
{
public:
  Observer();

  virtual ~Observer() {};

  virtual void update(Observable * source, const OT::String & message) = 0;
};
}
#endif