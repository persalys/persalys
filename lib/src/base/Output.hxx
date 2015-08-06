// Output.hxx

#ifndef OUTPUT_H
#define OUTPUT_H

#include "Variable.hxx"
#include "Collection.hxx"

namespace OTGUI {
class Output : public Variable
{
public:
  Output();
  Output(const std::string & name, const double & value=0., const std::string & description="",
         const std::string & formula="");
  Output(const Output & other);
  virtual Output * clone() const;

  virtual ~Output();

  std::string getFormula() const;
  void setFormula(const std::string & formula);
 
  std::string dump() const; 

private:
  std::string formula_;
};
typedef OT::Collection<Output> OutputCollection;
}
#endif