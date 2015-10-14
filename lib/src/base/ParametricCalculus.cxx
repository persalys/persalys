// ParametricCalculus.cxx

#include "ParametricCalculus.hxx"
#include "Box.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(ParametricCalculus);

ParametricCalculus::ParametricCalculus(const std::string & name, const PhysicalModel & physicalModel)
 : CalculusImplementation(name, physicalModel)
{
  computeParameters(physicalModel.getInputs());
}


ParametricCalculus::ParametricCalculus(const std::string & name, const PhysicalModel & physicalModel,
                         const NumericalPoint & infBounds, const NumericalPoint & supBounds,
                         const Indices & nbValues)
 : CalculusImplementation(name, physicalModel)
 , infBounds_(infBounds)
 , supBounds_(supBounds)
 , nbValues_(nbValues)
{
  int inputSize = physicalModel.getInputs().getSize();
  inputNames_ = Description(inputSize);

  for (int i=0; i<inputSize; ++i)
  {
    inputNames_[i] = physicalModel.getInputs()[i].getName();
  }
}


ParametricCalculus::ParametricCalculus(const ParametricCalculus & other)
 : CalculusImplementation(other)
 , inputNames_(other.inputNames_)
 , infBounds_(other.infBounds_)
 , supBounds_(other.supBounds_)
 , nbValues_(other.nbValues_)
{
}


ParametricCalculus* ParametricCalculus::clone() const
{
  return new ParametricCalculus(*this);
}


void ParametricCalculus::computeParameters(const InputCollection & inputs)
{
  int inputSize = inputs.getSize();
  infBounds_ = NumericalPoint(inputSize);
  supBounds_ = NumericalPoint(inputSize);
  nbValues_ = Indices(inputSize);
  inputNames_ = Description(inputSize);

  for (int i=0; i<inputSize; ++i)
  {
    inputNames_[i] = inputs[i].getName();
    if (inputs[i].getDistribution().getImplementation()->getClassName()=="Dirac")
    {
      infBounds_[i] = inputs[i].getValue();
      supBounds_[i] = inputs[i].getValue();
      nbValues_[i] = 1;
    }
    else
    {
      infBounds_[i] = inputs[i].getDistribution().computeQuantile(0.05)[0];
      supBounds_[i] = inputs[i].getDistribution().computeQuantile(0.95)[0];
      nbValues_[i] = 2;
  //     or truncation
    }
  }
}


void ParametricCalculus::updateParameters()
{
  int inputSize = getPhysicalModel().getInputs().getSize();
  Description inputNames(inputNames_);
  inputNames_ = Description(inputSize);

  NumericalPoint infBounds(infBounds_);
  NumericalPoint supBounds(supBounds_);
  Indices nbValues(nbValues_);

  computeParameters(getPhysicalModel().getInputs());

  for (int i=0; i<inputSize; ++i)
  {
    inputNames_[i] = getPhysicalModel().getInputs()[i].getName();
    const Description::const_iterator it = std::find(inputNames.begin(), inputNames.end(), inputNames_[i]);
    if (it != inputNames.end())
    {
      infBounds_[i] = infBounds[it - inputNames.begin()];
      supBounds_[i] = supBounds[it - inputNames.begin()];
      nbValues_[i] = nbValues[it - inputNames.begin()];
    }
  }
}


void ParametricCalculus::computeInputSample()
{
  inputSample_ = NumericalSample(0, 0);
  NumericalPoint scale(0);
  NumericalPoint transvec(0);
  NumericalPoint levels(0);

  for (int i=0; i<infBounds_.getSize(); ++i)
  {
    //TODO: improve this part if a variable is constant
    double inf = infBounds_[i];
    double sup = supBounds_[i];
    scale.add(sup - inf);
    transvec.add(inf);
    if (nbValues_[i]>1)
      levels.add(nbValues_[i]-2);
    else
      levels.add(0);
  }

  if (levels.getSize())
  {
    Box box = Box(levels);

    inputSample_ = box.generate();
    inputSample_*=scale;
    inputSample_+=transvec;
  }
}


void ParametricCalculus::run()
{
  // output = f(input)
  NumericalMathFunction model = getPhysicalModel().getFunction();

  computeInputSample();

  inputSample_.setDescription(model.getInputDescription());

  NumericalSample outputSample = model(inputSample_);
  result_ = ParametricCalculusResult(outputSample, inputSample_);

  notify("calculusFinished");
}


NumericalSample ParametricCalculus::getInputSample() const
{
  return inputSample_;
}


void ParametricCalculus::setInputSample(const NumericalSample & inputSample)
{
  inputSample_ = inputSample;
  notify("inputSampleChanged");
}


NumericalPoint ParametricCalculus::getInfBounds() const
{
  return infBounds_;
}


void ParametricCalculus::setInfBound(const int & index, const double & infBounds)
{
  infBounds_[index] = infBounds;
}


NumericalPoint ParametricCalculus::getSupBounds() const
{
  return supBounds_;
}


void ParametricCalculus::setSupBound(const int & index, const double & supBounds)
{
  supBounds_[index] = supBounds;
}


Indices ParametricCalculus::getNbValues() const
{
  return nbValues_;
}


void ParametricCalculus::setNbValues(const int & index, const int & nbValues)
{
  nbValues_[index] = nbValues;
}


ParametricCalculusResult ParametricCalculus::getResult() const
{
  return result_;
}


std::string ParametricCalculus::dump() const
{
  std::string result;
  OSS oss;
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


bool ParametricCalculus::calculusLaunched() const
{
  return getResult().getResultSample().getSize()!=0;
}


}