//                                               -*- C++ -*-
/**
 *  @brief SobolAnalysis computes the Sobol indices
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#ifndef PERSALYS_SOBOLANALYSIS_HXX
#define PERSALYS_SOBOLANALYSIS_HXX

#include "SimulationAnalysis.hxx"
#include "WithStopCriteriaAnalysis.hxx"
#include "SobolResult.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API SobolAnalysis : public SimulationAnalysis, public WithStopCriteriaAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  SobolAnalysis();
  /** Constructor with parameters */
  SobolAnalysis(const OT::String & name, const PhysicalModel & physicalModel);

  /** Virtual constructor */
  SobolAnalysis * clone() const override;

  OT::Scalar getMaximumConfidenceIntervalLength() const;
  void setMaximumConfidenceIntervalLength(const OT::Scalar length);

  OT::UnsignedInteger getReplicationSize() const;
  void setReplicationSize(const OT::UnsignedInteger size);

  OT::Scalar getConfidenceLevel() const;
  void setConfidenceLevel(const OT::Scalar level);

  SobolResult getResult() const;

  Parameters getParameters() const override;
  OT::String getPythonScript() const override;
  bool hasValidResult() const override;
  bool canBeLaunched(OT::String &errorMessage) const override;

  void stop() override;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  static void UpdateProgressValue(double percent, void* data);
  void initialize() override;
  void launch() override;

private:
  TimeCriteria timeCriteria_;
  OT::Scalar maximumConfidenceIntervalLength_;
  OT::UnsignedInteger replicationSize_;
  OT::Scalar confidenceLevel_;
  SobolResult result_;
};
}
#endif
