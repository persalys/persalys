// ParametricCalculus.cxx

#include "ParametricCalculus.hxx"
#include "Box.hxx"

namespace OTGUI {

CLASSNAMEINIT(ParametricCalculus);

ParametricCalculus::ParametricCalculus(const std::string & name, const PhysicalModel & physicalModel)
 : CalculusImplementation(name, physicalModel)
{
  computeParameters(physicalModel.getInputs());
}


ParametricCalculus::ParametricCalculus(const std::string & name, const PhysicalModel & physicalModel,
                         const OT::NumericalPoint & infBounds, const OT::NumericalPoint & supBounds,
                         const OT::Indices & nbValues)
 : CalculusImplementation(name, physicalModel)
 , infBounds_(infBounds)
 , supBounds_(supBounds)
 , nbValues_(nbValues)
{
  int inputSize = physicalModel.getInputs().getSize();
  inputNames_ = OT::Description(inputSize);

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


ParametricCalculus::~ParametricCalculus()
{
}


void ParametricCalculus::computeParameters(const InputCollection & inputs)
{
  int inputSize = inputs.getSize();
  infBounds_ = OT::NumericalPoint(inputSize);
  supBounds_ = OT::NumericalPoint(inputSize);
  nbValues_ = OT::Indices(inputSize);
  inputNames_ = OT::Description(inputSize);

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
  OT::Description inputNames(inputNames_);
  inputNames_ = OT::Description(inputSize);

  OT::NumericalPoint infBounds(infBounds_);
  OT::NumericalPoint supBounds(supBounds_);
  OT::Indices nbValues(nbValues_);

  computeParameters(getPhysicalModel().getInputs());

  for (int i=0; i<inputSize; ++i)
  {
    inputNames_[i] = getPhysicalModel().getInputs()[i].getName();
    const OT::Description::const_iterator it = std::find(inputNames.begin(), inputNames.end(), inputNames_[i]);
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
  inputSample_ = OT::NumericalSample(0, 0);
  OT::NumericalPoint scale(0);
  OT::NumericalPoint transvec(0);
  OT::NumericalPoint levels(0);

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
    OT::Box box = OT::Box(levels);

    inputSample_ = box.generate();
    inputSample_*=scale;
    inputSample_+=transvec;
  }
}


void ParametricCalculus::run()
{
  // output = f(input)
  OT::NumericalMathFunction model = getPhysicalModel().getFunction();

  computeInputSample();

  inputSample_.setDescription(model.getInputDescription());

  resultSample_ = model(inputSample_);

  notify("calculusFinished");
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


bool ParametricCalculus::calculusLaunched() const
{
  return resultSample_.getSize()!=0;
}


}