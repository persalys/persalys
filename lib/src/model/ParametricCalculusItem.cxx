#include "ParametricCalculusItem.hxx"

namespace OTGUI {

ParametricCalculusItem::ParametricCalculusItem(ParametricCalculus * calculus)
  : CalculusItem(calculus, QString("ParametricCalculus"))
{
}


ParametricCalculusItem::~ParametricCalculusItem()
{

}


void ParametricCalculusItem::update(Observable* source, const std::string & message)
{
  if (message=="inputSampleChanged")
  {
    emit inputSampleChanged(getCalculus<ParametricCalculus>()->getInputSample());
  }
  else if (message=="calculusFinished")
  {
    emit calculusFinished(this);
  }
}

}