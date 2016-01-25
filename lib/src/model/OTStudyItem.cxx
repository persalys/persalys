#include "otgui/OTStudyItem.hxx"

#include "otgui/ModelEvaluationItem.hxx"
#include "otgui/CentralTendencyItem.hxx"
#include "otgui/SensitivityAnalysisItem.hxx"
#include "otgui/ReliabilityAnalysisItem.hxx"
#include "otgui/ReliabilityAnalysis.hxx"

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
    connect(newPhysicalModelItem, SIGNAL(physicalModelChanged(PhysicalModel)), this, SLOT(updatePhysicalModel(PhysicalModel)));
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
      connect(newPhysicalModelItem, SIGNAL(physicalModelChanged(PhysicalModel)), newProbabilisticModelItem, SLOT(updatePhysicalModel(PhysicalModel)));
      emit newProbabilisticModelItemCreated(newProbabilisticModelItem);
    }

    item = new Item(tr("Designs of experiment"), QString("DesignOfExperimentList"));
    item->setEditable(false);
    newPhysicalModelItem->appendRow(item);

    emit newPhysicalModelItemCreated(newPhysicalModelItem);
  }
  else if (message=="addDesignOfExperiment")
  {
    DesignOfExperiment addedDesignOfExperiment = otStudy_->getDesignOfExperiments().back();

    DesignOfExperimentItem * newItem = new DesignOfExperimentItem(addedDesignOfExperiment);
    addedDesignOfExperiment.addObserver(newItem);
    addedDesignOfExperiment.getPhysicalModel().addObserver(newItem);
    for (int i=0; i<rowCount(); ++i)
      if (child(i)->text().toStdString() == addedDesignOfExperiment.getPhysicalModel().getName())
      {
        child(i)->child(2)->appendRow(newItem);
        emit newDesignOfExperimentItemCreated(newItem);
        return;
      }
    std::cerr<<"No item added for the design of experiment named "<<addedDesignOfExperiment.getName()<<std::endl;
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
        emit newLimitStateItemCreated(newItem);
        return;
      }
    std::cerr<<"No item added for the limit state named "<<addedLimitState.getName()<<std::endl;
  }
  else
  {
    Analysis addedAnalysis = otStudy_->getAnalyses().back();

    AnalysisItem * newItem;
    if (message=="addModelEvaluation")
    {
      newItem = new ModelEvaluationItem(addedAnalysis);
      addDeterministicAnalysisItem(addedAnalysis, newItem);
    }
    else if (message=="addMonteCarloAnalysis" || message=="addQuadraticCumulAnalysis")
    {
      newItem = new CentralTendencyItem(addedAnalysis);
      addProbabilisticAnalysisItem(addedAnalysis, newItem);
    }
    else if (message=="addSobolAnalysis" || message=="addSRCAnalysis")
    {
      newItem = new SensitivityAnalysisItem(addedAnalysis);
      addProbabilisticAnalysisItem(addedAnalysis, newItem);
    }
    else if (message=="addMonteCarloReliabilityAnalysis")
    {
      newItem = new ReliabilityAnalysisItem(addedAnalysis);
      addReliabilityAnalysisItem(addedAnalysis, newItem);
    }
    else
    {
      throw OT::InvalidArgumentException(HERE) << "In OTStudyItem::update: Impossible to add an item.\n";
    }
    connect(newItem, SIGNAL(analysisChanged(Analysis)), this, SLOT(updateAnalysis(Analysis)));
  }
}


void OTStudyItem::updatePhysicalModel(const PhysicalModel & physicalModel)
{
  otStudy_->getPhysicalModelByName(physicalModel.getName()).setImplementationAsPersistentObject(physicalModel.getImplementation());
}


void OTStudyItem::updateAnalysis(const Analysis & analysis)
{
  otStudy_->getAnalysisByName(analysis.getName()).setImplementationAsPersistentObject(analysis.getImplementation());
}


void OTStudyItem::addDeterministicAnalysisItem(Analysis & analysis, AnalysisItem * item)
{
  analysis.addObserver(item);
  for (int i=0; i<rowCount(); ++i)
    if (child(i)->text().toStdString() == analysis.getPhysicalModel().getName())
    {
      child(i)->child(0)->appendRow(item);
      emit newAnalysisItemCreated(item);
      return;
    }
  std::cerr<<"No item added for the deterministic analysis named "<<analysis.getName()<<std::endl;
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
      emit newAnalysisItemCreated(item);
      return;
    }
  std::cerr<<"No item added for the probabilistic analysis named "<<analysis.getName()<<std::endl;
}


void OTStudyItem::addReliabilityAnalysisItem(Analysis & analysis, AnalysisItem * item)
{
  analysis.addObserver(item);
  std::string limitStateName = dynamic_cast<ReliabilityAnalysis*>(&*analysis.getImplementation())->getLimitState().getName();
  for (int i=0; i<rowCount(); ++i)
    if (child(i)->text().toStdString() == analysis.getPhysicalModel().getName())
    {
      QStandardItem * probabilisticStudyItem = child(i)->child(1);
      for (int j=0; j<probabilisticStudyItem->rowCount(); ++j)
        if (probabilisticStudyItem->child(j)->text().toStdString() == limitStateName)
        {
          probabilisticStudyItem->child(j)->appendRow(item);
          emit newAnalysisItemCreated(item);
          return;
        }
    }
  std::cerr<<"No item added for the reliability analysis named "<<analysis.getName()<<std::endl;
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