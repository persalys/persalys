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
#include "persalys/LimitStateImplementation.hxx"
#include "persalys/BaseTools.hxx"

#include <openturns/Equal.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/ThresholdEvent.hxx>
#include <openturns/IntersectionEvent.hxx>
#include <openturns/UnionEvent.hxx>
#include <openturns/CompositeRandomVector.hxx>
#include <openturns/SymbolicFunction.hxx>
#include <openturns/ComposedFunction.hxx>
#include <openturns/Greater.hxx>
#include <openturns/MemoizeFunction.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(LimitStateImplementation)

const static Factory<LimitStateImplementation> Factory_LimitStateImplementation;

/* Default constructor */
LimitStateImplementation::LimitStateImplementation()
  : PersistentObject()
  , Observable()
  , thresholds_(1)
{
}


/* Constructor for regular limit state */
LimitStateImplementation::LimitStateImplementation(const String& name,
    const PhysicalModel& physicalModel,
    const String& outputName,
    const ComparisonOperator& comparisonOperator,
    const double& threshold)
  : PersistentObject()
  , Observable()
  , physicalModel_(physicalModel)
  , thresholds_(1, threshold)
{
  if (!physicalModel.getOutputDimension())
    throw InvalidArgumentException(HERE) << "The physical model does not contain output";

  setName(name);
  if (outputName.empty())
    setOutputNames(Description(1, physicalModel.getOutputs()[0].getName()));
  else
    setOutputNames(Description(1, outputName));

  setOperators(Collection<ComparisonOperator>(1, comparisonOperator));
}

/* Constructor for system limit state */
LimitStateImplementation::LimitStateImplementation(const String& name,
    const PhysicalModel& physicalModel,
    const Description& outputNames,
    const Collection<ComparisonOperator>& operators,
    const Type type,
    const Point& thresholds)
  : PersistentObject()
  , Observable()
  , physicalModel_(physicalModel)
  , thresholds_(thresholds)
  , type_(type)
{
  if (outputNames.getSize() != operators.getSize() || outputNames.getSize() != thresholds.getDimension())
    throw InvalidArgumentException(HERE) << "The size of outputNames, operators and thresholds must be the same.";
  
  if (physicalModel.getOutputDimension() < outputNames.getSize())
    throw InvalidArgumentException(HERE) << "The physical model does not contain enough outputs.";

  setName(name);
  setOutputNames(outputNames);
  setOperators(operators);
}


/* Virtual constructor */
LimitStateImplementation* LimitStateImplementation::clone() const
{
  return new LimitStateImplementation(*this);
}


void LimitStateImplementation::removeAllObservers()
{
  getPhysicalModel().getImplementation()->removeObserver(getObserver("LimitStateItem"));
  notifyAndRemove("LimitStateItem");
  notifyAndRemove("Study");
}


Bool LimitStateImplementation::operator==(const LimitStateImplementation& other) const
{
  if (this == &other)
    return true;
  return false;
}


Bool LimitStateImplementation::operator!=(const LimitStateImplementation& other) const
{
  return !operator==(other);
}


PhysicalModel LimitStateImplementation::getPhysicalModel() const
{
  return physicalModel_;
}


OT::Description LimitStateImplementation::getOutputNames() const
{
  return outputNames_;
}


void LimitStateImplementation::setOutputNames(const OT::Description& outputNames)
{
  for (const auto& outputName : outputNames)
  {
    if (!physicalModel_.hasOutputNamed(outputName))
      throw InvalidArgumentException(HERE) << "The physical model does not contain an output named '" << outputName << "'.";
  }
  outputNames_ = outputNames;
  notify("outputNameChanged");
}

void LimitStateImplementation::setOutputName(const UnsignedInteger index, const String& outputName)
{
  if (index >= outputNames_.getSize())
    throw InvalidArgumentException(HERE) << "Index out of range.";
  if (!physicalModel_.hasOutputNamed(outputName))
    throw InvalidArgumentException(HERE) << "The physical model does not contain an output named '" << outputName << "'.";
  outputNames_[index] = outputName;
  notify("outputNameChanged");
}


Collection<ComparisonOperator> LimitStateImplementation::getOperators() const
{
  return operators_;
}

ComparisonOperator LimitStateImplementation::getOperator(const UnsignedInteger index) const
{
  if (index >= operators_.getSize())
    throw InvalidArgumentException(HERE) << "Index out of range.";
  return operators_[index];
}


void LimitStateImplementation::setOperators(const OT::Collection<OT::ComparisonOperator>& operators)
{
  for (const auto& comparisonOperator : operators)
  {
    if (comparisonOperator.getImplementation()->getClassName() == "Equal")
      throw InvalidArgumentException(HERE) << "The operator Equal is not valid to define a limit state.\n";
  }
  operators_ = operators;
  notify("operatorChanged");
}

void LimitStateImplementation::setOperator(const UnsignedInteger index, const OT::ComparisonOperator& comparisonOperator)
{
  if (index >= operators_.getSize())
    throw InvalidArgumentException(HERE) << "Index out of range.";
  if (comparisonOperator.getImplementation()->getClassName() == "Equal")
    throw InvalidArgumentException(HERE) << "The operator Equal is not valid to define a limit state.\n";
  operators_[index] = comparisonOperator;
  notify("operatorChanged");
}

Point LimitStateImplementation::getThresholds() const
{
  return thresholds_;
}

double LimitStateImplementation::getThreshold(const UnsignedInteger index) const
{
  if (index >= thresholds_.getDimension())
    throw InvalidArgumentException(HERE) << "Index out of range.";
  return thresholds_[index];
}

void LimitStateImplementation::setThresholds(const OT::Point& thresholds)
{
  thresholds_ = thresholds;
  notify("thresholdChanged");
}

void LimitStateImplementation::setThreshold(const UnsignedInteger index, const double& threshold)
{
  if (index >= thresholds_.getDimension())
    throw InvalidArgumentException(HERE) << "Index out of range.";
  thresholds_[index] = threshold;
  notify("thresholdChanged");
}

void LimitStateImplementation::setType(const Type type)
{
  type_ = type;
  notify("typeChanged");
}

LimitStateImplementation::Type LimitStateImplementation::getType() const
{
  return type_;
}

void LimitStateImplementation::addFailureEvent(const String& variableName, const ComparisonOperator& comparisonOperator, const double& threshold)
{
  if (!physicalModel_.hasOutputNamed(variableName))
    throw InvalidArgumentException(HERE) << "The physical model does not contain an output named '" << variableName << "'.";
  
  outputNames_.add(variableName);
  operators_.add(comparisonOperator);
  thresholds_.add(threshold);
}

void LimitStateImplementation::removeFailureEvent(const UnsignedInteger index)
{
  if (outputNames_.getSize() <= 1)
    throw InvalidArgumentException(HERE) << "A limit state must contain at least one failure event.";
  if (index >= outputNames_.getSize())
    throw InvalidArgumentException(HERE) << "Index out of range.";

  outputNames_.erase(outputNames_.begin() + index);
  operators_.erase(operators_.begin() + index);
  thresholds_.erase(thresholds_.begin() + index);
}

bool LimitStateImplementation::isSystemLimitState() const
{
  return outputNames_.getSize() > 1;
}

UnsignedInteger LimitStateImplementation::getNumberOfFailureEvents() const
{
  return outputNames_.getSize();
}

bool LimitStateImplementation::isValid() const
{
  for (const auto& outputName : outputNames_)
  {
    if (!physicalModel_.hasOutputNamed(outputName))
      return false;
  }

  return true;
}

RandomVector LimitStateImplementation::getThresholdEvent(OT::Collection<OT::Function> &functions) const
{
  RandomVector event;

  if (isSystemLimitState())
  {
    // The input random vector must be shared across all events so that
    // UnionEvent/IntersectionEvent can verify they have the same antecedent.
    const RandomVector inputRandomVector = getPhysicalModel().getInputRandomVector();
    Collection<RandomVector> eventsCollection(outputNames_.getSize());
    for (UnsignedInteger i = 0; i < outputNames_.getSize(); ++i)
    {
      Description outputName(1, outputNames_[i]);
      Function fun{getPhysicalModel().getRestrictedFunction(outputName)};
      MemoizeFunction function(fun);
      functions.add(function);
      eventsCollection[i] = ThresholdEvent(CompositeRandomVector(function, inputRandomVector), operators_[i], thresholds_[i]);
      eventsCollection[i].setDescription(outputName);
    }

    if (type_ == Type::Union)
      event = UnionEvent(eventsCollection);
    else
      event = IntersectionEvent(eventsCollection);
  }
  else
  {
    Function fun{getPhysicalModel().getRestrictedFunction(outputNames_)};
    MemoizeFunction function(fun);
    functions.add(function);
    event = ThresholdEvent(CompositeRandomVector(function, getPhysicalModel().getInputRandomVector()), operators_[0], thresholds_[0]);
    event.setDescription(outputNames_);
  }

  return event;
}

RandomVector LimitStateImplementation::getThresholdEvent() const
{
  Collection<Function> functions;
  return getThresholdEvent(functions);
}

RandomVector LimitStateImplementation::getISThresholdEvent() const
{
  if (!isSystemLimitState() || type_ != Type::Intersection)
    return getThresholdEvent();
  
  return getThresholdEvent().getImplementation()->asComposedEvent();
}

String LimitStateImplementation::getPythonScript() const
{
  OSS oss;

  oss << "outputNames = " << Parameters::GetOTDescriptionStr(outputNames_) << "\n";
  oss << "operators = [";
  for (UnsignedInteger i = 0; i < operators_.getSize(); ++i)
  {
    if (i > 0)
      oss << ", ";
    oss << "ot." << operators_[i].getImplementation()->getClassName() << "()";
  }
  oss << "]\n";

  oss << "thresholds = " << Parameters::GetOTPointStr(thresholds_, ", ") << "\n";

  oss << getName() << " = persalys.LimitState('" << getName() << "', " << getPhysicalModel().getName() << ", outputNames, operators,  " << (type_ == Type::Union ? "persalys.LimitState.Union" : "persalys.LimitState.Intersection") << ", thresholds)\n";

  return oss;
}


String LimitStateImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " physicalModel=" << getPhysicalModel().getName()
      << " outputNames=" << getOutputNames()
      << " operators=" << getOperators()
      << " thresholds=" << getThresholds()
      << " type=" << (type_ == Type::Union ? "Union" : "Intersection");
  return oss;
}


String LimitStateImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset;
  oss << "type=" << (type_ == Type::Union ? "Union" : "Intersection") << ", ";
  oss << "failure events: ";

  for (UnsignedInteger i = 0; i < outputNames_.getSize(); ++i)
  {
    if (i > 0)
      oss << ", ";
    
    oss << "[" << outputNames_[i] << " " << operators_[i].__str__() << " " << thresholds_[i] << "]";
  }

  return oss;
}


/* Method save() stores the object through the StorageManager */
void LimitStateImplementation::save(Advocate& adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("physicalModel_", physicalModel_);
  adv.saveAttribute("outputNames_", outputNames_);
  adv.saveAttribute("operators_", operators_);
  adv.saveAttribute("thresholds_", thresholds_);
  adv.saveAttribute("type_", static_cast<UnsignedInteger>(type_));
}


/* Method load() reloads the object from the StorageManager */
void LimitStateImplementation::load(Advocate& adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("physicalModel_", physicalModel_);
  if (adv.hasAttribute("outputNames_"))
  {
    // 20.1+
    adv.loadAttribute("outputNames_", outputNames_);
    adv.loadAttribute("operators_", operators_);
    adv.loadAttribute("thresholds_", thresholds_);
    UnsignedInteger typeInt;
    adv.loadAttribute("type_", typeInt);
    type_ = static_cast<Type>(typeInt);
  }
  else
  {
    // 20.0 and earlier
    String outputName;
    ComparisonOperator comparisonOperator;
    double threshold;
    adv.loadAttribute("outputName_", outputName);
    adv.loadAttribute("operator_", comparisonOperator);
    adv.loadAttribute("threshold_", threshold);
    setOutputNames(Description(1, outputName));
    setOperators(Collection<ComparisonOperator>(1, comparisonOperator));
    setThresholds(Point(1, threshold));
  }
}
}
