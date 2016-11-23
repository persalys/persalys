//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all analysis with physical model
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
#ifndef OTGUI_PHYSICALMODELANALYSIS_HXX
#define OTGUI_PHYSICALMODELANALYSIS_HXX

#include "AnalysisImplementation.hxx"

namespace OTGUI {
class OTGUI_API PhysicalModelAnalysis : public AnalysisImplementation
{
public:
  /** Default constructor */
  PhysicalModelAnalysis();

  /** Constructor with parameters */
  PhysicalModelAnalysis(const OT::String & name, const PhysicalModel & physicalModel);

  /** Virtual constructor */
  virtual PhysicalModelAnalysis * clone() const;

  PhysicalModel getPhysicalModel() const;

  OT::String getModelName() const;

  virtual void setInterestVariables(const OT::Description& outputsNames);

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  PhysicalModel physicalModel_;
};
}
#endif