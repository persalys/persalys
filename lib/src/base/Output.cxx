// Output.cxx

#include "Output.hxx"


namespace OTGUI {
CLASSNAMEINIT(Output);

Output::Output()
 : Variable()
 , formula_("")
{
}


Output::Output(const std::string & name, const double & value, const std::string & description,
               const std::string & formula)
 : Variable(name, value, description)
 , formula_(formula)
{
}


Output::Output(const Output & other)
 : Variable(other)
 , formula_(other.formula_)
{
}


Output* Output::clone() const
{
  return new Output(*this);
}


Output::~Output()
{
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
  std::string result;
  OT::OSS oss;

  oss << getName() << " = otguibase.Output('" << getName() << "', " <<getValue() << ", '" << getDescription();
  oss << "', '" << formula_ << "')\n";

  result += oss.str();

  return result;
}

}