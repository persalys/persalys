//                                               -*- C++ -*-
/**
 *  @brief Class to define metamodel kriging result
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
#ifndef PERSALYS_KRIGINGANALYSISRESULT_HXX
#define PERSALYS_KRIGINGANALYSISRESULT_HXX

#include "MetaModelAnalysisResult.hxx"

#include <openturns/KrigingResult.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API KrigingAnalysisResult : public MetaModelAnalysisResult
{
  CLASSNAME

public:

  friend class KrigingAnalysis;
  typedef OT::Collection< OT::KrigingResult > KrigingResultCollection;

  /** Default constructor */
  KrigingAnalysisResult();

  /** Virtual constructor */
  KrigingAnalysisResult * clone() const override;

  KrigingResultCollection getKrigingResultCollection() const;
  OT::KrigingResult getResultForVariable(const OT::String& variableName) const;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  OT::PersistentCollection< OT::KrigingResult > krigingResultCollection_;
};
}
#endif
