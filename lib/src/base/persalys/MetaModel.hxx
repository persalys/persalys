//                                               -*- C++ -*-
/**
 *  @brief MetaModel implements models created by MetaModelAnalysis
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
#ifndef PERSALYS_METAMODEL_HXX
#define PERSALYS_METAMODEL_HXX

#include "PhysicalModelImplementation.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API MetaModel : public PhysicalModelImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  MetaModel(const OT::String & name = "Unnamed");
  /** Constructor with parameters */
  MetaModel(const OT::String & name,
            const OT::Function & function);

  /** Virtual constructor */
  MetaModel * clone() const override;

  void setFunction(const OT::Function & function);

  OT::String getPythonScript() const override;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  OT::Function generateFunction(const OT::Description & outputNames) const override;

private:
  OT::Function function_;
};

}
#endif
