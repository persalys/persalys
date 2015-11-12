// AnalysisItem.cxx

#include "otgui/AnalysisItem.hxx"

namespace OTGUI {

AnalysisItem::AnalysisItem(const Analysis & analysis, const QString & typeAnalysis)
  : ObserverItem(analysis.getName().c_str(), typeAnalysis)
  , analysis_(analysis)
{
}


AnalysisItem::~AnalysisItem()
{
}


void AnalysisItem::setData(const QVariant& value, int role)
{
  switch (role)
  {
    case Qt::EditRole:
      analysis_.setName(value.toString().toStdString());
      ObserverItem::setData(value, role);
      break;
  }
}


Analysis AnalysisItem::getAnalysis() const
{
  return analysis_;
}


void AnalysisItem::updateAnalysis(const Analysis & analysis)
{
  analysis_ = analysis;
  analysis_.addObserver(this);
}


}