// Input.cxx

#include "otgui/Input.hxx"
#include "DiracFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(Input);

Input::Input()
 : Variable()
 , distribution_(Dirac())
{
}


Input::Input(const std::string & name, const double & value, const std::string & description,
             const Distribution & distribution)
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

String Input::__repr__() const
{
  OSS oss;
  oss << "class="<<GetClassName() <<" var="<<getName()<<" value="<<getValue()<<" desc="<< getDescription();
  return oss;
//   <<" var="<<getName()<<" value="<<getValue()<<" desc="<< getDistribution();
}

Distribution Input::getDistribution() const
{
  return distribution_;
}


void Input::setDistribution(const Distribution & distribution)
{
  distribution_ = distribution;
}


std::string Input::dump() const
{
  std::string result;
  OSS oss;

  std::string className = distribution_.getImplementation()->getClassName();
  if (className != "Dirac")
  {
    oss << "dist_" << getName() << " = ot." << className << "(";
    NumericalPointWithDescription parameters = distribution_.getParametersCollection()[0];
    for (unsigned int i = 0; i < parameters.getSize(); ++ i) {
      oss << parameters[i];
      if (i < parameters.getSize() - 1)
        oss << ", ";
    }
    oss << ")\n";
  }

  oss << getName() << " = otguibase.Input('" << getName() << "', " <<getValue() << ", '" << getDescription();
  if (className != "Dirac")
    oss << "', dist_" << getName() << ")\n";
  else
    oss << "')\n";

  result += oss.str();

  return result;
}
}