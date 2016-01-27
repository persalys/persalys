// Output.hxx

#ifndef OUTPUT_H
#define OUTPUT_H

#include "Variable.hxx"
#include "Collection.hxx"

namespace OTGUI {
class Output : public Variable
{
  CLASSNAME;

public:
  Output(const OT::String & name="", const double & value=0., const OT::String & description="",
         const OT::String & formula="");

  virtual Output * clone() const;

  OT::String getFormula() const;
  void setFormula(const OT::String & formula);
 
  OT::String dump() const; 

private:
  OT::String formula_;
};
typedef OT::Collection<Output> OutputCollection;
}
#endif