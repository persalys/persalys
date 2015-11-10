#include "otgui/StudyItem.hxx"

#include "otgui/ParametricAnalysisItem.hxx"
#include "otgui/CentralTendencyItem.hxx"
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
  else if (message=="addParametricAnalysis")
  {
    Analysis addedParametricAnalysis = study_->getAnalyses().back();
    ParametricAnalysisItem * newItem = new ParametricAnalysisItem(addedParametricAnalysis);
    addAnalysisItem(addedParametricAnalysis, newItem);
  }
  else if (message=="addMonteCarloAnalysis" || message=="addQuadraticCumulAnalysis")
  {
    Analysis addedCentralTendency = study_->getAnalyses().back();
    CentralTendencyItem * newItem = new CentralTendencyItem(addedCentralTendency);
    addAnalysisItem(addedCentralTendency, newItem);
  }
  else if (message=="addSobolAnalysis" || message=="addSRCAnalysis")
  {
    Analysis addedSensitivityAnalysis = study_->getAnalyses().back();
    SensitivityAnalysisItem * newItem = new SensitivityAnalysisItem(addedSensitivityAnalysis);
    addAnalysisItem(addedSensitivityAnalysis, newItem);
  }
}


void StudyItem::addAnalysisItem(Analysis & analysis, AnalysisItem * item)
{
  analysis.addObserver(item);
  for (int i=0; i<rowCount(); ++i)
    if (child(i)->text().toStdString() == analysis.getPhysicalModel().getName())
    {
      child(i)->appendRow(item);
      break;
    }
  emit newAnalysisItemCreated(item);
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