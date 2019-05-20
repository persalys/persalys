//                                               -*- C++ -*-
/**
 *  @brief SRCResult contains the Standard Regression Coefficients
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
#ifndef PERSALYS_SRCRESULT_HXX
#define PERSALYS_SRCRESULT_HXX

#include "EvaluationResult.hxx"

#include <openturns/OTType.hxx>

namespace PERSALYS
{
class PERSALYS_API SRCResult : public EvaluationResult
{
  CLASSNAME

public:

  friend class SRCAnalysis;

  /** Default constructor */
  SRCResult();

  /** Virtual constructor */
  virtual SRCResult * clone() const;

  OT::Description getOutputNames() const;
  OT::Description getInputNames() const;
  OT::Sample getIndices() const;
  OT::Sample getSignedIndices() const;
  OT::Collection<OT::Interval> getIndicesInterval() const;
  OT::Collection<OT::Interval> getSignedIndicesInterval() const;
  OT::UnsignedInteger getCallsNumber() const;
  OT::Point getR2() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::Description outputNames_;
  OT::Sample indices_;
  OT::Sample signedIndices_;
  OT::PersistentCollection<OT::Interval> indicesInterval_;
  OT::PersistentCollection<OT::Interval> signedIndicesInterval_;
  OT::Point r2_;
  OT::UnsignedInteger callsNumber_;
};
}
#endif
