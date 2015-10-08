#include "ParametricCalculusItem.hxx"

namespace OTGUI {

ParametricCalculusItem::ParametricCalculusItem(const Calculus & calculus)
  : CalculusItem(calculus, QString("ParametricCalculus"))
  , calculus_(calculus)
{
}


ParametricCalculusItem::~ParametricCalculusItem()
{

}


void ParametricCalculusItem::updateCalculus(InputCollection inputs)
{
  dynamic_cast<ParametricCalculus*>(&*calculus_.getImplementation())->computeParameters(inputs);
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