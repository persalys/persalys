#include "otgui/DesignOfExperimentItem.hxx"

using namespace OT;

namespace OTGUI {

DesignOfExperimentItem::DesignOfExperimentItem(const DesignOfExperiment & designOfExperiment)
  : ObserverItem(designOfExperiment.getName().c_str(), QString("DesignOfExperiment"))
  , designOfExperiment_(designOfExperiment)
{
}


DesignOfExperimentItem::~DesignOfExperimentItem()
{
}


void DesignOfExperimentItem::setData(const QVariant & value, int role)
{
  switch (role)
  {
    case Qt::EditRole:
      designOfExperiment_.setName(value.toString().toStdString());
      ObserverItem::setData(value, role);
      break;
  }
}


DesignOfExperiment DesignOfExperimentItem::getDesignOfExperiment() const
{
  return designOfExperiment_;
}


void DesignOfExperimentItem::update(Observable* source, const String & message)
{
  if (message=="analysisFinished")
  {
    emit analysisFinished();
  }
}
}