// Calculus.cxx

#include "Calculus.hxx"


namespace OTGUI {

CLASSNAMEINIT(Calculus);

Calculus::Calculus(const std::string & name, const PhysicalModel & physicalModel)
  : OT::TypedInterfaceObject<OTGUI::CalculusImplementation>(new CalculusImplementation(name, physicalModel))
{
}

/* Default constructor */
Calculus::Calculus(const CalculusImplementation & implementation)
  : OT::TypedInterfaceObject<CalculusImplementation>(implementation.clone())
{
}


/* Constructor from implementation */
Calculus::Calculus(const Implementation & p_implementation)
  : OT::TypedInterfaceObject<CalculusImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}

/* Constructor from implementation pointer */
Calculus::Calculus(CalculusImplementation * p_implementation)
  : OT::TypedInterfaceObject<CalculusImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}


Calculus::Calculus(const Calculus& other)
  : OT::TypedInterfaceObject<CalculusImplementation>(other.getImplementation())
{
}


Calculus::~Calculus()
{
}


void Calculus::addObserver(Observer * observer)
{
  getImplementation()->addObserver(observer);
}


std::string Calculus::getName() const
{
  return getImplementation()->getName();
}


void Calculus::setName(const std::string & name)
{
  getImplementation()->setName(name);
}


PhysicalModel Calculus::getPhysicalModel() const
{
  return getImplementation()->getPhysicalModel();
}


void Calculus::setPhysicalModel(const PhysicalModel & physicalModel)
{
  getImplementation()->setPhysicalModel(physicalModel);
}


void Calculus::run()
{
  getImplementation()->run();
}


bool Calculus::calculusLaunched() const
{
  return getImplementation()->calculusLaunched();
}


std::string Calculus::dump() const
{
  return getImplementation()->dump();
}


}