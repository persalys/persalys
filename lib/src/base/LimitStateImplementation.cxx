// LimitStateImplementation.cxx

#include "otgui/LimitStateImplementation.hxx"

using namespace OT;

namespace OTGUI {

LimitStateImplementation::LimitStateImplementation(const std::string & name, const PhysicalModel & physicalModel,
                       const std::string & outputName, const ComparisonOperator & comparisonOperator,
                       const double & threshold)
  : PersistentObject()
  , Observable()
  , physicalModel_(physicalModel)
  , outputName_(outputName)
  , operator_(comparisonOperator)
  , threshold_(threshold)
{
  setName(name);
}


LimitStateImplementation* LimitStateImplementation::clone() const
{
  return new LimitStateImplementation(*this);
}


PhysicalModel LimitStateImplementation::getPhysicalModel() const
{
  return physicalModel_;
}


void LimitStateImplementation::setPhysicalModel(const PhysicalModel & physicalModel)
{
  physicalModel_ = physicalModel;
}


std::string LimitStateImplementation::getOutputName() const
{
  return outputName_;
}


void LimitStateImplementation::setOutputName(const std::string & outputName)
{
  outputName_ = outputName;
  notify("outputNameChanged");
}


ComparisonOperator LimitStateImplementation::getOperator() const
{
  return operator_;
}


void LimitStateImplementation::setOperator(const ComparisonOperator & comparisonOperator)
{
  operator_ = comparisonOperator;
  notify("operatorChanged");
}


double LimitStateImplementation::getThreshold() const
{
  return threshold_;
}


void LimitStateImplementation::setThreshold(const double & threshold)
{
  threshold_ = threshold;
  notify("thresholdChanged");
}


// build event
Event LimitStateImplementation::getEvent()
{
  Description outputDescription(1);
  outputDescription[0] = outputName_;
  Event event(getPhysicalModel().getOutputRandomVector(outputDescription), operator_, threshold_);
  Description outputName(1);
  outputName[0] = outputName_;
  event.setDescription(outputName);
  return event;
}


std::string LimitStateImplementation::dump() const
{
  std::string result;
  result += getName()+ " = otguibase.LimitState('" + getName() + "', " + getPhysicalModel().getName();
  result += ", '" + outputName_ + "', ot."+ operator_.getImplementation()->getClassName() + "(), ";
  OSS oss;
  oss << threshold_;
  result += oss.str() + ")\n";
  return result;
}
}