#include "otgui/StudyItem.hxx"

#include "otgui/ParametricCalculusItem.hxx"
#include "otgui/DistributionAnalysisItem.hxx"
#include "otgui/SensitivityAnalysisItem.hxx"

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
  if (message=="addPhysicalModel")
  {
    PhysicalModel addedPhysicalModel = study_->getPhysicalModels().back();
    PhysicalModelItem * newPhysicalModelItem = new PhysicalModelItem(addedPhysicalModel);
    addedPhysicalModel.addObserver(newPhysicalModelItem);
    appendRow(newPhysicalModelItem);

    emit newPhysicalModelItemCreated(newPhysicalModelItem);
  }
  else if (message=="addParametricCalculus")
  {
    Calculus addedParametricCalculus = study_->getCalculuses().back();
    ParametricCalculusItem * newItem = new ParametricCalculusItem(addedParametricCalculus);
    addCalculusItem(addedParametricCalculus, newItem);
  }
  else if (message=="addMonteCarloCalculus" || message=="addQuadraticCumulCalculus")
  {
    Calculus addedDistributionAnalysis = study_->getCalculuses().back();
    DistributionAnalysisItem * newItem = new DistributionAnalysisItem(addedDistributionAnalysis);
    addCalculusItem(addedDistributionAnalysis, newItem);
  }
  else if (message=="addSobolCalculus" || message=="addSRCCalculus")
  {
    Calculus addedSensitivityAnalysis = study_->getCalculuses().back();
    SensitivityAnalysisItem * newItem = new SensitivityAnalysisItem(addedSensitivityAnalysis);
    addCalculusItem(addedSensitivityAnalysis, newItem);
  }
}


void StudyItem::addCalculusItem(Calculus & calculus, CalculusItem * item)
{
  calculus.addObserver(item);
  for (int i=0; i<rowCount(); ++i)
    if (child(i)->text().toStdString() == calculus.getPhysicalModel().getName())
    {
      child(i)->appendRow(item);
      break;
    }
  emit newCalculusItemCreated(item);
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