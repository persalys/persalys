//                                               -*- C++ -*-
/**
 *  @brief Copula inference result for a set of variables
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
#ifndef PERSALYS_COPULAINFERENCESETRESULT_HXX
#define PERSALYS_COPULAINFERENCESETRESULT_HXX

#include "persalys/PersalysPrivate.hxx"

#include <openturns/ProcessSample.hxx>
#include <openturns/Distribution.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API CopulaInferenceSetResult : public OT::PersistentObject
{
  CLASSNAME

public:
  friend class CopulaInferenceAnalysis;

  /** Default constructor */
  CopulaInferenceSetResult();

  /** Virtual constructor */
  CopulaInferenceSetResult * clone() const override;

  OT::Description getSetOfVariablesNames() const;
  OT::Collection<OT::Distribution> getTestedDistributions() const;
  OT::Point getBICResults() const;
  OT::PersistentCollection<OT::ProcessSample > getKendallPlotData() const;
  OT::Description getErrorMessages() const;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  OT::Description setOfVariablesNames_;
  OT::PersistentCollection< OT::Distribution > testedDistributions_;
  OT::Point bicResults_;
  OT::PersistentCollection<OT::ProcessSample > kendallPlotData_;
  OT::Description errorMessages_;
};
}
#endif
