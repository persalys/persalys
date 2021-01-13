//                                               -*- C++ -*-
/**
 *  @brief Defines physical models thanks analytical formulas and mesh
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
#ifndef PERSALYS_SYMBOLICFIELDMODEL_HXX
#define PERSALYS_SYMBOLICFIELDMODEL_HXX

#include "SymbolicPhysicalModel.hxx"

namespace PERSALYS
{
// customized PointToFieldFunctionImplementation
class VertexValuePointToFieldFunction : public OT::PointToFieldFunctionImplementation
{

public:
  /** Constructor with parameters */
  VertexValuePointToFieldFunction(const OT::Mesh& mesh, const OT::Function& f)
  : OT::PointToFieldFunctionImplementation(f.getInputDimension() - mesh.getDimension(), mesh, f.getOutputDimension())
  , f_(f)
  {
    setOutputDescription(f_.getOutputDescription());
  }

  /** Virtual constructor */
  virtual VertexValuePointToFieldFunction * clone() const
  {
    return new VertexValuePointToFieldFunction(*this);
  }

  /** Operator () */
  virtual OT::Sample operator() (const OT::Point & inP) const
  {
    OT::Sample inputValues(getOutputMesh().getVertices().getSize(), inP);
    inputValues.stack(getOutputMesh().getVertices());
    return f_(inputValues);
  }

  /** Operator () */
  OT::ProcessSample operator() (const OT::Sample & inS) const
  {
    // check
    const OT::UnsignedInteger inDim = getInputDimension();
    if (inS.getDimension() != inDim)
      throw OT::InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << inDim << ", got " << inS.getDimension();
    const OT::UnsignedInteger size = inS.getSize();
    if (size == 0)
      throw OT::InvalidArgumentException(HERE) << "Error: the given sample has a size of 0.";

    // Simple loop over the evaluation operator based on time series
    // The calls number is updated by these calls
    OT::ProcessSample outSample(getOutputMesh(), size, getOutputDimension());
    for (OT::UnsignedInteger i = 0; i < size; ++i)
      outSample[i] = operator()(inS[i]);

    return outSample;
  }

private:
  OT::Function f_;
};


class PERSALYS_BASE_API SymbolicFieldModel : public SymbolicPhysicalModel
{
  CLASSNAME

public:
  /** Default constructor */
  SymbolicFieldModel(const OT::String& name = "Unnamed");
  /** Constructor with parameters */
  SymbolicFieldModel(const OT::String& name,
                        const MeshModel& meshModel,
                        const InputCollection& inputs,
                        const OutputCollection& outputs,
                        const OT::Description& formulas);

  /** Virtual constructor */
  SymbolicFieldModel * clone() const override;

  void addInput(const Input& input) override;
  void setInputName(const OT::String & inputName, const OT::String & newName) override;

  OT::String getPythonScript() const override;

protected:
  OT::PointToFieldFunction generatePointToFieldFunction(const OT::Description & outputNames) const override;
};
}
#endif
