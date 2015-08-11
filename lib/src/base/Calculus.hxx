// Calculus.hxx

#ifndef CALCULUS_H
#define CALCULUS_H

#include "CalculusImplementation.hxx"

namespace OTGUI {
class Calculus : public OT::TypedInterfaceObject<OTGUI::CalculusImplementation>
{
  CLASSNAME;

public:
  typedef OT::Pointer<OTGUI::CalculusImplementation>       Implementation;

  Calculus(const std::string & name, const PhysicalModel & physicalModel);
  Calculus(const CalculusImplementation & implementation);
  Calculus(const Implementation & p_implementation);
  Calculus(CalculusImplementation * p_implementation);
  Calculus(const Calculus & other);

  virtual ~Calculus();

  void addObserver(Observer * observer);

  std::string getName() const;
  void setName(const std::string & name);

  PhysicalModel getPhysicalModel() const;
  void setPhysicalModel(const PhysicalModel & physicalModel);

  void run();
  std::string dump() const;
  bool calculusLaunched() const;


};
}
#endif