//                                               -*- C++ -*-
/**
 *  @brief Class to define inputs for the physical models
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_INPUT_HXX
#define PERSALYS_INPUT_HXX

#include "Variable.hxx"

#include <openturns/Distribution.hxx>

namespace PERSALYS
{
class PERSALYS_API Input : public Variable
{
  CLASSNAME

public:
  /** Default constructor */
  Input();
  /** Constructor with parameters */
  Input(const OT::String& name);
  /** Constructor with parameters */
  Input(const OT::String& name, const double& value, const OT::Distribution& distribution, const OT::String& description = "");
  /** Constructor with parameters */
  Input(const OT::String& name, const OT::Distribution& distribution, const OT::String& description = "");
  /** Constructor with parameters */
  Input(const OT::String& name, const double& value, const OT::String& description = "");

  /** Virtual constructor */
  virtual Input * clone() const;

  OT::Distribution getDistribution() const;
  void setDistribution(const OT::Distribution & distribution);

  OT::UnsignedInteger getDistributionParametersType() const;
  void setDistributionParametersType(const OT::UnsignedInteger & distributionParametersType);

  double getFiniteDifferenceStep() const;
  void setFiniteDifferenceStep(const double& step);

  bool isStochastic() const;
  void setStochastic(const bool stoch);

  OT::String getPythonScript() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  OT::String getDistributionPythonScript() const;

private:
  bool isStochastic_;
  OT::Distribution distribution_;
  OT::UnsignedInteger distributionParametersType_;
  double finiteDifferenceStep_;
};
typedef OT::Collection<Input> InputCollection;
}
#endif
