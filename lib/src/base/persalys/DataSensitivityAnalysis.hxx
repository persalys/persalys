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
#include <openturns/Function.hxx>

#ifndef PERSALYS_DATASENSITIVITYANALYSIS_HXX
#define PERSALYS_DATASENSITIVITYANALYSIS_HXX

 namespace PERSALYS
{
class PERSALYS_BASE_API DataSensitivityAnalysis : public DesignOfExperimentAnalysis
{
  CLASSNAME

#ifndef SWIG
  struct HSICParameters
  {
    HSICParameters(bool computePermutationPValues = false, bool computeAsymptoticPValues = false, bool useUStatistic = false)
      : computePermutationPValues_(computePermutationPValues)
      , computeAsymptoticPValues_(computeAsymptoticPValues)
      , useUStatistic_(useUStatistic)
    {}

    bool computePermutationPValues()  const { return computePermutationPValues_; }
    bool computeAsymptoticPValues()   const { return computeAsymptoticPValues_; }
    bool useUStatistic()              const { return useUStatistic_; }

    bool computePermutationPValues_ = false;
    bool computeAsymptoticPValues_ = false;
    bool useUStatistic_ = false;
  };
#endif

public:
  using Type = DataSensitivityAnalysisResult::Type;
  using HSICType = DataSensitivityAnalysisResult::HSICType;

  /** constructors */
  DataSensitivityAnalysis() = default;

  /** Constructor with parameters */
  DataSensitivityAnalysis(
    const OT::String &name, 
    const DesignOfExperiment& design, 
    const unsigned char analysisType = Type::RankSobol | Type::SRC,
    const OT::Description & interestVariables = OT::Description(),
    bool computeCovModelParameters = true
  );

  /** Virtual constructor */
  DataSensitivityAnalysis * clone() const override;

  void setType(unsigned char analysisType);

  void setCovarianceModels(const OT::Collection<OT::CovarianceModel> &covarianceModels, HSICType hsicType);

  void setFilterAlphas(const OT::Point & filterAlphas);
  void setWeightAlphas(const OT::Point & weightAlphas);
  void setFilterFunctions(const OT::Collection<OT::Function> &filterFunctions);
  void setWeightFunctions(const OT::Collection<OT::Function> &weightFunctions);

  void setHSICParameters(bool computePermutationPValues, bool computeAsymptoticPValues, bool useUStatistic, HSICType hsicType);

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
  bool computeHSIC(HSICType hsicType) const;

  bool computeAsymptoticPValues(HSICType hsicType) const;
  bool computePermutationPValues(HSICType hsicType) const;
  bool useUStatistic(HSICType hsicType) const;

  OT::Collection<OT::CovarianceModel> getCovarianceModels(HSICType hsicType) const;

  OT::Point getFilterAlphas() const;
  OT::Point getWeightAlphas() const;
  OT::Collection<OT::Function> getFilterFunctions() const;
  OT::Collection<OT::Function> getWeightFunctions() const;

  bool defaultHSICParametersChanged() const;

protected:
  void initialize() override;
  void launch() override;

private:
  void computeSobolIndices();
  void computeSRCIndices();
  void computeGlobalHSICIndices();
  void computeTargetHSICIndices();
  void computeConditionalHSICIndices();
  
  void checkIndependance();

private:
  DataSensitivityAnalysisResult result_;
  DataSensitivityAnalysisResult::AnalysisType type_ = std::byte{0b0011};
  OT::PersistentCollection<OT::CovarianceModel> globalCovarianceModels_;
  OT::PersistentCollection<OT::CovarianceModel> targetCovarianceModels_;
  OT::PersistentCollection<OT::CovarianceModel> conditionalCovarianceModels_;
  HSICParameters globalHSICParameters_;
  HSICParameters targetHSICParameters_;
  HSICParameters conditionalHSICParameters_;
  OT::PersistentCollection<OT::Function> filterFunctions_;
  OT::PersistentCollection<OT::Function> weightFunctions_;
  OT::Bool computeCovModelParameters_ = true;
  OT::Bool defaultHSICParametersChanged_ = false;
  OT::Description interestVariables_;
  OT::Point filterAlphas_;
  OT::Point weightAlphas_;

};

} // namespace PERSALYS

 #endif // PERSALYS_DATASENSITIVITYANALYSIS_HXX