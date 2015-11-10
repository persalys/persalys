// Analysis.cxx

#include "otgui/Analysis.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(Analysis);

Analysis::Analysis(const std::string & name, const PhysicalModel & physicalModel)
  : TypedInterfaceObject<OTGUI::AnalysisImplementation>(new AnalysisImplementation(name, physicalModel))
{
}

/* Default constructor */
Analysis::Analysis(const AnalysisImplementation & implementation)
  : TypedInterfaceObject<AnalysisImplementation>(implementation.clone())
{
}


/* Constructor from implementation */
Analysis::Analysis(const Implementation & p_implementation)
  : TypedInterfaceObject<AnalysisImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}

/* Constructor from implementation pointer */
Analysis::Analysis(AnalysisImplementation * p_implementation)
  : TypedInterfaceObject<AnalysisImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}


Analysis::Analysis(const Analysis& other)
  : TypedInterfaceObject<AnalysisImplementation>(other.getImplementation())
{
}


Analysis::~Analysis()
{
}


void Analysis::addObserver(Observer * observer)
{
  getImplementation()->addObserver(observer);
}


std::string Analysis::getName() const
{
  return getImplementation()->getName();
}


void Analysis::setName(const std::string & name)
{
  getImplementation()->setName(name);
}


PhysicalModel Analysis::getPhysicalModel() const
{
  return getImplementation()->getPhysicalModel();
}


void Analysis::setPhysicalModel(const PhysicalModel & physicalModel)
{
  getImplementation()->setPhysicalModel(physicalModel);
}


void Analysis::run()
{
  getImplementation()->run();
}


bool Analysis::analysisLaunched() const
{
  return getImplementation()->analysisLaunched();
}


std::string Analysis::dump() const
{
  return getImplementation()->dump();
}


}