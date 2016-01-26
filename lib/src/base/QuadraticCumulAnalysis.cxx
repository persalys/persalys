// QuadraticCumulAnalysis.cxx

#include "otgui/QuadraticCumulAnalysis.hxx"
#include "QuadraticCumul.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(QuadraticCumulAnalysis);

QuadraticCumulAnalysis::QuadraticCumulAnalysis(const std::string & name, const PhysicalModel & physicalModel)
  : AnalysisImplementation(name, physicalModel)
  , outputs_(physicalModel.getOutputs())
{
//TODO ctr with outputNames (pas OutputCollection!) optionnel par défaut prendrait tous les outputs
}


QuadraticCumulAnalysis::QuadraticCumulAnalysis(const QuadraticCumulAnalysis & other)
  : AnalysisImplementation(other)
  , outputs_(other.outputs_)
  , result_(other.result_)
{
}


QuadraticCumulAnalysis* QuadraticCumulAnalysis::clone() const
{
  return new QuadraticCumulAnalysis(*this);
}


OutputCollection QuadraticCumulAnalysis::getOutputs() const
{
  return outputs_;
}


void QuadraticCumulAnalysis::setOutputs(const OutputCollection & outputs)
{
  outputs_ = outputs;
}


void QuadraticCumulAnalysis::run()
{
//   TODO: cf CTR
  Description outputNames = Description(outputs_.getSize());
  for (int i=0; i<outputs_.getSize(); ++i)
    outputNames[i] = outputs_[i].getName();

  QuadraticCumul algoQuadraticCumul(getPhysicalModel().getOutputRandomVector(outputNames));

  // set results
  result_ = QuadraticCumulResult(algoQuadraticCumul, outputs_);

  notify("analysisFinished");
}


QuadraticCumulResult QuadraticCumulAnalysis::getResult() const
{
  return result_;
}


std::string QuadraticCumulAnalysis::dump() const
{
  std::string result;
  result += getName()+ " = otguibase.QuadraticCumulAnalysis('" + getName() + "', " + getPhysicalModel().getName() + ")\n";
  return result;
}


bool QuadraticCumulAnalysis::analysisLaunched() const
{
//   return outputSample_.getSize()!=0;
}
}