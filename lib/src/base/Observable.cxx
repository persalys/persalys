//                                               -*- C++ -*-
/**
 *  @brief Class defining object observable by the interface
 *
 *  Copyright 2015-2016 EDF
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