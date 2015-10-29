#include "ParametricCalculusItem.hxx"

namespace OTGUI {

ParametricCalculusItem::ParametricCalculusItem(const Calculus & calculus)
  : CalculusItem(calculus, QString("ParametricCalculus"))
{
}


void ParametricCalculusItem::update(Observable* source, const std::string & message)
{
  if (message=="inputSampleChanged")
  {
    emit inputSampleChanged(dynamic_cast<ParametricCalculus*>(&*getCalculus().getImplementation())->getInputSample());
  }
  else if (message=="calculusFinished")
  {
    emit calculusFinished(this);
  }
}

}