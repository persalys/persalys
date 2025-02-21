//                                               -*- C++ -*-
/**
 *  @brief Class to define the limit states
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#ifndef PERSALYS_LIMITSTATE_HXX
#define PERSALYS_LIMITSTATE_HXX

#include "LimitStateImplementation.hxx"

#include <openturns/Less.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API LimitState : public OT::TypedInterfaceObject<LimitStateImplementation>
{
  CLASSNAME

public:
  typedef OT::Pointer<LimitStateImplementation>       Implementation;

  /** Default constructor */
  LimitState();
  /** Constructor with parameters */
  LimitState(const OT::String & name, const PhysicalModel & physicalModel,
             const OT::String & outputName = "",
             const OT::ComparisonOperator & failure = OT::Less(),
             const double & threshold = 0.);
  /** Default constructor */
  LimitState(const LimitStateImplementation & implementation);
  /** Constructor from implementation */
  LimitState(const Implementation & p_implementation);
  /** Constructor from implementation pointer */
  LimitState(LimitStateImplementation * p_implementation);

  /** Comparison operator */
  OT::Bool operator ==(const LimitState & other) const;
  OT::Bool operator !=(const LimitState & other) const;

  void addObserver(Observer * observer);
  void blockNotification(const OT::String & blockedObserverType = "");

  PhysicalModel getPhysicalModel() const;

  OT::String getOutputName() const;
  void setOutputName(const OT::String & outputName);

  OT::ComparisonOperator getOperator() const;
  void setOperator(const OT::ComparisonOperator & comparisonOperator);

  double getThreshold() const;
  void setThreshold(const double & threshold);

  bool isValid() const;

  OT::String getPythonScript() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);
};
}
#endif
