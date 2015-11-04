// QuadraticCumulCalculus.cxx

#include "otgui/QuadraticCumulCalculus.hxx"
#include "QuadraticCumul.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(QuadraticCumulCalculus);

QuadraticCumulCalculus::QuadraticCumulCalculus(const std::string & name, const PhysicalModel & physicalModel)
 : CalculusImplementation(name, physicalModel)
 , outputs_(physicalModel.getOutputs())
 , result_()
{
//TODO ctr with outputNames (pas OutputCollection!) optionnel par défaut prendrait tous les outputs
}


QuadraticCumulCalculus::QuadraticCumulCalculus(const QuadraticCumulCalculus & other)
 : CalculusImplementation(other)
 , outputs_(other.outputs_)
 , result_(other.result_)
{
}


QuadraticCumulCalculus* QuadraticCumulCalculus::clone() const
{
  return new QuadraticCumulCalculus(*this);
}


OutputCollection QuadraticCumulCalculus::getOutputs() const
{
  return outputs_;
}


void QuadraticCumulCalculus::setOutputs(const OutputCollection & outputs)
{
  outputs_ = outputs;
}


void QuadraticCumulCalculus::run()
{
  QuadraticCumul algoQuadraticCumul(getPhysicalModel().getOutputRandomVector(outputs_));
  // set results
  result_ = QuadraticCumulResult(algoQuadraticCumul, outputs_);

  notify("calculusFinished");
}


QuadraticCumulResult QuadraticCumulCalculus::getResult() const
{
  return result_;
}


std::string QuadraticCumulCalculus::dump() const
{
  std::string result;
  result += getName()+ " = otguibase.QuadraticCumulCalculus('" + getName() + "', " + getPhysicalModel().getName() + ")\n";
  return result;
}


bool QuadraticCumulCalculus::calculusLaunched() const
{
//   return resultSample_.getSize()!=0;
}


}