//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all analysis 
 *
 *  Copyright 2015-2016 EDF
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
class Analysis : public OT::TypedInterfaceObject<AnalysisImplementation>
{
  CLASSNAME;

public:
  typedef OT::Pointer<AnalysisImplementation>       Implementation;

  Analysis(const OT::String & name="Unamed", const PhysicalModel & physicalModel=PhysicalModel());
  Analysis(const AnalysisImplementation & implementation);
  Analysis(const Implementation & p_implementation);
  Analysis(AnalysisImplementation * p_implementation);

  void addObserver(Observer * observer);

  PhysicalModel getPhysicalModel() const;
  void setPhysicalModel(const PhysicalModel & physicalModel);

  bool isReliabilityAnalysis() const;

  void run();
  OT::String dump() const;
  bool analysisLaunched() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);
};
}
#endif