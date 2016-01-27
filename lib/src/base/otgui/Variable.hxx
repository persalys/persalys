// Variable.hxx

#ifndef VARIABLE_H
#define VARIABLE_H

#include <PersistentObject.hxx>

namespace OTGUI {
class Variable : public OT::PersistentObject
{
  CLASSNAME;

public:
  Variable(const std::string & name="", const double & value=0., const std::string & description="");

    /* String converter */
  virtual OT::String __repr__() const;

  double getValue() const;
  void setValue(const double & value);

  std::string getDescription() const;
  void setDescription(const std::string & description);

  virtual std::string dump() const = 0;

private:
  double value_;
  std::string description_;
};
}
#endif