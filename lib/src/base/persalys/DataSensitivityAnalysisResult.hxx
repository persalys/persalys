//                                               -*- C++ -*-
/**
 *  @brief Results of a data sensitivity analysis
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

#ifndef PERSALYS_DATASENSITIVITYANALYSISRESULT_HXX
#define PERSALYS_DATASENSITIVITYANALYSISRESULT_HXX

#include "EvaluationResult.hxx"

#include <openturns/CovarianceModel.hxx>
#include <cstddef>

namespace PERSALYS
{
class PERSALYS_BASE_API DataSensitivityAnalysisResult : public EvaluationResult
{
  CLASSNAME

public:

  friend class DataSensitivityAnalysis;

  enum HSICType {Global, Target, Conditional};

  enum Type : unsigned char
  {
    RankSobol       = 0b00000001,
    SRC             = 0b00000010,
    GlobalHSIC      = 0b00000100,
    TargetHSIC      = 0b00001000,
    ConditionalHSIC = 0b00010000
  };

private:
  /**
   * @brief This struct is used to determine which indices should be computed in the analysis launch() method.
   * 
   * The type is encoded in a byte, where each bit corresponds to an index type:
   * - bit 0 (0b0001): compute Rank Sobol indices
   * - bit 1 (0b0010): compute SRC indices
   * - bit 2 (0b0100): compute global HSIC indices
   * - bit 3 (0b1000): compute target HSIC indices
   * - bit 4 (0b10000): compute conditional HSIC indices
   */
  struct AnalysisType
  {
    AnalysisType(std::byte type) : type_(type) {}

    bool computeRankSobol()       const {return (type_ & std::byte{RankSobol})        != std::byte{0};}
    bool computeSRC()             const {return (type_ & std::byte{SRC})              != std::byte{0};}
    bool computeGlobalHSIC()      const {return (type_ & std::byte{GlobalHSIC})       != std::byte{0};}
    bool computeTargetHSIC()      const {return (type_ & std::byte{TargetHSIC})       != std::byte{0};}
    bool computeConditionalHSIC() const {return (type_ & std::byte{ConditionalHSIC})  != std::byte{0};}

    std::byte getType() const { return type_; }

    private:
      std::byte type_;
  };

public:

  /** Default constructor */
  DataSensitivityAnalysisResult();

  /** Constructor with parameters */
  explicit DataSensitivityAnalysisResult(const DesignOfExperiment& design);

  /** Virtual constructor */
  DataSensitivityAnalysisResult* clone() const override;

  /** Accessors */
  const OT::Collection<OT::Point>& getFirstOrderSobolIndices() const;
  const OT::Collection<OT::Interval>& getFirstOrderSobolIndicesInterval() const;

  const OT::Sample& getSRCIndices() const;
  const OT::Sample& getSignedSRCIndices() const;
  const OT::Collection<OT::Interval>& getSRCIndicesInterval() const;
  const OT::Collection<OT::Interval>& getSignedSRCIndicesInterval() const;
  const OT::Point& getR2() const;

  const OT::Collection<OT::Point>& getHSICIndices(HSICType hsicType) const;
  const OT::Collection<OT::Point>& getR2HSICIndices(HSICType hsicType) const;
  const OT::Collection<OT::Point>& getPValuesAsymptotic(HSICType hsicType) const;
  const OT::Collection<OT::Point>& getPValuesPermutation(HSICType hsicType) const;
  bool computeHSICPValuesAsymptotic(HSICType hsicType) const;
  bool computeHSICPValuesPermutation(HSICType hsicType) const;

  const OT::Description& getInterestVariables() const;

#ifndef SWIG
  const AnalysisType& getAnalysisType() const;
#endif

  bool isIndependent() const;
  const OT::String& getIndependenceWarningMessage() const;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

private:
  AnalysisType analysisType_ = AnalysisType(std::byte{0});

  OT::PersistentCollection<OT::Point> firstOrderSobolIndices_;
  OT::PersistentCollection<OT::Interval> firstOrderSobolIndicesInterval_;

  OT::Sample SRCIndices_;
  OT::Sample signedSRCIndices_;
  OT::Point r2_;
  OT::PersistentCollection<OT::Interval> SRCIndicesInterval_;
  OT::PersistentCollection<OT::Interval> signedSRCIndicesInterval_;

  OT::PersistentCollection<OT::Point> globalHSICIndices_;
  OT::PersistentCollection<OT::Point> globalR2HSICIndices_;
  OT::PersistentCollection<OT::Point> globalPValuesAsymptotic_;
  OT::PersistentCollection<OT::Point> globalPValuesPermutation_;

  OT::PersistentCollection<OT::CovarianceModel> globalCovarianceModels_;  
  bool computeGlobalAsymptoticPValues_  = false;                          
  bool computeGlobalPermutationPValues_ = false;                           
  bool useUStatisticGlobal_             = false;                          

  OT::PersistentCollection<OT::Point> targetHSICIndices_;
  OT::PersistentCollection<OT::Point> targetR2HSICIndices_;
  OT::PersistentCollection<OT::Point> targetPValuesAsymptotic_;
  OT::PersistentCollection<OT::Point> targetPValuesPermutation_;

  OT::PersistentCollection<OT::CovarianceModel> targetCovarianceModels_;
  bool computeTargetAsymptoticPValues_  = false;
  bool computeTargetPermutationPValues_ = false;
  bool useUStatisticTarget_             = false;

  OT::PersistentCollection<OT::Point> conditionalHSICIndices_;
  OT::PersistentCollection<OT::Point> conditionalR2HSICIndices_;
  OT::PersistentCollection<OT::Point> conditionalPValuesPermutation_;

  OT::PersistentCollection<OT::CovarianceModel> conditionalCovarianceModels_;
  bool computeConditionalPermutationPValues_ = false;

  OT::Description interestVariables_;
                                                                    
  bool isIndependent_ = true;
  OT::String independenceWarningMessage_; // used to store the warning message if the variables are not independent

};

} // namespace PERSALYS

#endif // PERSALYS_DATASENSITIVITYANALYSISRESULT_HXX