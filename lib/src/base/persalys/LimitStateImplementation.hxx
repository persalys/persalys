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
#ifndef PERSALYS_LIMITSTATEIMPLEMENTATION_HXX
#define PERSALYS_LIMITSTATEIMPLEMENTATION_HXX

#include "PhysicalModel.hxx"

#include <openturns/Less.hxx>
#include <openturns/RandomVector.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API LimitStateImplementation : public OT::PersistentObject, public Observable
{
  CLASSNAME

public:
  enum Type { Union, Intersection };

  using ComparisonOperatorCollection = OT::Collection<OT::ComparisonOperator>;

  /** Default constructor */
  LimitStateImplementation();

  /** Constructor for regular limit state */
  LimitStateImplementation(const OT::String & name, const PhysicalModel & physicalModel,
                           const OT::String & outputName,
                           const OT::ComparisonOperator & failure, const double & threshold);
  
  /** Constructor for system limit state */
  LimitStateImplementation( const OT::String & name, const PhysicalModel & physicalModel,
                            const OT::Description & outputNames,
                            const ComparisonOperatorCollection & operators,
                            const Type type, const OT::Point & thresholds );
  
  /** Virtual constructor */
  LimitStateImplementation * clone() const override;

  void removeAllObservers() override;

  /** Comparison operators */
  OT::Bool operator ==(const LimitStateImplementation & other) const;
  OT::Bool operator !=(const LimitStateImplementation & other) const;

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

  void addFailureEvent( const OT::String & variableName, 
                    const OT::ComparisonOperator & comparisonOperator = OT::Less(), 
                    const double & threshold = 0.0);
  void removeFailureEvent(OT::UnsignedInteger index);

  bool isSystemLimitState() const;
  OT::UnsignedInteger getNumberOfFailureEvents() const;

  bool isValid() const;

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
  OT::RandomVector getISThresholdEvent() const;

  OT::String getPythonScript() const;

  /** String converter */
  OT::String __repr__() const override;
  OT::String __str__(const OT::String & offset) const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

private:
  PhysicalModel physicalModel_;
  OT::Description outputNames_;
  OT::PersistentCollection<OT::ComparisonOperator> operators_;
  OT::Point thresholds_;
  Type type_ = Type::Union;
};
}

#endif
