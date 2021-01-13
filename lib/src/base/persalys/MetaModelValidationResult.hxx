//                                               -*- C++ -*-
/**
 *  @brief Class to define metamodel validation result
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#ifndef PERSALYS_METAMODELVALIDATIONRESULT_HXX
#define PERSALYS_METAMODELVALIDATIONRESULT_HXX

#include "persalys/PersalysPrivate.hxx"

#include <openturns/OTType.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API MetaModelValidationResult : public OT::PersistentObject
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
  MetaModelValidationResult * clone() const override;

  // attributs accessors
  OT::String getName() const;
  OT::PointWithDescription getParameters() const;
  OT::Sample getMetaModelOutputSample() const;
  OT::Point getResiduals() const;
  OT::Point getQ2() const;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

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
