//                                               -*- C++ -*-
/**
 *  @brief Base class to make a sensitivity analysis of a data model
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

#include "DesignOfExperimentAnalysis.hxx"
#include "DataSensitivityAnalysisResult.hxx"

#ifndef PERSALYS_DATASENSITIVITYANALYSIS_HXX
#define PERSALYS_DATASENSITIVITYANALYSIS_HXX

 namespace PERSALYS
{
class PERSALYS_BASE_API DataSensitivityAnalysis : public DesignOfExperimentAnalysis
{
  CLASSNAME
public:
  /** constructors */
  DataSensitivityAnalysis();

  /** Constructor with parameters */
  explicit DataSensitivityAnalysis(const OT::String &name, const DesignOfExperiment& design);

  /** Virtual constructor */
  DataSensitivityAnalysis * clone() const override;

  bool canBeLaunched(OT::String &errorMessage) const override;
  bool hasValidResult() const override;

  const DataSensitivityAnalysisResult& getResult() const;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

  static bool CanBeLaunched(OT::String &errorMessage, const DesignOfExperiment &doe);

protected:
  void initialize() override;
  void launch() override;

private:
  void computeSobolIndices();
  void computeSRCIndices();
  void checkIndependance();

private:
  DataSensitivityAnalysisResult result_;
};

} // namespace PERSALYS

 #endif // PERSALYS_DATASENSITIVITYANALYSIS_HXX