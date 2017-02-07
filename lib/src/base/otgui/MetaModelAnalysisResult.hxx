//                                               -*- C++ -*-
/**
 *  @brief Class to define metamodel result
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#ifndef OTGUI_METAMODELANALYSISRESULT_HXX
#define OTGUI_METAMODELANALYSISRESULT_HXX

#include "PhysicalModel.hxx"

#include "otgui/OTGuiprivate.hxx"

namespace OTGUI {
class OTGUI_API MetaModelAnalysisResult : public OT::PersistentObject
{
  CLASSNAME;

public:

  friend class FunctionalChaosAnalysis;
  friend class KrigingAnalysis;
  friend class MetaModelAnalysis;

  /** Default constructor */
  MetaModelAnalysisResult();

  /** Virtual constructor */
  virtual MetaModelAnalysisResult * clone() const;

  PhysicalModel getMetaModel() const;

  OT::NumericalSample getOutputSample() const;

  OT::NumericalSample getMetaModelOutputSample() const;

  OT::NumericalSample getMetaModelOutputSampleLeaveOneOut() const;

  OT::NumericalPoint getErrorQ2LeaveOneOut() const;
  OT::NumericalPoint getQ2LeaveOneOut() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  PhysicalModel metaModel_;
  OT::NumericalSample outputSample_;
  OT::NumericalSample metaModelOutputSample_;
  OT::NumericalSample metaModelOutputSampleLOO_;
  OT::NumericalPoint errorQ2LOO_;
  OT::NumericalPoint q2LOO_;
};
}
#endif