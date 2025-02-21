//                                               -*- C++ -*-
/**
 *  @brief Evaluates the physical models
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#ifndef PERSALYS_FIELDMODELEVALUATION_HXX
#define PERSALYS_FIELDMODELEVALUATION_HXX

#include "ModelEvaluation.hxx"

#include <openturns/ProcessSample.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API FieldModelEvaluation : public ModelEvaluation
{
  CLASSNAME

public:
  /** Default constructor */
  FieldModelEvaluation();
  /** Constructor with parameters */
  FieldModelEvaluation(const OT::String & name, const PhysicalModel & physicalModel);
  FieldModelEvaluation(const OT::String & name, const PhysicalModel & physicalModel,
                       const OT::Point & inputsValues);

  /** Virtual constructor */
  FieldModelEvaluation * clone() const override;

  OT::ProcessSample getProcessSample() const;

  bool hasValidResult() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  void launch() override;

private:
  OT::ProcessSample processSample_;
};
}
#endif
