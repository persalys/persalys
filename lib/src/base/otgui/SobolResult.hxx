//                                               -*- C++ -*-
/**
 *  @brief SobolResult contains the Sobol indices
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#ifndef OTGUI_SOBOLRESULT_HXX
#define OTGUI_SOBOLRESULT_HXX

#include "otgui/OTGuiprivate.hxx"

#include <openturns/OTType.hxx>

namespace OTGUI
{
class OTGUI_API SobolResult : public OT::PersistentObject
{
  CLASSNAME

public:

  friend class SobolAnalysis;

  /** Default constructor */
  SobolResult();
  /** Constructor with parameters */
  SobolResult(const OT::Sample& firstOrderIndices,
              const OT::Sample& totalIndices,
              const OT::Description& outputNames);

  /** Virtual constructor */
  virtual SobolResult * clone() const;

  OT::Description getOutputNames() const;
  OT::Description getInputNames() const;
  OT::Sample getFirstOrderIndices() const;
  OT::Sample getTotalIndices() const;
  OT::Collection<OT::Interval> getFirstOrderIndicesInterval() const;
  OT::Collection<OT::Interval> getTotalIndicesInterval() const;

  OT::UnsignedInteger getCallsNumber() const;

  double getElapsedTime() const;

  double getCoefficientOfVariation() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::Description outputNames_;
  OT::Sample firstOrderIndices_;
  OT::Sample totalIndices_;
  OT::PersistentCollection<OT::Interval> firstOrderIndicesInterval_;
  OT::PersistentCollection<OT::Interval> totalIndicesInterval_;
  OT::UnsignedInteger callsNumber_;
  double elapsedTime_;
  double coefficientOfVariation_;
};
}
#endif
