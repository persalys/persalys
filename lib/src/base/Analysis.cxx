//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all analysis
 *
 *  Copyright 2015-2018 EDF-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/Analysis.hxx"

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(Analysis)

/* Default constructor */
Analysis::Analysis()
  : TypedInterfaceObject<AnalysisImplementation>(new AnalysisImplementation())
{
}


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


Bool Analysis::operator==(const Analysis& other) const
{
  if (this == &other) return true;
  return *getImplementation() == *other.getImplementation();
}


Bool Analysis::operator!=(const Analysis& other) const
{
  return !operator==(other);
}


void Analysis::addObserver(Observer * observer)
{
  getImplementation()->addObserver(observer);
}


bool Analysis::isReliabilityAnalysis() const
{
  return getImplementation()->isReliabilityAnalysis();
}


bool Analysis::isRunning() const
{
  return getImplementation()->isRunning();
}


String Analysis::getInformationMessage() const
{
  return getImplementation()->getInformationMessage();
}


String Analysis::getErrorMessage() const
{
  return getImplementation()->getErrorMessage();
}


String Analysis::getWarningMessage() const
{
  return getImplementation()->getWarningMessage();
}


int Analysis::getProgressValue() const
{
  return getImplementation()->getProgressValue();
}


void Analysis::run()
{
  getImplementation()->run();
}


bool Analysis::hasValidResult() const
{
  return getImplementation()->hasValidResult();
}


String Analysis::getPythonScript() const
{
  return getImplementation()->getPythonScript();
}


void Analysis::stop()
{
  getImplementation()->stop();
}


void Analysis::setImplementationAsPersistentObject(const ImplementationAsPersistentObject& obj)
{
  TypedInterfaceObject< AnalysisImplementation >::setImplementationAsPersistentObject(obj);
  getImplementation()->notify("implementationModified");
}


void Analysis::acceptLaunchParameters(LaunchParametersVisitor* visitor)
{
  getImplementation()->acceptLaunchParameters(visitor);
}
}
