// Observable.hxx

#ifndef OTGUI_OBSERVABLE_H
#define OTGUI_OBSERVABLE_H

#include "Observer.hxx"

#include <vector>

namespace OTGUI {
class Observable
{
public:
  Observable();

  virtual ~Observable() {};

  void addObserver(Observer * observer);
  void notify(const std::string & message);
  std::vector<Observer *> getObserver() const;
  void setObserver(const std::vector<Observer *> observer);
  void blockNotification(bool block, const std::string & notBlockedMessage="");

private:
  std::vector<Observer *> observers_;
  bool notificationBlocked_;
  std::string notBlockedMessage_;
};
}
#endif