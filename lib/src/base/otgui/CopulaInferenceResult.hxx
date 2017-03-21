//                                               -*- C++ -*-
/**
 *  @brief Copula Inference analysis result
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
#ifndef OTGUI_COPULAINFERENCERESULT_HXX
#define OTGUI_COPULAINFERENCERESULT_HXX

#include "CopulaInferenceSetResult.hxx"
#include "DesignOfExperiment.hxx"

namespace OTGUI {
class OTGUI_API CopulaInferenceResult : public OT::PersistentObject
{
  CLASSNAME;

public:
  typedef OT::Collection<CopulaInferenceSetResult> CopulaInferenceSetResultCollection;

  friend class CopulaInferenceAnalysis;

  /** Default constructor */
  CopulaInferenceResult();

  /** Virtual constructor */
  virtual CopulaInferenceResult * clone() const;

  CopulaInferenceSetResultCollection getCopulaInferenceSetResultCollection() const;
  CopulaInferenceSetResult getCopulaInferenceSetResult(const OT::Description& variablesNames) const;

  DesignOfExperiment getDesignOfExperiment() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::PersistentCollection< CopulaInferenceSetResult > copulaInferenceSetResultCollection_;
  DesignOfExperiment designOfExperiment_;
};
}
#endif