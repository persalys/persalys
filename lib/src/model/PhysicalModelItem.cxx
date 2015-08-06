#include "PhysicalModelItem.hxx"

namespace OTGUI {

PhysicalModelItem::PhysicalModelItem(const PhysicalModel & physicalModel)
  : Item(physicalModel.getName(), QString("PhysicalModel"))
  , physicalModel_(physicalModel)
{
}


PhysicalModelItem::~PhysicalModelItem()
{
// delete all children (calculus) + physicalModel_
}


void PhysicalModelItem::setData(const QVariant & value, int role)
{
  switch (role)
  {
    case Qt::EditRole:
      physicalModel_.setName(value.toString().toStdString());
      Item::setData(value, role);
  }
}


void PhysicalModelItem::setPhysicalModelInputs(const InputCollection & inputs)
{
  physicalModel_.setInputs(inputs);
  bool inputsAreValid = physicalModel_.checkInputs();
}


void PhysicalModelItem::setPhysicalModelOutputs(const OutputCollection & outputs)
{
  physicalModel_.setOutputs(outputs);
  bool outputsAreValid = physicalModel_.checkOutputs();
}


PhysicalModel PhysicalModelItem::getPhysicalModel() const
{
  return physicalModel_;
}


void PhysicalModelItem::loadDataWithYACS(const QString & fileName)
{
#ifdef OTGUI_HAVE_YACS
  physicalModel_.loadDataWithYACS(fileName.toStdString());
#endif
}


void PhysicalModelItem::update(Observable* source, const std::string & message)
{
  PhysicalModelImplementation * physicalModelImpl = static_cast<PhysicalModelImplementation*>(source);
  if (message=="inputChanged")
  {
    emit inputChanged(physicalModelImpl->getInputs());
  }
  else if (message=="outputChanged")
  {
    emit outputChanged(physicalModelImpl->getOutputs());
  }
}



}