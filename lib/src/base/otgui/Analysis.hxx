//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all analysis 
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#ifndef OTGUI_ANALYSIS_HXX
#define OTGUI_ANALYSIS_HXX

#include "AnalysisImplementation.hxx"

namespace OTGUI {
class OTGUI_API Analysis : public OT::TypedInterfaceObject<AnalysisImplementation>
{
  CLASSNAME;

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

  void addObserver(Observer * observer);

  OT::String getModelName() const;

  bool isReliabilityAnalysis() const;

  void run();
  OT::String getPythonScript() const;
  bool analysisLaunched() const;
};
}
#endif