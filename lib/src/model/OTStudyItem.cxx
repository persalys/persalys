#include "otgui/OTStudyItem.hxx"

#include "otgui/ParametricAnalysisItem.hxx"
#include "otgui/CentralTendencyItem.hxx"
#include "otgui/SensitivityAnalysisItem.hxx"

namespace OTGUI {

OTStudyItem::OTStudyItem(OTStudy * otStudy)
  : ObserverItem(otStudy->getName().c_str(), QString("OTStudy"))
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
    Item * item = new Item(tr("Deterministic study"), QString("DeterministicStudy"));
    item->setEditable(false);
    newPhysicalModelItem->appendRow(item);
    item = new Item(tr("Probabilistic study"), QString("ProbabilisticStudy"));
    item->setEditable(false);
    newPhysicalModelItem->appendRow(item);

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
    addAnalysisItem(addedCentralTendency, newItem, false);
  }
  else if (message=="addSobolAnalysis" || message=="addSRCAnalysis")
  {
    Analysis addedSensitivityAnalysis = otStudy_->getAnalyses().back();
    SensitivityAnalysisItem * newItem = new SensitivityAnalysisItem(addedSensitivityAnalysis);
    addAnalysisItem(addedSensitivityAnalysis, newItem, false);
  }
}


void OTStudyItem::addAnalysisItem(Analysis & analysis, AnalysisItem * item, bool deterministic)
{
  analysis.addObserver(item);
  for (int i=0; i<rowCount(); ++i)
    if (child(i)->text().toStdString() == analysis.getPhysicalModel().getName())
    {
      if (deterministic)
        child(i)->child(0)->appendRow(item);
      else
        child(i)->child(1)->appendRow(item);
      break;
    }
  emit newAnalysisItemCreated(item);
}


void OTStudyItem::setData(const QVariant & value, int role)
{
  switch (role)
  {
    case Qt::EditRole:
      otStudy_->setName(value.toString().toStdString());
      ObserverItem::setData(value, role);
      break;
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