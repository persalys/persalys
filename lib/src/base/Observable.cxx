//                                               -*- C++ -*-
/**
 *  @brief Class defining object observable by the interface
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "persalys/Observable.hxx"

#include <algorithm>

using namespace OT;

namespace PERSALYS
{

/* Default constructor */
Observable::Observable()
  : observers_()
  , blockedObserverType_("")
{
}


/* Copy constructor */
Observable::Observable(const Observable & /*other*/)
  : observers_()
  , blockedObserverType_("")
{
}


Observable::~Observable()
{
}


/* Assingment operator */
Observable & Observable::operator=(const Observable & rhs)
{
  if (this != &rhs)
  {
    observers_ = rhs.observers_;
    blockedObserverType_ = rhs.blockedObserverType_;
  }
  return *this;
}


void Observable::addObserver(Observer * observer)
{
  observers_.push_back(observer);
}


void Observable::removeObserver(Observer * observer)
{
  if (observer)
    observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
}


void Observable::removeObserver(const String& observerType)
{
  removeObserver(getObserver(observerType));
}


void Observable::notify(const String & message)
{
  // do not use for (std::vector<Observer*>::iterator it = observers_.begin(); it != observers_.end(); ++it)
  // avoid problem if an observer is added in the list observers_ in the update function
  for (int i = 0; i < (int)observers_.size(); ++i)
  {
    if (observers_[i]->getType() != blockedObserverType_)
    {
      observers_[i]->update(this, message);
    }
  }
}


void Observable::notifyAndRemove(const String & type)
{
  for (std::vector<Observer*>::iterator it = observers_.begin(); it != observers_.end(); ++it)
  {
    if ((*it)->getType() == type)
    {
      (*it)->update(this, "objectRemoved");
      observers_.erase(it);
      return;
    }
  }
}


void Observable::blockNotification(const String& blockedObserverType)
{
  // check observer type
  if (!blockedObserverType.empty())
  {
    bool found = false;
    for (int i = 0; i < (int)observers_.size(); ++i)
    {
      if (observers_[i]->getType() == blockedObserverType)
      {
        found = true;
        break;
      }
    }
    if (!found)
      throw InvalidArgumentException(HERE) << "In Observable::blockNotification : observer type not found " << blockedObserverType;
  }
  blockedObserverType_ = blockedObserverType;
}


std::vector< Observer* > Observable::getObservers() const
{
  return observers_;
}


Observer* Observable::getObserver(const String & type) const
{
  for (std::vector<Observer*>::const_iterator it = observers_.begin(); it != observers_.end(); ++it)
  {
    if ((*it)->getType() == type)
      return (*it);
  }
  return 0;
}
}
