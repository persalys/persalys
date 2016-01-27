#include "otgui/ModelEvaluationItem.hxx"

using namespace OT;

namespace OTGUI {

ModelEvaluationItem::ModelEvaluationItem(const Analysis & analysis)
  : AnalysisItem(analysis, QString("ModelEvaluation"))
{
}


void ModelEvaluationItem::update(Observable* source, const String & message)
{
  if (message=="analysisFinished")
  {
    emit analysisFinished(this);
  }
}
}