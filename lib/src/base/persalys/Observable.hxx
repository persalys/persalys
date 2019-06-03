//                                               -*- C++ -*-
/**
 *  @brief Class defining object observable by the interface
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#ifndef PERSALYS_OBSERVABLE_HXX
#define PERSALYS_OBSERVABLE_HXX

#include "Observer.hxx"
#include "AppliException.hxx"

#include <vector>

namespace PERSALYS
{
class PERSALYS_API Observable
{
public:
  /** Default constructor */
  Observable();
  /** Copy constructor */
  Observable(const Observable & other);

  virtual ~Observable() {};

  void addObserver(Observer * observer);
  void removeObserver(Observer * observer);
  void removeObserver(const OT::String & observerType);
  void notify(const OT::String & message);
  void notifyAndRemove(const OT::String & message, const OT::String & type);
  void blockNotification(const OT::String & blockedObserverType = "");
  std::vector<Observer *> getObservers() const;
  Observer * getObserver(const OT::String & type);

private:
  std::vector<Observer *> observers_;
  OT::String blockedObserverType_;
};
}
#endif
