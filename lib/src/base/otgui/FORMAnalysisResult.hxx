//                                               -*- C++ -*-
/**
 *  @brief Results of a FORM analysis
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#ifndef OTGUI_FORMANALYSISRESULT_HXX
#define OTGUI_FORMANALYSISRESULT_HXX

#include "AnalysisResult.hxx"

#include <openturns/FORMResult.hxx>

namespace OTGUI
{
class OTGUI_API FORMAnalysisResult : public AnalysisResult
{
  CLASSNAME

public:
  friend class FORMAnalysis;

  /** Default constructor */
  FORMAnalysisResult();

  /** Constructor with parameters */
  FORMAnalysisResult(const OT::FORMResult& formResult);

  /** Virtual constructor */
  virtual FORMAnalysisResult * clone() const;

  OT::FORMResult getFORMResult() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  OT::FORMResult formResult_;
};
}
#endif
