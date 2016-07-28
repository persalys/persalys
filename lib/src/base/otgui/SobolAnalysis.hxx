//                                               -*- C++ -*-
/**
 *  @brief SobolAnalysis computes the Sobol indices
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
#ifndef OTGUI_SOBOLANALYSIS_HXX
#define OTGUI_SOBOLANALYSIS_HXX

#include "SimulationAnalysis.hxx"
#include "SobolResult.hxx"

namespace OTGUI {
class OTGUI_API SobolAnalysis : public SimulationAnalysis
{
  CLASSNAME;

public:
  /** Default constructor */
  SobolAnalysis();
  /** Constructor with parameters */
  SobolAnalysis(const OT::String & name, const PhysicalModel & physicalModel, const OT::UnsignedInteger nbSimu=10000);

  /** Virtual constructor */
  virtual SobolAnalysis * clone() const;

  OT::UnsignedInteger getSimulationsNumber() const;
  void setSimulationsNumber(const OT::UnsignedInteger number);

  OT::UnsignedInteger getBlockSize() const;
  void setBlockSize(const OT::UnsignedInteger & size);

  SobolResult getResult() const;

  virtual void run();
  virtual OT::String getPythonScript() const;
  virtual bool analysisLaunched() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::UnsignedInteger simulationsNumber_;
  OT::UnsignedInteger blockSize_;
  SobolResult result_;
};
}
#endif