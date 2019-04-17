//                                               -*- C++ -*-
/**
 *  @brief Copula inference result for a set of variables
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
#ifndef OTGUI_COPULAINFERENCESETRESULT_HXX
#define OTGUI_COPULAINFERENCESETRESULT_HXX

#include "otgui/OTGuiprivate.hxx"

#include <openturns/ProcessSample.hxx>
#include <openturns/Distribution.hxx>

namespace OTGUI
{
class OTGUI_API CopulaInferenceSetResult : public OT::PersistentObject
{
  CLASSNAME

public:
  friend class CopulaInferenceAnalysis;

  /** Default constructor */
  CopulaInferenceSetResult();

  /** Virtual constructor */
  virtual CopulaInferenceSetResult * clone() const;

  OT::Description getSetOfVariablesNames() const;
  OT::Collection<OT::Distribution> getTestedDistributions() const;
  OT::Point getBICResults() const;
  OT::PersistentCollection<OT::ProcessSample > getKendallPlotData() const;
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
  OT::Point bicResults_;
  OT::PersistentCollection<OT::ProcessSample > kendallPlotData_;
  OT::Description errorMessages_;
};
}
#endif
