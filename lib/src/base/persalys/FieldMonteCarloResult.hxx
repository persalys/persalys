//                                               -*- C++ -*-
/**
 *  @brief Monte Carlo analysis result
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#ifndef PERSALYS_FIELDMONTECARLORESULT_HXX
#define PERSALYS_FIELDMONTECARLORESULT_HXX

#include "EvaluationResult.hxx"

#include <openturns/KarhunenLoeveResult.hxx>

namespace PERSALYS
{

class PERSALYS_BASE_API FieldMonteCarloResult : public EvaluationResult
{
  CLASSNAME

public:
  friend class FieldMonteCarloAnalysis;

  /** Default constructor */
  FieldMonteCarloResult();

  /** Virtual constructor */
  FieldMonteCarloResult * clone() const override;

  OT::ProcessSample getProcessSample() const;
  OT::Sample getMeanSample() const;
  OT::Sample getLowerQuantileSample() const;
  OT::Sample getUpperQuantileSample() const;
  OT::Collection<OT::Sample> getXiSamples() const;
  OT::Collection<OT::Function> getCorrelationFunction() const;
  OT::Collection<OT::KarhunenLoeveResult> getKarhunenLoeveResults() const;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

private:
  OT::ProcessSample processSample_;
  OT::Sample meanSample_;
  OT::Sample lowerQuantileSample_;
  OT::Sample upperQuantileSample_;
  OT::PersistentCollection<OT::Sample> xiSamples_;
  OT::PersistentCollection<OT::Function> correlationFunction_;
  OT::PersistentCollection<OT::KarhunenLoeveResult > karhunenLoeveResults_;
};
}
#endif
