// Output.cxx

#include "otgui/Output.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(Output);

Output::Output(const std::string & name, const double & value, const std::string & description,
               const std::string & formula)
  : Variable(name, value, description)
  , formula_(formula)
{
}


Output* Output::clone() const
{
  return new Output(*this);
}


std::string Output::getFormula() const
{
  return formula_;
}


void Output::setFormula(const std::string & formula)
{
  formula_ = formula;
}


std::string Output::dump() const
{
  OSS oss;
  oss << getName() << " = otguibase.Output('" << getName() << "', " << getValue() << ", '" << getDescription();
  oss << "', '" << formula_ << "')\n";

  return oss;
}
}