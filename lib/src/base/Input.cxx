//                                               -*- C++ -*-
/**
 *  @brief Class to define inputs for the physical models
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
#include "persalys/Input.hxx"

#include "persalys/DistributionDictionary.hxx"

#include <openturns/TruncatedDistribution.hxx>
#include <openturns/Dirac.hxx>
#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(Input)

static Factory<Input> Factory_Input;

/* Default constructor */
Input::Input()
  : Variable()
  , isStochastic_(false)
  , distribution_(Dirac())
  , distributionParametersType_(0)
  , finiteDifferenceStep_(ResourceMap::GetAsScalar("NonCenteredFiniteDifferenceGradient-DefaultEpsilon"))
{
}


/* Constructor with parameters */
Input::Input(const String& name)
  : Variable(name)
  , isStochastic_(false)
  , distribution_(Dirac())
  , distributionParametersType_(0)
  , finiteDifferenceStep_(ResourceMap::GetAsScalar("NonCenteredFiniteDifferenceGradient-DefaultEpsilon"))
{
}


/* Constructor with parameters */
Input::Input(const String& name, const double& value, const Distribution& distribution, const String& description)
  : Variable(name, value, description)
  , isStochastic_(false)
  , distribution_(Dirac())
  , distributionParametersType_(0)
  , finiteDifferenceStep_(ResourceMap::GetAsScalar("NonCenteredFiniteDifferenceGradient-DefaultEpsilon"))
{
  setDistribution(distribution);
}


/* Constructor with parameters */
Input::Input(const String& name, const Distribution& distribution, const String& description)
  : Variable(name, 0., description)
  , isStochastic_(false)
  , distribution_(Dirac())
  , distributionParametersType_(0)
  , finiteDifferenceStep_(ResourceMap::GetAsScalar("NonCenteredFiniteDifferenceGradient-DefaultEpsilon"))
{
  setDistribution(distribution);
}


/* Constructor with parameters */
Input::Input(const String& name, const double& value, const String& description)
  : Variable(name, value, description)
  , isStochastic_(false)
  , distribution_(Dirac())
  , distributionParametersType_(0)
  , finiteDifferenceStep_(ResourceMap::GetAsScalar("NonCenteredFiniteDifferenceGradient-DefaultEpsilon"))
{
}


/* Virtual constructor */
Input* Input::clone() const
{
  return new Input(*this);
}


Distribution Input::getDistribution() const
{
  return distribution_;
}


void Input::setDistribution(const Distribution & distribution)
{
  if (distribution.getDimension() > 1)
    throw InvalidDimensionException(HERE) << "The input distribution has to be of dimension 1. Here the dimension is " << distribution.getDimension();

  if (distribution.getImplementation()->getClassName() == "Dirac")
  {
    isStochastic_ = false;
    return;
  }
  isStochastic_ = true;
  if (distribution_.getName() != distribution.getName())
    distributionParametersType_ = 0;
  distribution_ = distribution;
}


UnsignedInteger Input::getDistributionParametersType() const
{
  return distributionParametersType_;
}


void Input::setDistributionParametersType(const UnsignedInteger & distributionParametersType)
{
  distributionParametersType_ = distributionParametersType;
}


double Input::getFiniteDifferenceStep() const
{
  return finiteDifferenceStep_;
}


void Input::setFiniteDifferenceStep(const double& step)
{
  finiteDifferenceStep_ = step;
}


bool Input::isStochastic() const
{
  return isStochastic_;
}


void Input::setStochastic(const bool stoch)
{
  isStochastic_ = stoch;
  if (isStochastic_ && distribution_.getImplementation()->getClassName() == "Dirac")
    distribution_ = DistributionDictionary::BuildDistribution("Normal", getValue());
}


String Input::getDistributionPythonScript() const
{
  OSS oss;

  String distributionName = distribution_.getImplementation()->getClassName();
  if (distributionName != "TruncatedDistribution")
  {
    oss << "dist_" << getName() << " = ot." << distributionName << "(";
    PointWithDescription parameters = distribution_.getParametersCollection()[0];
    for (unsigned int i = 0; i < parameters.getSize(); ++ i)
    {
      oss << parameters[i];
      if (i < parameters.getSize() - 1)
        oss << ", ";
    }
    oss << ")\n";
  }
  else
  {
    TruncatedDistribution truncatedDistribution = *dynamic_cast<TruncatedDistribution*>(distribution_.getImplementation().get());
    Distribution distribution = truncatedDistribution.getDistribution();
    oss << "dist_" << getName() << " = ot." << distribution.getImplementation()->getClassName() << "(";
    PointWithDescription parameters = distribution.getParametersCollection()[0];
    for (unsigned int i = 0; i < parameters.getSize(); ++ i)
    {
      oss << parameters[i];
      if (i < parameters.getSize() - 1)
        oss << ", ";
    }
    oss << ")\n";
    oss << "dist_" << getName() << " = ot." << distributionName << "(";
    oss << "dist_" << getName() << ", ";

    const Interval bounds(truncatedDistribution.getBounds());
    if (!(bounds.getFiniteLowerBound()[0] && bounds.getFiniteUpperBound()[0])) // one side truncation ?
    {
      if (bounds.getFiniteLowerBound()[0])    //lower bound truncation
        oss << bounds.getLowerBound()[0] << ")\n";
      else
        oss << bounds.getUpperBound()[0] << ", ot.TruncatedDistribution.UPPER)\n";
    }
    else  // both sides truncation
      oss << "ot.Interval(" << bounds.getLowerBound()[0] << ", " << bounds.getUpperBound()[0] << "))\n";
  }
  return oss;
}


String Input::getPythonScript() const
{
  OSS oss;
  oss.setPrecision(12);

  String pythonName(getName());
  std::replace(pythonName.begin(), pythonName.end(), '.', '_');

  if (!isStochastic())
    oss << pythonName << " = persalys.Input('" << getName() << "', " << getValue() << ", '" << getEscapedDescription() << "')\n";
  else
  {
    oss << getDistributionPythonScript();
    oss << pythonName << " = persalys.Input('" << getName() << "', " << getValue();
    oss << ", dist_" << pythonName << ", '" << getEscapedDescription() << "')\n";
  }
  if (getFiniteDifferenceStep() != ResourceMap::GetAsScalar("NonCenteredFiniteDifferenceGradient-DefaultEpsilon"))
    oss << pythonName << ".setFiniteDifferenceStep(" << getFiniteDifferenceStep() << ")\n";

  return oss;
}


/* String converter */
String Input::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " value=" << getValue()
      << " description=" << getDescription();
  if (isStochastic())
    oss << " distribution=" << getDistribution();
  oss << " finiteDifferenceStep=" << getFiniteDifferenceStep();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void Input::save(Advocate & adv) const
{
  Variable::save(adv);
  adv.saveAttribute("isStochastic_", isStochastic_);
  adv.saveAttribute("distribution_", distribution_);
  adv.saveAttribute("distributionParametersType_", distributionParametersType_);
  adv.saveAttribute("finiteDifferenceStep_", finiteDifferenceStep_);
}


/* Method load() reloads the object from the StorageManager */
void Input::load(Advocate & adv)
{
  Variable::load(adv);
  adv.loadAttribute("isStochastic_", isStochastic_);
  adv.loadAttribute("distribution_", distribution_);
  adv.loadAttribute("distributionParametersType_", distributionParametersType_);
  adv.loadAttribute("finiteDifferenceStep_", finiteDifferenceStep_);
}
}
