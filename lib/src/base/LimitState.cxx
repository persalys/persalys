// LimitState.cxx

#include "otgui/LimitState.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(LimitState);

LimitState::LimitState(const std::string & name, const PhysicalModel & physicalModel,
                       const std::string & outputName,
                       const OT::ComparisonOperator & comparisonOperator, const double & threshold)
  : TypedInterfaceObject<LimitStateImplementation>(new LimitStateImplementation(name, physicalModel, outputName, comparisonOperator, threshold))
{
}


/* Default constructor */
LimitState::LimitState(const LimitStateImplementation & implementation)
  : TypedInterfaceObject<LimitStateImplementation>(implementation.clone())
{
}


/* Constructor from implementation */
LimitState::LimitState(const Implementation & p_implementation)
  : TypedInterfaceObject<LimitStateImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}

/* Constructor from implementation pointer */
LimitState::LimitState(LimitStateImplementation * p_implementation)
  : TypedInterfaceObject<LimitStateImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}


LimitState::LimitState(const LimitState & other)
  : TypedInterfaceObject<LimitStateImplementation>(other.getImplementation())
{
}


LimitState * LimitState::clone() const
{
  return new LimitState(*this);
}


LimitState::~LimitState()
{

}


void LimitState::addObserver(Observer * observer)
{
  getImplementation()->addObserver(observer);
}


void LimitState::blockNotification(bool block, const std::string & notBlockedMessage)
{
  getImplementation()->blockNotification(block, notBlockedMessage);
}


std::string LimitState::getName() const
{
  return getImplementation()->getName();
}


void LimitState::setName(const std::string & name)
{
  getImplementation()->setName(name);
}


PhysicalModel LimitState::getPhysicalModel() const
{
  return getImplementation()->getPhysicalModel();
}


void LimitState::setPhysicalModel(const PhysicalModel & physicalModel)
{
  getImplementation()->setPhysicalModel(physicalModel);
}


std::string LimitState::getOutputName() const
{
  return getImplementation()->getOutputName();
}


void LimitState::setOutputName(const std::string & outputName)
{
  getImplementation()->setOutputName(outputName);
}


ComparisonOperator LimitState::getOperator() const
{
  return getImplementation()->getOperator();
}


void LimitState::setOperator(const ComparisonOperator & comparisonOperator)
{
  getImplementation()->setOperator(comparisonOperator);
}


double LimitState::getThreshold() const
{
  return getImplementation()->getThreshold();
}


void LimitState::setThreshold(const double & threshold)
{
  getImplementation()->setThreshold(threshold);
}


// build event
Event LimitState::getEvent()
{
  return getImplementation()->getEvent();
}


std::string LimitState::dump() const
{
  return getImplementation()->dump();
}
}