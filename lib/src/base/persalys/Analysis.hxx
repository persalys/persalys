//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all analysis
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef PERSALYS_ANALYSIS_HXX
#define PERSALYS_ANALYSIS_HXX

#include "AnalysisImplementation.hxx"

namespace PERSALYS
{
class LaunchParametersVisitor;

class PERSALYS_API Analysis : public OT::TypedInterfaceObject<AnalysisImplementation>
{
  CLASSNAME

public:
  typedef OT::Pointer<AnalysisImplementation> Implementation;

  /** Default constructor */
  Analysis();
  /** Default constructor */
  Analysis(const AnalysisImplementation & implementation);
  /** Constructor from implementation */
  Analysis(const Implementation & p_implementation);
  /** Constructor from implementation pointer */
  Analysis(AnalysisImplementation * p_implementation);

  /** Comparison operators */
  OT::Bool operator ==(const Analysis & other) const;
  OT::Bool operator !=(const Analysis & other) const;

  void addObserver(Observer * observer);
  Observer * getParentObserver() const;

  bool isReliabilityAnalysis() const;

  bool isRunning() const;

  OT::String getInformationMessage() const;
  OT::String getErrorMessage() const;
  OT::String getWarningMessage() const;
  int getProgressValue() const;
  OT::String getHtmlDescription() const;

  void run();
  Parameters getParameters() const;
  OT::String getPythonScript() const;
  bool hasValidResult() const;
  bool canBeLaunched(OT::String &errorMessage) const;

  void stop();

  /** override this method in order to emit a notification */
  virtual void setImplementationAsPersistentObject(const ImplementationAsPersistentObject& obj);

  void acceptLaunchParameters(LaunchParametersVisitor* visitor);
};
}
#endif
