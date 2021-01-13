//                                               -*- C++ -*-
/**
 *  @brief Class to observe the base objects and give messages to the interface
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#ifndef PERSALYS_OBSERVER_HXX
#define PERSALYS_OBSERVER_HXX

#include "persalys/PersalysPrivate.hxx"

#include <openturns/OTtypes.hxx>

namespace PERSALYS
{
class Observable;
class PhysicalModel;
class Analysis;
class LimitState;
class DesignOfExperiment;

class PERSALYS_BASE_API Observer
{
public:
  /** Default constructor */
  Observer(const OT::String & type = "");

  virtual ~Observer() {};

  OT::String getType() const;
  virtual void update(Observable * source, const OT::String & message) = 0;
  virtual void appendItem(const PhysicalModel &/*model*/) {};
  virtual void appendItem(const Analysis &/*analysis*/) {};
  virtual void appendItem(const LimitState &/*limitState*/) {};
  virtual void appendItem(const DesignOfExperiment &/*doe*/) {};

private:
  OT::String type_;
};
}
#endif
