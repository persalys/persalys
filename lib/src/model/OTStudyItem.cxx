#include "otgui/OTStudyItem.hxx"

#include "otgui/ParametricAnalysisItem.hxx"
#include "otgui/CentralTendencyItem.hxx"
#include "otgui/SensitivityAnalysisItem.hxx"

namespace OTGUI {

OTStudyItem::OTStudyItem(OTStudy * otStudy)
  : Item(otStudy->getName(), QString("OTStudy"))
  , otStudy_(otStudy)
{

}


OTStudyItem::~OTStudyItem()
{

}


void OTStudyItem::update(Observable * source, const std::string & message)
{
  if (message=="addPhysicalModel")
  {
    PhysicalModel addedPhysicalModel = otStudy_->getPhysicalModels().back();
    PhysicalModelItem * newPhysicalModelItem = new PhysicalModelItem(addedPhysicalModel);
    addedPhysicalModel.addObserver(newPhysicalModelItem);
    appendRow(newPhysicalModelItem);

    emit newPhysicalModelItemCreated(newPhysicalModelItem);
  }
  else if (message=="addParametricAnalysis")
  {
    Analysis addedParametricAnalysis = otStudy_->getAnalyses().back();
    ParametricAnalysisItem * newItem = new ParametricAnalysisItem(addedParametricAnalysis);
    addAnalysisItem(addedParametricAnalysis, newItem);
  }
  else if (message=="addMonteCarloAnalysis" || message=="addQuadraticCumulAnalysis")
  {
    Analysis addedCentralTendency = otStudy_->getAnalyses().back();
    CentralTendencyItem * newItem = new CentralTendencyItem(addedCentralTendency);
    addAnalysisItem(addedCentralTendency, newItem);
  }
  else if (message=="addSobolAnalysis" || message=="addSRCAnalysis")
  {
    Analysis addedSensitivityAnalysis = otStudy_->getAnalyses().back();
    SensitivityAnalysisItem * newItem = new SensitivityAnalysisItem(addedSensitivityAnalysis);
    addAnalysisItem(addedSensitivityAnalysis, newItem);
  }
}


void OTStudyItem::addAnalysisItem(Analysis & analysis, AnalysisItem * item)
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


void OTStudyItem::setData(const QVariant & value, int role)
{
  switch (role)
  {
    case Qt::EditRole:
      Item::setData(value, role);
  }
}


OTStudy* OTStudyItem::getOTStudy()
{
  return otStudy_;
}


QString OTStudyItem::dumpOTStudy()
{
  return QString::fromStdString(otStudy_->dump());
}



}