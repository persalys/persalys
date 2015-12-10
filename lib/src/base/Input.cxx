// Input.cxx

#include "otgui/Input.hxx"

#include "TruncatedDistribution.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(Input);

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

  std::string distributionName = distribution_.getImplementation()->getClassName();

  if (distributionName != "Dirac" && distributionName != "TruncatedDistribution")
  {
    oss << "dist_" << getName() << " = ot." << distributionName << "(";
    NumericalPointWithDescription parameters = distribution_.getParametersCollection()[0];
    for (unsigned int i = 0; i < parameters.getSize(); ++ i)
    {
      oss << parameters[i];
      if (i < parameters.getSize() - 1)
        oss << ", ";
    }
    oss << ")\n";
  }
  else if (distributionName == "TruncatedDistribution")
  {
    TruncatedDistribution truncatedDistribution = *dynamic_cast<TruncatedDistribution*>(&*distribution_.getImplementation());
    Distribution distribution = truncatedDistribution.getDistribution();
    oss << "dist_" << getName() << " = ot." << distribution.getImplementation()->getClassName() << "(";
    NumericalPointWithDescription parameters = distribution.getParametersCollection()[0];
    for (unsigned int i = 0; i < parameters.getSize(); ++ i)
    {
      oss << parameters[i];
      if (i < parameters.getSize() - 1)
        oss << ", ";
    }
    oss << ")\n";
    oss << "dist_" << getName() << " = ot." << distributionName << "(";
    oss << "dist_" << getName() << ", ";

    if (!(truncatedDistribution.getFiniteLowerBound() && truncatedDistribution.getFiniteUpperBound())) // one side truncation ?
    {
      if (truncatedDistribution.getFiniteLowerBound())    //lower bound truncation
        oss << truncatedDistribution.getLowerBound() << ")\n";
      else
        oss << truncatedDistribution.getUpperBound() << ", ot.TruncatedDistribution.UPPER)\n";
    }
    else  // both sides truncation
      oss << truncatedDistribution.getUpperBound() << ", " << truncatedDistribution.getUpperBound() <<")\n";
  }

  oss << getName() << " = otguibase.Input('" << getName() << "', " <<getValue() << ", '" << getDescription();
  if (distributionName != "Dirac")
    oss << "', dist_" << getName() << ")\n";
  else
    oss << "')\n";

  result += oss.str();

  return result;
}
}