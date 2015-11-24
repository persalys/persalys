// LimitState.cxx

#include "otgui/LimitState.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(LimitState);

LimitState::LimitState(const std::string & name, const PhysicalModel & physicalModel,
                       const std::string & outputName, const ComparisonOperator & comparisonOperator,
                       const double & threshold)
  : PersistentObject()
  , name_(name)
  , physicalModel_(physicalModel)
  , output_(physicalModel.getOutputByName(outputName))
  , comparisonOperator_(comparisonOperator)
  , threshold_(threshold)
{
  if (output_.getName() != outputName)
    throw InvalidArgumentException(HERE) << "The given output name " << outputName <<" does not correspond to an output of the given physical model\n";
}


LimitState::LimitState(const LimitState & other)
  : PersistentObject()
  , name_(other.name_)
  , physicalModel_(other.physicalModel_)
  , output_(other.output_)
  , comparisonOperator_(other.comparisonOperator_)
  , threshold_(other.threshold_)
{
}


LimitState * LimitState::clone() const
{
  return new LimitState(*this);
}


LimitState::~LimitState()
{

}


std::string LimitState::getName() const
{
  return name_;
}


PhysicalModel LimitState::getPhysicalModel() const
{
  return physicalModel_;
}


// build event
Event LimitState::getEvent()
{
  OutputCollection output;
  output.add(output_);
  Event event(physicalModel_.getOutputRandomVector(output), comparisonOperator_, threshold_);
  Description outputName(1);
  outputName[0] = output_.getName();
  event.setDescription(outputName);
  return event;
}


std::string LimitState::dump() const
{
  std::string result;
  result += getName()+ " = otguibase.LimitState('" + getName() + "', " + physicalModel_.getName();
  result += ", '" + output_.getName() + "', ot."+ comparisonOperator_.getImplementation()->getClassName() + "(), ";
  OSS oss;
  oss << threshold_;
  result += oss.str() + ")\n";
  return result;
}
}