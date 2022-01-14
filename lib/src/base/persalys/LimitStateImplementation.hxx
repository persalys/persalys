//                                               -*- C++ -*-
/**
 *  @brief Class to define the limit states
 *
 *  Copyright 2015-2022 EDF-Phimeca
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

namespace PERSALYS
{
class PERSALYS_BASE_API LimitStateImplementation : public OT::PersistentObject, public Observable
{
  CLASSNAME

public:
  typedef OT::Pointer<LimitStateImplementation>       Implementation;

  /** Default constructor */
  LimitStateImplementation();
  /** Constructor with parameters */
  LimitStateImplementation(const OT::String & name, const PhysicalModel & physicalModel,
                           const OT::String & outputName,
                           const OT::ComparisonOperator & failure, const double & threshold);

  /** Virtual constructor */
  LimitStateImplementation * clone() const override;

  void removeAllObservers() override;

  /** Comparison operators */
  OT::Bool operator ==(const LimitStateImplementation & other) const;
  OT::Bool operator !=(const LimitStateImplementation & other) const;

  PhysicalModel getPhysicalModel() const;

  OT::String getOutputName() const;
  void setOutputName(const OT::String & outputName);

  OT::ComparisonOperator getOperator() const;
  void setOperator(const OT::ComparisonOperator & comparisonOperator);

  double getThreshold() const;
  void setThreshold(const double & threshold);

  bool isValid() const;

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
  OT::Function function_;
  OT::String outputName_;
  OT::ComparisonOperator operator_;
  double threshold_;
};
}
#endif
