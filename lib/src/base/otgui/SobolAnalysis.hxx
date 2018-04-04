//                                               -*- C++ -*-
/**
 *  @brief SobolAnalysis computes the Sobol indices
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_SOBOLANALYSIS_HXX
#define OTGUI_SOBOLANALYSIS_HXX

#include "SimulationAnalysis.hxx"
#include "WithStopCriteriaAnalysis.hxx"
#include "SobolResult.hxx"

namespace OTGUI
{
class OTGUI_API SobolAnalysis : public SimulationAnalysis, public WithStopCriteriaAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  SobolAnalysis();
  /** Constructor with parameters */
  SobolAnalysis(const OT::String & name, const PhysicalModel & physicalModel);

  /** Virtual constructor */
  virtual SobolAnalysis * clone() const;

  OT::UnsignedInteger getBootstrapSize() const;
  void setBootstrapSize(const OT::UnsignedInteger size);

  double getBootstrapConfidenceLevel() const;
  void setBootstrapConfidenceLevel(const double level);

  SobolResult getResult() const;

  virtual Parameters getParameters() const;
  virtual OT::String getPythonScript() const;
  virtual bool hasValidResult() const;

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
  OT::UnsignedInteger bootstrapSize_;
  double bootstrapConfidenceLevel_;
  SobolResult result_;
};
}
#endif
