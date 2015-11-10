// AnalysisItem.cxx

#include "otgui/AnalysisItem.hxx"

namespace OTGUI {

AnalysisItem::AnalysisItem(const Analysis & analysis, const QString & typeAnalysis)
  : Item(analysis.getName(), typeAnalysis)
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
      Item::setData(value, role);
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