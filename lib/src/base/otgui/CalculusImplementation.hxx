// CalculusImplementation.hxx

#ifndef CALCULUSIMPLEMENTATION_H
#define CALCULUSIMPLEMENTATION_H

#include "PhysicalModel.hxx"

namespace OTGUI {
class CalculusImplementation : public OT::PersistentObject, public Observable
{
public:
  CalculusImplementation(const std::string & name, const PhysicalModel & physicalModel);
  CalculusImplementation(const CalculusImplementation & other);
  virtual CalculusImplementation * clone() const;

  virtual ~CalculusImplementation();

  std::string getName() const;
  void setName(const std::string & name);

  PhysicalModel getPhysicalModel() const;
  void setPhysicalModel(const PhysicalModel & physicalModel);

  virtual void run();
  virtual std::string dump() const;
  virtual bool calculusLaunched() const;

private:
  std::string name_;
  PhysicalModel physicalModel_;
};
}
#endif