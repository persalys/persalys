//                                               -*- C++ -*-
/**
 *  @brief Copula inference result for a set of variables
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#ifndef OTGUI_COPULAINFERENCESETRESULT_HXX
#define OTGUI_COPULAINFERENCESETRESULT_HXX

#include "otgui/OTGuiprivate.hxx"

#include <openturns/Distribution.hxx>

namespace OTGUI {
class OTGUI_API CopulaInferenceSetResult : public OT::PersistentObject
{
  CLASSNAME;

public:
  friend class CopulaInferenceAnalysis;

  /** Default constructor */
  CopulaInferenceSetResult();

  /** Virtual constructor */
  virtual CopulaInferenceSetResult * clone() const;

  OT::Description getSetOfVariablesNames() const;
  OT::Collection<OT::Distribution> getTestedDistributions() const;
  OT::PersistentCollection<OT::PersistentCollection<OT::NumericalSample > > getKendallPlotData() const;
  OT::Description getErrorMessages() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  OT::Description setOfVariablesNames_;
  OT::PersistentCollection< OT::Distribution > testedDistributions_;
  OT::PersistentCollection<OT::PersistentCollection<OT::NumericalSample > > kendallPlotData_;
  OT::Description errorMessages_;
};
}
#endif