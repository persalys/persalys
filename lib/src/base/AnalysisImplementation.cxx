// AnalysisImplementation.cxx

#include "otgui/AnalysisImplementation.hxx"


namespace OTGUI {

AnalysisImplementation::AnalysisImplementation(const std::string & name, const PhysicalModel & physicalModel)
  : PersistentObject()
  , Observable()
  , name_(name)
  , physicalModel_(physicalModel)
{
}


AnalysisImplementation::AnalysisImplementation(const AnalysisImplementation & other)
  : PersistentObject()
  , Observable()
  , name_(other.name_)
  , physicalModel_(other.physicalModel_)
{
}


AnalysisImplementation * AnalysisImplementation::clone() const
{
  return new AnalysisImplementation(*this);
}


AnalysisImplementation::~AnalysisImplementation()
{
}


std::string AnalysisImplementation::getName() const
{
  return name_;
}


void AnalysisImplementation::setName(const std::string & name)
{
  name_ = name;
}


PhysicalModel AnalysisImplementation::getPhysicalModel() const
{
  return physicalModel_;
}


void AnalysisImplementation::setPhysicalModel(const PhysicalModel & physicalModel)
{
  physicalModel_ = physicalModel;
}


void AnalysisImplementation::run()
{

}


bool AnalysisImplementation::analysisLaunched() const
{
  return false;
}


std::string AnalysisImplementation::dump() const
{
  return "";
}

}