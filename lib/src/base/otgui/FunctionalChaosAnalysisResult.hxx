//                                               -*- C++ -*-
/**
 *  @brief Class to define metamodel functional chaos result
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_FUNCTIONALCHAOSANALYSISRESULT_HXX
#define OTGUI_FUNCTIONALCHAOSANALYSISRESULT_HXX

#include "MetaModelAnalysisResult.hxx"
#include "SobolResult.hxx"

#include "openturns/FunctionalChaosResult.hxx"

namespace OTGUI {
class OTGUI_API FunctionalChaosAnalysisResult : public MetaModelAnalysisResult
{
  CLASSNAME;

public:

  friend class FunctionalChaosAnalysis;

  /** Default constructor */
  FunctionalChaosAnalysisResult();

  /** Virtual constructor */
  virtual FunctionalChaosAnalysisResult * clone() const;

  OT::FunctionalChaosResult getFunctionalChaosResult() const;

  OT::NumericalPoint getMean() const;
  OT::NumericalPoint getVariance() const;

  SobolResult getSobolResult() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  OT::FunctionalChaosResult functionalChaosResult_;
  OT::NumericalPoint mean_;
  OT::NumericalPoint variance_;
  SobolResult sobolResult_;
};
}
#endif