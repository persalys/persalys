// Observable.hxx

#ifndef OTGUI_OBSERVABLE_H
#define OTGUI_OBSERVABLE_H

#include <string>
#include <vector>

#include "Observer.hxx"

namespace OTGUI {
class Observable
{
public:
  Observable();

  virtual ~Observable() {};

  void addObserver(Observer * observer);
  void notify(std::string message);
  std::vector<Observer *> getObserver() const;
  void setObserver(const std::vector<Observer *> observer);

private:
  std::vector<Observer *> observers_;
};
}
#endif