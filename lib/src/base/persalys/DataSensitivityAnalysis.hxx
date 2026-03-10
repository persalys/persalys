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

#include <openturns/CovarianceModel.hxx>

#ifndef PERSALYS_DATASENSITIVITYANALYSIS_HXX
#define PERSALYS_DATASENSITIVITYANALYSIS_HXX

 namespace PERSALYS
{
class PERSALYS_BASE_API DataSensitivityAnalysis : public DesignOfExperimentAnalysis
{
  CLASSNAME

public:
  using Type = DataSensitivityAnalysisResult::Type;

  /** constructors */
  DataSensitivityAnalysis() = default;

  /** Constructor with parameters */
  DataSensitivityAnalysis(
    const OT::String &name, 
    const DesignOfExperiment& design, 
    const unsigned char analysisType = Type::RankSobol | Type::SRC,
    const OT::Collection<OT::CovarianceModel> &covarianceModels = OT::Collection<OT::CovarianceModel>(),
    bool computeCovModelParameters = true
  );

  /** Virtual constructor */
  DataSensitivityAnalysis * clone() const override;

  void setHSICParameters(bool computeAsymptoticPValues, bool computePermutationPValues, bool useUStatistic);

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
  Parameters getParameters() const override;

  bool computeRankSobol() const;
  bool computeSRC() const;
  bool computeGlobalHSIC() const;

  bool computeAsymptoticPValues() const;
  bool computePermutationPValues() const;
  bool useUStatistic() const;

  OT::Collection<OT::CovarianceModel> getCovarianceModels() const;

  bool defaultHSICParametersChanged() const;

protected:
  void initialize() override;
  void launch() override;

private:
  void computeSobolIndices();
  void computeSRCIndices();
  void computeGlobalHSICIndices();
  
  void checkIndependance();

private:
  DataSensitivityAnalysisResult result_;
  DataSensitivityAnalysisResult::AnalysisType type_ = std::byte{0b0011};
  OT::PersistentCollection<OT::CovarianceModel> covarianceModels_;
  OT::Bool computeAsymptoticPValues_ = false;
  OT::Bool computePermutationPValues_ = false;
  OT::Bool useUStatistic_ = false;
  OT::Bool computeCovModelParameters_ = true;
  OT::Bool defaultHSICParametersChanged_ = false;
};

} // namespace PERSALYS

 #endif // PERSALYS_DATASENSITIVITYANALYSIS_HXX