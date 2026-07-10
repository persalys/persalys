//                                               -*- C++ -*-
/**
 *  @brief Class to define the limit states
 *
 *  Copyright 2015-2026 EDF-Phimeca
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

namespace PERSALYS
{
class PERSALYS_BASE_API LimitState : public OT::TypedInterfaceObject<LimitStateImplementation>
{
  CLASSNAME

public:
  using Implementation = OT::Pointer<LimitStateImplementation>;
  using Type = LimitStateImplementation::Type;
  using ComparisonOperatorCollection = LimitStateImplementation::ComparisonOperatorCollection;

  /** Default constructor */
  LimitState();

  /** Constructor for regular limit state */
  LimitState( const OT::String & name, 
              const PhysicalModel & physicalModel,
              const OT::String & outputName = "",
              const OT::ComparisonOperator & failure = OT::Less(),
              const double & threshold = 0.);
  
  /** Constructor for system limit state */
  LimitState( const OT::String & name, 
              const PhysicalModel & physicalModel,
              const OT::Description & outputNames,
              const ComparisonOperatorCollection & operators, 
              const OT::Point & thresholds,
              const Type type = Type::Union);
  
  /** Default constructor */
  LimitState(const LimitStateImplementation & implementation);
  /** Constructor from implementation */
  LimitState(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  LimitState(LimitStateImplementation * p_implementation);
#endif

  /** Comparison operator */
  OT::Bool operator ==(const LimitState & other) const;
  OT::Bool operator !=(const LimitState & other) const;

  void addObserver(Observer * observer);
  void blockNotification(const OT::String & blockedObserverType = "");

  PhysicalModel getPhysicalModel() const;

  OT::Description getOutputNames() const;
  void setOutputNames(const OT::Description & outputNames);
  void setOutputName(OT::UnsignedInteger index, const OT::String & outputName);

  ComparisonOperatorCollection getOperators() const;
  OT::ComparisonOperator getOperator(OT::UnsignedInteger index) const;
  void setOperators(const ComparisonOperatorCollection & operators);
  void setOperator(OT::UnsignedInteger index, const OT::ComparisonOperator & comparisonOperator);

  OT::Point getThresholds() const;
  double getThreshold(OT::UnsignedInteger index) const;
  void setThresholds(const OT::Point & thresholds);
  void setThreshold(OT::UnsignedInteger index, const double & threshold);

  void setType(const Type type);
  Type getType() const;

  bool isSystemLimitState() const;
  OT::UnsignedInteger getNumberOfFailureEvents() const;

  bool isValid() const;

  void addFailureEvent( const OT::String & variableName,
                        const OT::ComparisonOperator & comparisonOperator = OT::Less(),
                        const double & threshold = 0.0 );
  void removeFailureEvent(OT::UnsignedInteger index);

  #ifndef SWIG
  /** Get the threshold event corresponding to the limit state
   * 
   * @param functions A collection in which will be added every function used to build 
   * the threshold event in the form of a MemoizedFunction.
   */
  OT::RandomVector getThresholdEvent(OT::Collection<OT::Function> &functions) const;
  #endif
  OT::RandomVector getThresholdEvent() const;

  /** Get the the threshold event constructed specifically for importance sampling */
  OT::RandomVector asComposedEvent() const;

  OT::String getPythonScript() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);
};
}
#endif
