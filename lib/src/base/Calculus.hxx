// Calculus.hxx

#ifndef CALCULUS_H
#define CALCULUS_H

#include "PhysicalModel.hxx"

namespace OTGUI {
class Calculus : public OT::PersistentObject, public Observable
{
public:
  Calculus(const std::string & name, const PhysicalModel & physicalModel);
  Calculus(const Calculus & other);
  virtual Calculus * clone() const = 0;
  virtual ~Calculus();

  std::string getName() const;
  void setName(std::string name);

  PhysicalModel getPhysicalModel() const;
  void setPhysicalModel(const PhysicalModel & physicalModel);

  virtual void run() = 0;
  virtual std::string dump() const = 0;

private:
  std::string name_;
  PhysicalModel physicalModel_;
};
}
#endif