//                                               -*- C++ -*-
/**
 *  @brief Analysis SORM
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#ifndef PERSALYS_SORMANALYSIS_HXX
#define PERSALYS_SORMANALYSIS_HXX

#include "ApproximationAnalysis.hxx"
#include "ReliabilityAnalysis.hxx"
#include "SORMAnalysisResult.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API SORMAnalysis : public ReliabilityAnalysis, public ApproximationAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  SORMAnalysis();
  /** Constructor with parameters */
  SORMAnalysis(const OT::String& name, const LimitState& limitState);

  /** Virtual constructor */
  SORMAnalysis * clone() const override;

  SORMAnalysisResult getResult() const;

  Parameters getParameters() const override;
  OT::String getPythonScript() const override;
  bool hasValidResult() const override;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  void initialize() override;
  void launch() override;

private:
  SORMAnalysisResult result_;
};
}
#endif
