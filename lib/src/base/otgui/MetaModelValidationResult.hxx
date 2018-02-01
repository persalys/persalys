//                                               -*- C++ -*-
/**
 *  @brief Class to define metamodel validation result
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_METAMODELVALIDATIONRESULT_HXX
#define OTGUI_METAMODELVALIDATIONRESULT_HXX

#include "otgui/OTGuiprivate.hxx"

#include <openturns/OTType.hxx>

namespace OTGUI
{
class OTGUI_API MetaModelValidationResult : public OT::PersistentObject
{
  CLASSNAME

public:

  friend class FunctionalChaosAnalysis;
  friend class KrigingAnalysis;
  friend class MetaModelAnalysis;

  /** Default constructor */
  MetaModelValidationResult();
  /** Constructor with parameters */
  MetaModelValidationResult(const OT::String& name);
  MetaModelValidationResult(const OT::Sample& metaModelSample,
                            const OT::Point& q2,
                            const OT::Point& residuals);

  /** Virtual constructor */
  virtual MetaModelValidationResult * clone() const;

  // attributs accessors
  OT::String getName() const;
  OT::PointWithDescription getParameters() const;
  OT::Sample getMetaModelOutputSample() const;
  OT::Point getResiduals() const;
  OT::Point getQ2() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  void add(const MetaModelValidationResult& other);

protected:
  OT::String name_;
  OT::PointWithDescription parameters_;
  OT::Sample metaModelSample_;
  OT::Point q2_;
  OT::Point residuals_;
};
}
#endif
