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
#ifndef OTGUI_OBSERVABLE_HXX
#define OTGUI_OBSERVABLE_HXX

#include "Observer.hxx"
#include "OTguiException.hxx"

#include <vector>

namespace OTGUI {
class OTGUI_API Observable
{
public:
  /** Default constructor */
  Observable();

  virtual ~Observable() {};

  void addObserver(Observer * observer);
  void removeObserver(Observer * observer);
  void notify(const OT::String & message);
  std::vector<Observer *> getObservers() const;
  void setObservers(const std::vector<Observer *> observer);
  void blockNotification(bool block, const OT::String & notBlockedMessage="");

private:
  std::vector<Observer *> observers_;
  bool notificationBlocked_;
  OT::String notBlockedMessage_;
};
}
#endif