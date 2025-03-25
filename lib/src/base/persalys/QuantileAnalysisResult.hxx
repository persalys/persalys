//                                               -*- C++ -*-
/**
 *  @brief Results of a quantile analysis
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
#ifndef PERSALYS_QUANTILEANALYSISRESULT_HXX
#define PERSALYS_QUANTILEANALYSISRESULT_HXX

#include "EvaluationResult.hxx"

#include <openturns/Distribution.hxx>

namespace PERSALYS
{

class PERSALYS_BASE_API QuantileAnalysisResult : public EvaluationResult
{
  CLASSNAME

public:
  typedef OT::Collection<OT::Sample> SampleCollection;
  typedef OT::Collection<OT::Point> PointCollection;
  enum Type {MonteCarlo, GeneralizedPareto};
  enum TailType {Lower = 1, Upper = 2, Bilateral = 4};

  friend class QuantileAnalysis;

  /** Default constructor */
  QuantileAnalysisResult();

  /** Constructor with parameters */
  explicit QuantileAnalysisResult(const DesignOfExperiment& design);

  /** Virtual constructor */
  QuantileAnalysisResult * clone() const override;

  OT::UnsignedInteger getSize() const {return quantiles_.size();}
  SampleCollection getQuantiles(const OT::String& varName) {return quantiles_[varName];}
  OT::Indices getWilksValidity(const OT::String& varName, TailType tailType) {return wilksValidity_[std::make_pair(varName,tailType)];}
  OT::Scalar getPValue(const OT::String& varName, TailType tailType) {return pValue_[std::make_pair(varName,tailType)];}

  OT::UnsignedInteger getType() const {return type_;}

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

private:
  OT::UnsignedInteger type_ = 0;
  std::map<OT::String, OT::PersistentCollection<OT::Sample>> quantiles_;
  std::map<std::pair<OT::String, TailType>, OT::Scalar> pValue_;
  std::map<std::pair<OT::String, TailType>, OT::Indices> wilksValidity_;

};
}
#endif
