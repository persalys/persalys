// Output.cxx

#include "otgui/Output.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(Output);

Output::Output(const String & name, const double & value, const String & description,
               const String & formula)
  : Variable(name, value, description)
  , formula_(formula)
  , hasBeenComputed_(false)
{
}


Output* Output::clone() const
{
  return new Output(*this);
}


String Output::getFormula() const
{
  return formula_;
}


void Output::setFormula(const String & formula)
{
  formula_ = formula;
}


bool Output::hasBeenComputed() const
{
  return hasBeenComputed_;
}


void Output::setHasBeenComputed(const bool hasBeenComputed)
{
  hasBeenComputed_ = hasBeenComputed;
}


String Output::dump() const
{
  OSS oss;
  oss << getName() << " = otguibase.Output('" << getName() << "', " << getValue() << ", '" << getDescription();
  oss << "', '" << formula_ << "')\n";

  return oss;
}
}