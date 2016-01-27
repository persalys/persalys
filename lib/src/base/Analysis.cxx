// Analysis.cxx

#include "otgui/Analysis.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(Analysis);

Analysis::Analysis(const std::string & name, const PhysicalModel & physicalModel)
  : TypedInterfaceObject<AnalysisImplementation>(new AnalysisImplementation(name, physicalModel))
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


void Analysis::addObserver(Observer * observer)
{
  getImplementation()->addObserver(observer);
}


PhysicalModel Analysis::getPhysicalModel() const
{
  return getImplementation()->getPhysicalModel();
}


void Analysis::setPhysicalModel(const PhysicalModel & physicalModel)
{
  getImplementation()->setPhysicalModel(physicalModel);
}


bool Analysis::isReliabilityAnalysis() const
{
  return getImplementation()->isReliabilityAnalysis();
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