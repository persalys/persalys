// ParametricAnalysis.cxx

#include "otgui/ParametricAnalysis.hxx"
#include "Box.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(ParametricAnalysis);

ParametricAnalysis::ParametricAnalysis(const std::string & name, const PhysicalModel & physicalModel)
 : SimulationAnalysis(name, physicalModel, 0)
{
  initializeParameters(physicalModel.getInputs());
}


ParametricAnalysis::ParametricAnalysis(const std::string & name, const PhysicalModel & physicalModel,
                         const NumericalPoint & infBounds, const NumericalPoint & supBounds,
                         const Indices & nbValues)
 : SimulationAnalysis(name, physicalModel, 0)
 , inputNames_(getPhysicalModel().getInputNames())
 , infBounds_(infBounds)
 , supBounds_(supBounds)
 , nbValues_(nbValues)
{
}


ParametricAnalysis::ParametricAnalysis(const ParametricAnalysis & other)
 : SimulationAnalysis(other)
 , inputNames_(other.inputNames_)
 , infBounds_(other.infBounds_)
 , supBounds_(other.supBounds_)
 , nbValues_(other.nbValues_)
{
}


ParametricAnalysis* ParametricAnalysis::clone() const
{
  return new ParametricAnalysis(*this);
}


void ParametricAnalysis::initializeParameters(const InputCollection & inputs)
{
  inputSample_.clear();
  inputNames_ = getPhysicalModel().getInputNames();

  int inputSize = inputs.getSize();
  infBounds_ = NumericalPoint(inputSize);
  supBounds_ = NumericalPoint(inputSize);
  nbValues_ = Indices(inputSize);

  for (int i=0; i<inputSize; ++i)
  {
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


void ParametricAnalysis::updateParameters()
{
  Description inputNames(inputNames_);
  NumericalPoint infBounds(infBounds_);
  NumericalPoint supBounds(supBounds_);
  Indices nbValues(nbValues_);

  initializeParameters(getPhysicalModel().getInputs());

  for (int i=0; i<inputNames.getSize(); ++i)
  {
    const Description::const_iterator it = std::find(inputNames.begin(), inputNames.end(), inputNames_[i]);
    if (it != inputNames.end())
    {
      infBounds_[i] = infBounds[it - inputNames.begin()];
      supBounds_[i] = supBounds[it - inputNames.begin()];
      nbValues_[i] = nbValues[it - inputNames.begin()];
    }
  }
}


void ParametricAnalysis::run()
{
  // output = f(input)
  result_ = ParametricAnalysisResult(getInputSample(), getOutputSample(getInputSample()));
  notify("analysisFinished");
}


NumericalSample ParametricAnalysis::getInputSample()
{
  if (!inputSample_.getSize())
  {
    inputSample_.clear();
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
    inputSample_.setDescription(inputNames_);
  }
  return inputSample_;
}


void ParametricAnalysis::setInputSample(const NumericalSample & inputSample)
{
  inputSample_ = inputSample;
  notify("inputSampleChanged");
}


NumericalPoint ParametricAnalysis::getInfBounds() const
{
  return infBounds_;
}


void ParametricAnalysis::setInfBound(const int & index, const double & infBounds)
{
  infBounds_[index] = infBounds;
}


NumericalPoint ParametricAnalysis::getSupBounds() const
{
  return supBounds_;
}


void ParametricAnalysis::setSupBound(const int & index, const double & supBounds)
{
  supBounds_[index] = supBounds;
}


Indices ParametricAnalysis::getNbValues() const
{
  return nbValues_;
}


void ParametricAnalysis::setNbValues(const int & index, const int & nbValues)
{
  nbValues_[index] = nbValues;
}


ParametricAnalysisResult ParametricAnalysis::getResult() const
{
  return result_;
}


std::string ParametricAnalysis::dump() const
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
  result += getName()+ " = otguibase.ParametricAnalysis('" + getName() + "', " + getPhysicalModel().getName();
  result += ", infBounds, supBounds, nbValues)\n";
  return result;
}


bool ParametricAnalysis::analysisLaunched() const
{
  return getResult().getOutputSample().getSize()!=0;
}


}