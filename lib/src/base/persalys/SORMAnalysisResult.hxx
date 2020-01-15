//                                               -*- C++ -*-
/**
 *  @brief Results of a SORM analysis
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_SORMANALYSISRESULT_HXX
#define PERSALYS_SORMANALYSISRESULT_HXX

#include "AnalysisResult.hxx"

#include <openturns/SORMResult.hxx>

namespace PERSALYS
{
class PERSALYS_API SORMAnalysisResult : public AnalysisResult
{
  CLASSNAME

public:
  friend class SORMAnalysis;

  /** Default constructor */
  SORMAnalysisResult();

  /** Constructor with parameters */
  SORMAnalysisResult(const OT::SORMResult& sormResult);

  /** Virtual constructor */
  virtual SORMAnalysisResult * clone() const;

  OT::SORMResult getSORMResult() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  OT::SORMResult sormResult_;
};
}
#endif
