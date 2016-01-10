#include "otgui/ModelEvaluationItem.hxx"

#include "otgui/ModelEvaluation.hxx"

namespace OTGUI {

ModelEvaluationItem::ModelEvaluationItem(const Analysis & analysis)
  : AnalysisItem(analysis, QString("ModelEvaluation"))
{
}


void ModelEvaluationItem::update(Observable* source, const std::string & message)
{
  if (message=="analysisFinished")
  {
    emit analysisFinished(this);
  }
}
}