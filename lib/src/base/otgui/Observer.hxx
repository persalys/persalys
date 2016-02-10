//                                               -*- C++ -*-
/**
 *  @brief Class to observe the base objects and give messages to the interface
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
#ifndef OTGUI_OBSERVER_HXX
#define OTGUI_OBSERVER_HXX

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