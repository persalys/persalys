#include "StudyItem.hxx"


namespace OTGUI {

StudyItem::StudyItem(OTStudy * study)
  : Item(study->getName(), QString("Study"))
  , study_(study)
{

}


StudyItem::~StudyItem()
{

}


void StudyItem::update(Observable * source, const std::string & message)
{
  OTStudy * study = static_cast<OTStudy*>(source);
  if (message=="addPhysicalModel")
  {
    PhysicalModel addedPhysicalModel = study->getPhysicalModels().back();
    PhysicalModelItem * newPhysicalModelItem = new PhysicalModelItem(addedPhysicalModel);
    addedPhysicalModel.addObserver(newPhysicalModelItem);
    appendRow(newPhysicalModelItem);

    emit newPhysicalModelItemCreated(newPhysicalModelItem);
  }
  else if (message=="addParametricCalculus")
  {
    Calculus addedParametricCalculus = study->getCalculuses().back();
    ParametricCalculusItem * newParametricCalculusItem = new ParametricCalculusItem(addedParametricCalculus);
    addedParametricCalculus.addObserver(newParametricCalculusItem);
    for (int i=0; i<rowCount(); ++i)
      if (child(i)->text().toStdString() == addedParametricCalculus.getPhysicalModel().getName())
      {
        child(i)->appendRow(newParametricCalculusItem);
        PhysicalModelItem * physicalModelItem = static_cast<PhysicalModelItem*>(child(i));
        connect(physicalModelItem, SIGNAL(inputChanged(InputCollection)), newParametricCalculusItem, SLOT(updateCalculus(InputCollection)));
      }

    emit newParametricCalculusItemCreated(newParametricCalculusItem);
  }
}


void StudyItem::setData(const QVariant & value, int role)
{
  switch (role)
  {
    case Qt::EditRole:
      Item::setData(value, role);
  }
}


OTStudy* StudyItem::getStudy()
{
  return study_;
}


QString StudyItem::dumpStudy()
{
  return QString::fromStdString(study_->dump());
}



}