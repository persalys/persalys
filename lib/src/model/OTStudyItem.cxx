#include "otgui/OTStudyItem.hxx"

#include "otgui/ParametricAnalysisItem.hxx"
#include "otgui/CentralTendencyItem.hxx"
#include "otgui/SensitivityAnalysisItem.hxx"
#include "otgui/ReliabilityAnalysisItem.hxx"

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

    if (addedPhysicalModel.hasStochasticInputs())
    {
      ProbabilisticModelItem * newProbabilisticModelItem = new ProbabilisticModelItem(addedPhysicalModel);
      addedPhysicalModel.addObserver(newProbabilisticModelItem);
      item->appendRow(newProbabilisticModelItem);
      emit newProbabilisticModelItemCreated(newProbabilisticModelItem);
    }

    emit newPhysicalModelItemCreated(newPhysicalModelItem);
  }
  else if (message=="addLimitState")
  {
    LimitState addedLimitState = otStudy_->getLimitStates().back();

    LimitStateItem * newItem = new LimitStateItem(addedLimitState);
    addedLimitState.addObserver(newItem);
    addedLimitState.getPhysicalModel().addObserver(newItem);
    for (int i=0; i<rowCount(); ++i)
      if (child(i)->text().toStdString() == addedLimitState.getPhysicalModel().getName())
      {
        if (!addedLimitState.getPhysicalModel().hasStochasticInputs() && !child(i)->child(1)->hasChildren())
        {
          ProbabilisticModelItem * newProbabilisticModelItem = new ProbabilisticModelItem(addedLimitState.getPhysicalModel());
          addedLimitState.getPhysicalModel().addObserver(newProbabilisticModelItem);
          child(i)->child(1)->appendRow(newProbabilisticModelItem);
          emit newProbabilisticModelItemCreated(newProbabilisticModelItem);
        }
        child(i)->child(1)->appendRow(newItem);
        break;
      }
    emit newLimitStateItemCreated(newItem);
  }
  else
  {
    Analysis addedAnalysis = otStudy_->getAnalyses().back();

    if (message=="addParametricAnalysis")
    {
      ParametricAnalysisItem * newItem = new ParametricAnalysisItem(addedAnalysis);
      addDeterministicAnalysisItem(addedAnalysis, newItem);
    }
    else if (message=="addMonteCarloAnalysis" || message=="addQuadraticCumulAnalysis")
    {
      CentralTendencyItem * newItem = new CentralTendencyItem(addedAnalysis);
      addProbabilisticAnalysisItem(addedAnalysis, newItem);
    }
    else if (message=="addSobolAnalysis" || message=="addSRCAnalysis")
    {
      SensitivityAnalysisItem * newItem = new SensitivityAnalysisItem(addedAnalysis);
      addProbabilisticAnalysisItem(addedAnalysis, newItem);
    }
    else if (message=="addMonteCarloReliabilityAnalysis")
    {
      ReliabilityAnalysisItem * newItem = new ReliabilityAnalysisItem(addedAnalysis);
      addProbabilisticAnalysisItem(addedAnalysis, newItem);
    }
  }
}


void OTStudyItem::addDeterministicAnalysisItem(Analysis & analysis, AnalysisItem * item)
{
  analysis.addObserver(item);
  for (int i=0; i<rowCount(); ++i)
    if (child(i)->text().toStdString() == analysis.getPhysicalModel().getName())
    {
      child(i)->child(0)->appendRow(item);
      break;
    }
  emit newAnalysisItemCreated(item);
}

void OTStudyItem::addProbabilisticAnalysisItem(Analysis & analysis, AnalysisItem * item)
{
  analysis.addObserver(item);
  for (int i=0; i<rowCount(); ++i)
    if (child(i)->text().toStdString() == analysis.getPhysicalModel().getName())
    {
      if (!child(i)->child(1)->hasChildren())
      {
        ProbabilisticModelItem * newProbabilisticModelItem = new ProbabilisticModelItem(analysis.getPhysicalModel());
        analysis.getPhysicalModel().addObserver(newProbabilisticModelItem);
        child(i)->child(1)->appendRow(newProbabilisticModelItem);
        emit newProbabilisticModelItemCreated(newProbabilisticModelItem);
      }
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
  return otStudy_->dump().c_str();
}



}