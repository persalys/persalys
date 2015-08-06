#include "ParametricCalculusItem.hxx"

namespace OTGUI {

ParametricCalculusItem::ParametricCalculusItem(ParametricCalculus * calculus)
  : CalculusItem(calculus, QString("ParametricCalculus"))
{
}


ParametricCalculusItem::~ParametricCalculusItem()
{

}


void ParametricCalculusItem::setData(const QVariant & value, int role)
{
  switch (role)
  {
    case Qt::EditRole:
      calculus_->setName(value.toString().toStdString());
      Item::setData(value, role);
  }
}


void ParametricCalculusItem::setCalculus(ParametricCalculus * calculus)
{
  calculus_ = calculus;
}


void ParametricCalculusItem::runCalculus()
{
  getCalculus<ParametricCalculus>()->run();
  emit calculusFinished();
}


OT::NumericalSample ParametricCalculusItem::getInputSample() const
{
  return getCalculus<ParametricCalculus>()->getInputSample();
}


OT::NumericalSample ParametricCalculusItem::getResultSample() const
{
  return getCalculus<ParametricCalculus>()->getResultSample();
}


void ParametricCalculusItem::update(Observable* source, const std::string & message)
{
  if (message=="inputSampleChanged")
  {
    emit inputSampleChanged(getCalculus<ParametricCalculus>()->getInputSample());
  }
}

}