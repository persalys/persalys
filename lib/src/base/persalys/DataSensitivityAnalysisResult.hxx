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


namespace PERSALYS
{
class PERSALYS_BASE_API DataSensitivityAnalysisResult : public EvaluationResult
{
  CLASSNAME

public:

  friend class DataSensitivityAnalysis;

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

  bool isIndependent() const;
  const OT::String& getIndependenceWarningMessage() const;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

private:
  OT::PersistentCollection<OT::Point> firstOrderSobolIndices_;
  OT::PersistentCollection<OT::Interval> firstOrderSobolIndicesInterval_;

  OT::Sample SRCIndices_;
  OT::Sample signedSRCIndices_;
  OT::Point r2_;
  OT::PersistentCollection<OT::Interval> SRCIndicesInterval_;
  OT::PersistentCollection<OT::Interval> signedSRCIndicesInterval_;

  bool isIndependent_ = true;
  OT::String independenceWarningMessage_; // used to store the warning message if the variables are not independent

};

} // namespace PERSALYS

#endif // PERSALYS_DATASENSITIVITYANALYSISRESULT_HXX