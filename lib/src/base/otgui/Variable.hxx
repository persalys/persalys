// Variable.hxx

#ifndef VARIABLE_H
#define VARIABLE_H

#include <PersistentObject.hxx>

namespace OTGUI {
class Variable : public OT::PersistentObject
{
  CLASSNAME;

public:
  Variable(const OT::String & name="", const double & value=0., const OT::String & description="");

    /* String converter */
  virtual OT::String __repr__() const;

  double getValue() const;
  void setValue(const double & value);

  OT::String getDescription() const;
  void setDescription(const OT::String & description);

  virtual OT::String dump() const = 0;

private:
  double value_;
  OT::String description_;
};
}
#endif