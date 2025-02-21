//                                               -*- C++ -*-
/**
 *  @brief Results of an analysis
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
#ifndef PERSALYS_ANALYSISRESULT_HXX
#define PERSALYS_ANALYSISRESULT_HXX

#include "persalys/PersalysPrivate.hxx"

#include <openturns/PersistentObject.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API AnalysisResult : public OT::PersistentObject
{
  CLASSNAME

public:
  /** Default constructor */
  AnalysisResult();

  /** Virtual constructor */
  AnalysisResult * clone() const override;

  OT::Scalar getElapsedTime() const;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  OT::Scalar elapsedTime_;
};
}
#endif
