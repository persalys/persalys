#include "otgui/ProbabilisticModelItem.hxx"

namespace OTGUI {

ProbabilisticModelItem::ProbabilisticModelItem(const PhysicalModel & physicalModel)
  : ObserverItem(QString("Probabilistic Model"), QString("ProbabilisticModel"))
  , physicalModel_(physicalModel)
{
}


ProbabilisticModelItem::~ProbabilisticModelItem()
{
}


PhysicalModel ProbabilisticModelItem::getPhysicalModel() const
{
  return physicalModel_;
}


void ProbabilisticModelItem::update(Observable* source, const std::string & message)
{
  if (message=="updateProbabilisticModelWindow")
  {
    emit inputChanged();
  }
}



}