//                                               -*- C++ -*-
/**
 *  @brief Class to define the limit states
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
#ifndef OTGUI_LIMITSTATE_HXX
#define OTGUI_LIMITSTATE_HXX

#include "LimitStateImplementation.hxx"

#include <Less.hxx>
#include <Event.hxx>

namespace OTGUI {
class OTGUI_API LimitState : public OT::TypedInterfaceObject<LimitStateImplementation>
{
  CLASSNAME;

public:
  typedef OT::Pointer<LimitStateImplementation>       Implementation;

  /** Default constructor */
  LimitState();
  /** Constructor with parameters */
  LimitState(const OT::String & name, const PhysicalModel & physicalModel,
             const OT::String & outputName="",
             const OT::ComparisonOperator & failure=OT::Less(),
             const double & threshold=0.);
  /** Default constructor */
  LimitState(const LimitStateImplementation & implementation);
  /** Constructor from implementation */
  LimitState(const Implementation & p_implementation);
  /** Constructor from implementation pointer */
  LimitState(LimitStateImplementation * p_implementation); 

  void addObserver(Observer * observer);
  void blockNotification(bool block, const OT::String & notBlockedMessage="");

  PhysicalModel getPhysicalModel() const;
  void setPhysicalModel(const PhysicalModel & physicalModel);

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