#include "otgui/Observable.hxx"

using namespace OT;

namespace OTGUI {

Observable::Observable()
  : notificationBlocked_(false)
  , notBlockedMessage_("")
{
}

void Observable::addObserver(Observer * observer)
{
  observers_.push_back(observer);
}


void Observable::notify(const String & message)
{
  if (!notificationBlocked_ || (notificationBlocked_ && message == notBlockedMessage_))
    for (unsigned int i = 0; i < observers_.size(); ++ i)
      observers_[i]->update(this, message);
}


void Observable::blockNotification(bool block, const String & notBlockedMessage)
{
  notificationBlocked_ = block;
  notBlockedMessage_ = notBlockedMessage;
}


std::vector< Observer* > Observable::getObserver() const
{
  return observers_;
}


void Observable::setObserver(const std::vector<Observer *> observer)
{
  observers_ = observer;
}
}