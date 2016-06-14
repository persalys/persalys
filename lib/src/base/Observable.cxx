//                                               -*- C++ -*-
/**
 *  @brief Class defining object observable by the interface
 *
 *  Copyright 2015-2016 EDF-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/Observable.hxx"

#include <algorithm>

using namespace OT;

namespace OTGUI {

/* Default constructor */
Observable::Observable()
  : notificationBlocked_(false)
  , notBlockedMessage_("")
{
}


/* Copy constructor */
Observable::Observable(const Observable & other)
  : observers_()
  , notificationBlocked_(false)
  , notBlockedMessage_("")
{
}


void Observable::addObserver(Observer * observer)
{
  observers_.push_back(observer);
}


void Observable::removeObserver(Observer * observer)
{
  observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
}


void Observable::notify(const String & message)
{
  if (!notificationBlocked_ || (notificationBlocked_ && message == notBlockedMessage_))
  {
    std::vector<Observer*>::reverse_iterator rit = observers_.rbegin();
    for (; rit!= observers_.rend(); ++rit)
    {
      (*rit)->update(this, message);
    }
  }
}


void Observable::blockNotification(bool block, const String & notBlockedMessage)
{
  notificationBlocked_ = block;
  notBlockedMessage_ = notBlockedMessage;
}


std::vector< Observer* > Observable::getObservers() const
{
  return observers_;
}


void Observable::setObservers(const std::vector<Observer *> observer)
{
  observers_ = observer;
}
}