// ParametricCalculus.cxx

#include "ParametricCalculus.hxx"
#include "Box.hxx"

namespace OTGUI {

ParametricCalculus::ParametricCalculus(const std::string & name, const PhysicalModel & physicalModel)
 : Calculus(name, physicalModel)
{
  computeParameters(physicalModel.getInputs());
}


ParametricCalculus::ParametricCalculus(const std::string & name, const PhysicalModel & physicalModel,
                         const OT::NumericalPoint & infBounds, const OT::NumericalPoint & supBounds,
                         const OT::Indices & nbValues)
 : Calculus(name, physicalModel)
 , infBounds_(infBounds)
 , supBounds_(supBounds)
 , nbValues_(nbValues)
{

}


ParametricCalculus::ParametricCalculus(const ParametricCalculus & other)
 : Calculus(other)
 , infBounds_(other.infBounds_)
 , supBounds_(other.supBounds_)
 , nbValues_(other.nbValues_)
{
}


ParametricCalculus* ParametricCalculus::clone() const
{
  return new ParametricCalculus(*this);
}


ParametricCalculus::~ParametricCalculus()
{
}


void ParametricCalculus::computeParameters(const InputCollection & inputs)
{
  for (int i=0; i<inputs.getSize(); ++i)
  {
    if (inputs[i].getDistribution().getImplementation()->getClassName()=="Dirac")
    {
      infBounds_.add(inputs[i].getValue());
      supBounds_.add(inputs[i].getValue());
      nbValues_.add(2);
    }
    else
    {
      infBounds_.add(inputs[i].getDistribution().computeQuantile(0.05));
      supBounds_.add(inputs[i].getDistribution().computeQuantile(0.95));
      nbValues_.add(2);
  //     or truncation
    }
  }
}


void ParametricCalculus::computeInputSample()
{
  OT::NumericalPoint scale(0);
  OT::NumericalPoint transvec(0);
  OT::NumericalPoint levels(0);

  for (int i=0; i<infBounds_.getSize(); ++i)
  {
    if (nbValues_[i])
    {
      double inf = infBounds_[i];
      double sup = supBounds_[i];
      scale.add(sup - inf);
      transvec.add(inf);
      levels.add(nbValues_[i]-2);
    }
  }

  OT::Box box = OT::Box(levels);

  inputSample_ = box.generate();
  inputSample_*=scale;
  inputSample_+=transvec;

  if (inputSample_.getDimension() != infBounds_.getSize())
    for (int i=0; i<infBounds_.getSize(); ++i)
      if (!nbValues_[i])
        inputSample_.stack(OT::NumericalSample(inputSample_.getSize(), OT::NumericalPoint(1, infBounds_[i])));
}


void ParametricCalculus::run()
{
  // output = f(input)
  OT::NumericalMathFunction model = getPhysicalModel().getFunction();

  computeInputSample();

  inputSample_.setDescription(model.getInputDescription());

  resultSample_ = model(inputSample_);
}


OT::NumericalSample ParametricCalculus::getInputSample() const
{
  return inputSample_;
}


void ParametricCalculus::setInputSample(const OT::NumericalSample & inputSample)
{
  inputSample_ = inputSample;
  notify("inputSampleChanged");
}


OT::NumericalSample ParametricCalculus::getResultSample() const
{
  return resultSample_;
}


OT::NumericalPoint ParametricCalculus::getInfBounds() const
{
  return infBounds_;
}


void ParametricCalculus::setInfBound(const int & index, const double & infBounds)
{
  infBounds_[index] = infBounds;
}


OT::NumericalPoint ParametricCalculus::getSupBounds() const
{
  return supBounds_;
}


void ParametricCalculus::setSupBound(const int & index, const double & supBounds)
{
  supBounds_[index] = supBounds;
}


OT::Indices ParametricCalculus::getNbValues() const
{
  return nbValues_;
}


void ParametricCalculus::setNbValues(const int & index, const int & nbValues)
{
  nbValues_[index] = nbValues;
}


std::string ParametricCalculus::dump() const
{
  std::string result;
  OT::OSS oss;
  oss << "infBounds = ot.NumericalPoint(" << infBounds_.getSize() << ")\n";
  oss << "supBounds = ot.NumericalPoint(" << supBounds_.getSize() << ")\n";
  oss << "nbValues = ot.Indices(" << nbValues_.getSize() << ")\n";

  for (int i=0; i<infBounds_.getSize(); ++i)
  {
    oss << "infBounds[" << i << "] = " << infBounds_[i] << "\n";
    oss << "supBounds[" << i << "] = " << supBounds_[i] << "\n";
    oss << "nbValues[" << i << "] = " << nbValues_[i] << "\n";
  }
  result += oss.str();
  result += getName()+ " = otguibase.ParametricCalculus('" + getName() + "', " + getPhysicalModel().getName();
  result += ", infBounds, supBounds, nbValues)\n";
  return result;
}

}