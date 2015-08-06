// Input.cxx

#include "Input.hxx"
#include "DiracFactory.hxx"

namespace OTGUI {

Input::Input()
 : Variable()
 , distribution_(OT::Dirac())
{
}


Input::Input(const std::string & name, const double & value, const std::string & description,
             const OT::Distribution & distribution)
 : Variable(name, value, description)
 , distribution_(distribution)
{
}


Input::Input(const Input & other)
 : Variable(other)
 , distribution_(other.distribution_)
{

}


Input* Input::clone() const
{
  return new Input(*this);
}


Input::~Input()
{
}


OT::Distribution Input::getDistribution() const
{
  return distribution_;
}


void Input::setDistribution(const OT::Distribution & distribution)
{
  distribution_ = distribution;
}


std::string Input::dump() const
{
  std::string result;
  OT::OSS oss;

  std::string className = distribution_.getImplementation()->getClassName();
  if ( className != "Dirac")
  {
    oss << "dist_" << getName() << " = ot." << className << "(";
    OT::NumericalPointWithDescription parameters = distribution_.getParametersCollection()[0];
    for (int i=0; i<parameters.getSize(); ++i)
      oss << parameters[i] << ",";

    oss << ")\n";
  }
    
  oss << getName() << " = otguibase.Input(\"" << getName() << "\", " <<getValue() << ", \"" << getDescription();
  if (className != "Dirac")
    oss << "\", dist_" << getName() << ")\n";
  else
    oss << "\")\n";

  result += oss.str();

  return result;
}
}