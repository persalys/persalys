//                                               -*- C++ -*-
/**
 *  @brief SRCAnalysis computes the Standard Regression Coefficients
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
#ifndef PERSALYS_SRCANALYSIS_HXX
#define PERSALYS_SRCANALYSIS_HXX

#include "SimulationAnalysis.hxx"
#include "SRCResult.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API SRCAnalysis : public SimulationAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  SRCAnalysis();
  /** Constructor with parameters */
  SRCAnalysis(const OT::String & name, const PhysicalModel & physicalModel, const OT::UnsignedInteger nbSimu = 10000);

  /** Virtual constructor */
  virtual SRCAnalysis * clone() const;

  OT::UnsignedInteger getSimulationsNumber() const;
  void setSimulationsNumber(const OT::UnsignedInteger number);

  SRCResult getResult() const;

  virtual Parameters getParameters() const;
  virtual OT::String getPythonScript() const;
  virtual bool hasValidResult() const;
  virtual bool canBeLaunched(OT::String &errorMessage) const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  virtual void initialize();
  virtual void launch();

private:
  OT::UnsignedInteger simulationsNumber_;
  SRCResult result_;
};
}
#endif
