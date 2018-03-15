#include "otgui/PVXYChartViewWidget.hxx"

#include <vtkSMProperty.h>
#include <vtkSMPropertyHelper.h>
#include <pqSMAdaptor.h>
#include <pqDataRepresentation.h>
#include <vtkPlotPoints.h>
#include <pqView.h>
#include <pqContextView.h>
#include <vtkSMContextViewProxy.h>
#include <vtkContextMouseEvent.h>
#include <vtkCommand.h>
#include <vtkAxis.h>
#include <vtkContextScene.h>

#include <vtkXYChartRepresentation.h>

#include <QEvent>

using namespace OT;

namespace OTGUI
{
const char PVXYChartViewWidget::PV_VIEW_TYPE[] = "XYChartView";

const char PVXYChartViewWidget::PV_REPRESENTATION_TYPE[] = "XYChartRepresentation";

const QColor PVXYChartViewWidget::DEFAULT_SCATTER_PLOT_COLOR = Qt::blue;

PVXYChartViewWidget::PVXYChartViewWidget(QWidget *parent, PVServerManagerInterface *smb)
  : PVViewWidget(parent, smb, PV_VIEW_TYPE)
  , chartXY_(0)
  , reprColors_()
{
  pqContextView * viewC(dynamic_cast<pqContextView *>(getView()));
  if(!viewC)
    return ;
  vtkAbstractContextItem * contextItem(viewC->getContextViewProxy()->GetContextItem());
  chartXY_ = vtkChartXY::SafeDownCast(contextItem);

  int selectionType(vtkChart::SELECT_RECTANGLE);
  int selectionModifier(vtkContextScene::SELECTION_DEFAULT);
  chartXY_->SetActionToButton(selectionType, vtkContextMouseEvent::RIGHT_BUTTON);
  chartXY_->SetSelectionMode(selectionModifier);
}


PVXYChartViewWidget::~PVXYChartViewWidget()
{
  chartXY_ = 0;
}


void PVXYChartViewWidget::setData(const std::vector< std::vector<double> >& valuesByColumn, const std::vector<std::string>& columnNames)
{
  // set data
  PVViewWidget::setData(valuesByColumn, columnNames);

  // set axis/title properties
  for (std::size_t cc = 0; cc < columnNames.size(); cc++)
  {
    for (std::size_t cc2 = 0; cc2 < columnNames.size(); cc2++)
    {
      QPair<QString, QString> varPair(columnNames[cc].c_str(), columnNames[cc2].c_str());
      if (!chartsTitle_.contains(varPair))
      {
        chartsTitle_[varPair] = tr("Scatter plot:") + " " + columnNames[cc2].c_str() + " " + tr("vs") + " " + columnNames[cc].c_str();
        XAxisTitle_[varPair] = columnNames[cc].c_str();
        YAxisTitle_[varPair] = columnNames[cc2].c_str();
      }
    }
  }

  // set scatter plot style
  setScatterPlotStyle();
}


void PVXYChartViewWidget::setData(const Sample& sample, const QColor color)
{
  // set data
  PVViewWidget::setData(sample);
  // set color
  setRepresentationColor(color, getView()->getNumberOfRepresentations() - 1);
}


void PVXYChartViewWidget::showChart(const QString& varX, const QString& varY)
{
  setXLogScale(false);
  setYLogScale(false);

  for (int reprInd = 0; reprInd < getView()->getNumberOfRepresentations(); reprInd++)
  {
    // set x axis
    vtkSMProperty * idvpXName(getView()->getRepresentation(reprInd)->getProxy()->GetProperty("XArrayName"));
    if (!idvpXName)
      return;
    vtkSMPropertyHelper * smphXName(new vtkSMPropertyHelper(idvpXName));
    if (!smphXName)
      return;
    smphXName->Set(varX.toStdString().c_str());
    getView()->getRepresentation(reprInd)->getProxy()->UpdateProperty("XArrayName");
    delete smphXName;
  }

  // set y axis
  std::vector<std::string> sts(1, varY.toStdString());
  setAxisToShow(sts);

  // set title
  setChartTitle(varX, varY);
  // set axis titles
  setXAxisTitle(varX, varY);
  setYAxisTitle(varX, varY);

  // recalculate automatically the axis bounds
  chartXY_->GetAxis(vtkAxis::LEFT)->SetBehavior(0);
  chartXY_->GetAxis(vtkAxis::BOTTOM)->SetBehavior(0);
}


double PVXYChartViewWidget::getXAxisRangeMinimum()
{
  return chartXY_->GetAxis(vtkAxis::BOTTOM)->GetMinimum();
}


double PVXYChartViewWidget::getXAxisRangeMaximum()
{
  return chartXY_->GetAxis(vtkAxis::BOTTOM)->GetMaximum();
}


double PVXYChartViewWidget::getYAxisRangeMinimum()
{
  return chartXY_->GetAxis(vtkAxis::LEFT)->GetMinimum();
}


double PVXYChartViewWidget::getYAxisRangeMaximum()
{
  return chartXY_->GetAxis(vtkAxis::LEFT)->GetMaximum();
}


void PVXYChartViewWidget::setXAxisRange(const double minValue, const double maxValue)
{
  // set range
  chartXY_->GetAxis(vtkAxis::BOTTOM)->SetBehavior(1);
  chartXY_->GetAxis(vtkAxis::BOTTOM)->SetMinimum(minValue);
  chartXY_->GetAxis(vtkAxis::BOTTOM)->SetMaximum(maxValue);

  getView()->resetDisplay();
}


void PVXYChartViewWidget::setYAxisRange(const double minValue, const double maxValue)
{
  // set range
  chartXY_->GetAxis(vtkAxis::LEFT)->SetBehavior(1);
  chartXY_->GetAxis(vtkAxis::LEFT)->SetMinimum(minValue);
  chartXY_->GetAxis(vtkAxis::LEFT)->SetMaximum(maxValue);

  getView()->resetDisplay();
}


void PVXYChartViewWidget::setScatterPlotStyle()
{
  for (int reprInd = 0; reprInd < getView()->getNumberOfRepresentations(); reprInd++)
  {
    // line style : none
    vtkSMProperty * idvpLineStyle(getView()->getRepresentation(reprInd)->getProxy()->GetProperty("SeriesLineStyle"));
    QList<QVariant> lineStyles = pqSMAdaptor::getMultipleElementProperty(idvpLineStyle);

    vtkSMPropertyHelper * smphLineStyle(new vtkSMPropertyHelper(idvpLineStyle));

    for (int cc = 0; cc < lineStyles.size() / 2; cc++)
    {
      smphLineStyle->Set(2 * cc + 1, 0);
    }

    getView()->getRepresentation(reprInd)->getProxy()->UpdateProperty("SeriesLineStyle");
    delete smphLineStyle;

    // marker style : circle
    setMarkerStyle(vtkPlotPoints::CIRCLE);

    // UseIndexForXAxis : no
    vtkSMProperty * idvpUseIndex(getView()->getRepresentation(reprInd)->getProxy()->GetProperty("UseIndexForXAxis"));
    vtkSMPropertyHelper * smphUseIndex(new vtkSMPropertyHelper(idvpUseIndex));
    smphUseIndex->Set(false);
    getView()->getRepresentation(reprInd)->getProxy()->UpdateProperty("UseIndexForXAxis");
    delete smphUseIndex;
  }

  // ChartTitleBold : yes
  vtkSMProperty * idvpTitleBold(getView()->getProxy()->GetProperty("ChartTitleBold"));
  vtkSMPropertyHelper * smphTitleBold(new vtkSMPropertyHelper(idvpTitleBold));
  smphTitleBold->Set(true);
  getView()->getProxy()->UpdateProperty("ChartTitleBold");
  delete smphTitleBold;

  // ShowLegend : no if only one representation
  setShowLegend(getView()->getNumberOfRepresentations() > 1);

  // set color
  reprColors_.append(DEFAULT_SCATTER_PLOT_COLOR);
  setRepresentationColor(DEFAULT_SCATTER_PLOT_COLOR, getView()->getNumberOfRepresentations() - 1);
}


QString PVXYChartViewWidget::getChartTitle(const QString& varX, const QString& varY)
{
  QPair<QString, QString> varPair(varX, varY);
  return chartsTitle_[varPair];
}


QString PVXYChartViewWidget::getXAxisTitle(const QString& varX, const QString& varY)
{
  QPair<QString, QString> varPair(varX, varY);
  return XAxisTitle_[varPair];
}


QString PVXYChartViewWidget::getYAxisTitle(const QString& varX, const QString& varY)
{
  QPair<QString, QString> varPair(varX, varY);
  return YAxisTitle_[varPair];
}


void PVXYChartViewWidget::setChartTitle(const QString& varX, const QString& varY)
{
  // pair
  QPair<QString, QString> varPair(varX, varY);
  if (!chartsTitle_.contains(varPair))
    return;

  // set internal parameter to be able to update automatically the plot
  chartXY_->GetScene()->SetDirty(true);

  // chart title
  chartXY_->SetTitle(chartsTitle_[varPair].toStdString());
}


void PVXYChartViewWidget::setXAxisTitle(const QString& varX, const QString& varY)
{
  // pair
  QPair<QString, QString> varPair(varX, varY);
  if (!XAxisTitle_.contains(varPair) || !chartXY_)
    return;

  // set internal parameter to be able to update automatically the plot
  chartXY_->GetScene()->SetDirty(true);

  // bottom axis title
  chartXY_->GetAxis(vtkAxis::BOTTOM)->SetTitle(XAxisTitle_[varPair].toStdString());
}


void PVXYChartViewWidget::setYAxisTitle(const QString& varX, const QString& varY)
{
  // pair
  QPair<QString, QString> varPair(varX, varY);
  if (!YAxisTitle_.contains(varPair) || !chartXY_)
    return;

  // set internal parameter to be able to update automatically the plot
  chartXY_->GetScene()->SetDirty(true);

  // left axis title
  chartXY_->GetAxis(vtkAxis::LEFT)->SetTitle(YAxisTitle_[varPair].toStdString());
}


void PVXYChartViewWidget::setChartTitle(const QString& varX, const QString& varY, const QString& title)
{
  // pair
  QPair<QString, QString> varPair(varX, varY);
  if (!chartsTitle_.contains(varPair))
    return;

  // chart title
  chartsTitle_[varPair] = title;
  setChartTitle(varX, varY);
}


void PVXYChartViewWidget::setXAxisTitle(const QString& varX, const QString& varY, const QString& title)
{
  // pair
  QPair<QString, QString> varPair(varX, varY);
  if (!XAxisTitle_.contains(varPair))
    return;

  // bottom axis title
  XAxisTitle_[varPair] = title;
  setXAxisTitle(varX, varY);
}


void PVXYChartViewWidget::setYAxisTitle(const QString& varX, const QString& varY, const QString& title)
{
  // pair
  QPair<QString, QString> varPair(varX, varY);
  if (!YAxisTitle_.contains(varPair))
    return;
  // left axis title
  YAxisTitle_[varPair] = title;
  setYAxisTitle(varX, varY);
}


void PVXYChartViewWidget::setAxisTitles(const QStringList& variablesNames, const QStringList& titles)
{
  for (int i = 0; i < variablesNames.size(); ++i)
  {
    for (int j = 0; j < variablesNames.size(); ++j)
    {
      setXAxisTitle(variablesNames[i], variablesNames[j], titles[i]);
      setYAxisTitle(variablesNames[i], variablesNames[j], titles[j]);
    }
  }
}


void PVXYChartViewWidget::setXLogScale(const bool scale)
{
  // set internal parameter to be able to update automatically the plot
  chartXY_->GetScene()->SetDirty(true);

  // left axis title
  chartXY_->GetAxis(vtkAxis::BOTTOM)->SetBehavior(1);
  chartXY_->GetAxis(vtkAxis::BOTTOM)->SetLogScale(scale);
}


void PVXYChartViewWidget::setYLogScale(const bool scale)
{
  // set internal parameter to be able to update automatically the plot
  chartXY_->GetScene()->SetDirty(true);

  // left axis title
  chartXY_->GetAxis(vtkAxis::LEFT)->SetBehavior(1);
  chartXY_->GetAxis(vtkAxis::LEFT)->SetLogScale(scale);
}


bool PVXYChartViewWidget::logScalingValidForXAxis()
{
  double bounds[2];
  chartXY_->GetAxis(vtkAxis::BOTTOM)->GetUnscaledRange(bounds);
  return bounds[0] * bounds[1] > 0.;
}


bool PVXYChartViewWidget::logScalingValidForYAxis()
{
  double bounds[2];
  chartXY_->GetAxis(vtkAxis::LEFT)->GetUnscaledRange(bounds);
  return bounds[0] * bounds[1] > 0.;
}


void PVXYChartViewWidget::setRepresentationColor(const QColor& color, const int reprIndex)
{
  const int numberOfRepr = getView()->getNumberOfRepresentations();

  // check index
  if (reprIndex >= numberOfRepr)
  {
    OSS oss;
    oss << "PVXYChartViewWidget::setRepresentationColor: the given representation index "
        << reprIndex
        << " is not valid. The number of representations is "
        << numberOfRepr;
    throw InvalidArgumentException(HERE) << oss.str();
  }

  // set internal parameter to be able to update automatically the plot
  chartXY_->GetScene()->SetDirty(true);

  // set color property
  vtkSMProperty * idvp(getView()->getRepresentation(reprIndex)->getProxy()->GetProperty("SeriesColor"));
  vtkSMPropertyHelper * smph(new vtkSMPropertyHelper(idvp));
  QList<QVariant> colors = pqSMAdaptor::getMultipleElementProperty(idvp);

  for (int cc = 0; cc < colors.size() / 4; cc++)
  {
    smph->Set(4 * cc + 1, (OSS() << color.redF()).str().c_str());
    smph->Set(4 * cc + 2, (OSS() << color.greenF()).str().c_str());
    smph->Set(4 * cc + 3, (OSS() << color.blueF()).str().c_str());
  }
  getView()->getRepresentation(reprIndex)->getProxy()->UpdateProperty("SeriesColor");
  delete smph;

  if (reprColors_.size() == numberOfRepr)
    reprColors_[reprIndex] = color;
}


QColor PVXYChartViewWidget::getRepresentationColor(const int reprIndex) const
{
  if (reprColors_.size() != getView()->getNumberOfRepresentations())
  {
    OSS oss;
    oss << "PVXYChartViewWidget::getRepresentationColor: the number of stored colors is "
        << reprColors_.size()
        << ". It must be "
        << getView()->getNumberOfRepresentations();
    throw InvalidArgumentException(HERE) << oss.str();
  }
  return reprColors_[reprIndex];
}


void PVXYChartViewWidget::setRepresentationLabels(const QStringList& newLabels, const int reprIndex)
{
  const int numberOfRepr = getView()->getNumberOfRepresentations();

  // check index
  if (reprIndex >= numberOfRepr)
  {
    OSS oss;
    oss << "PVXYChartViewWidget::setRepresentationColor: the given representation index "
        << reprIndex
        << " is not valid. The number of representations is "
        << numberOfRepr;
    throw InvalidArgumentException(HERE) << oss.str();
  }

  // set internal parameter to be able to update automatically the plot
  chartXY_->GetScene()->SetDirty(true);

  // set label property
  vtkSMProperty * idvp(getView()->getRepresentation(reprIndex)->getProxy()->GetProperty("SeriesLabel"));
  vtkSMPropertyHelper * smph(new vtkSMPropertyHelper(idvp));
  QList<QVariant> labels = pqSMAdaptor::getMultipleElementProperty(idvp);

  // check labels
  if (newLabels.size() != labels.size() / 2)
  {
    OSS oss;
    oss << "PVXYChartViewWidget::setRepresentationLabels: the number of given labels "
        << newLabels.size()
        << " does not match the number of variables of the representation "
        << labels.size() / 2;
    throw InvalidArgumentException(HERE) << oss.str();
  }

  for (int cc = 0; cc < labels.size() / 2; cc++)
  {
    smph->Set(2 * cc + 1, newLabels[cc].toStdString().c_str());
  }
  getView()->getRepresentation(reprIndex)->getProxy()->UpdateProperty("SeriesLabel");
  delete smph;
}


QStringList PVXYChartViewWidget::getRepresentationLabels(const int reprIndex) const
{
  const int numberOfRepr = getView()->getNumberOfRepresentations();

  // check index
  if (reprIndex >= numberOfRepr)
  {
    OSS oss;
    oss << "PVXYChartViewWidget::getRepresentationLabels: the given representation index "
        << reprIndex
        << " is not valid. The number of representations is "
        << numberOfRepr;
    throw InvalidArgumentException(HERE) << oss.str();
  }

  // set label property
  vtkSMProperty * idvp(getView()->getRepresentation(reprIndex)->getProxy()->GetProperty("SeriesLabel"));
  vtkSMPropertyHelper * smph(new vtkSMPropertyHelper(idvp));
  QList<QVariant> labelsVariant = pqSMAdaptor::getMultipleElementProperty(idvp);

  QStringList labels;
  for (int cc = 0; cc < labelsVariant.size() / 2; cc++)
  {
    labels << labelsVariant[2 * cc + 1].toString();
  }
  delete smph;
  return labels;
}


void PVXYChartViewWidget::setMarkerStyle(const int markerStyle)
{
  // set internal parameter to be able to update automatically the plot
  chartXY_->GetScene()->SetDirty(true);

  for (int reprInd = 0; reprInd < getView()->getNumberOfRepresentations(); reprInd++)
  {
    // set marker style property
    vtkSMProperty * idvp(getView()->getRepresentation(reprInd)->getProxy()->GetProperty("SeriesMarkerStyle"));
    QList<QVariant> markerStyles = pqSMAdaptor::getMultipleElementProperty(idvp);

    vtkSMPropertyHelper * smph(new vtkSMPropertyHelper(idvp));

    for (int cc = 0; cc < markerStyles.size() / 2; cc++)
    {
      smph->Set(2 * cc + 1, markerStyle);
    }

    getView()->getRepresentation(reprInd)->getProxy()->UpdateProperty("SeriesMarkerStyle");
    delete smph;
  }
}


void PVXYChartViewWidget::setMarkerSize(const int markerSize)
{
  // set internal parameter to be able to update automatically the plot
  chartXY_->GetScene()->SetDirty(true);

  for (int reprInd = 0; reprInd < getView()->getNumberOfRepresentations(); reprInd++)
  {
    // set marker style property
    vtkSMProperty * idvp(getView()->getRepresentation(reprInd)->getProxy()->GetProperty("SeriesLineThickness"));
    QList<QVariant> markerSizes = pqSMAdaptor::getMultipleElementProperty(idvp);

    vtkSMPropertyHelper * smph(new vtkSMPropertyHelper(idvp));

    for (int cc = 0; cc < markerSizes.size() / 2; cc++)
    {
      smph->Set(2 * cc + 1, markerSize);
    }

    getView()->getRepresentation(reprInd)->getProxy()->UpdateProperty("SeriesLineThickness");
    delete smph;
  }
}


void PVXYChartViewWidget::setShowLegend(const bool show)
{
  chartXY_->SetShowLegend(show);
}


int PVXYChartViewWidget::getNumberOfRepresentations() const
{
  return getView()->getNumberOfRepresentations();
}


void PVXYChartViewWidget::setRepresentationVisibility(const bool visibility, const int reprIndex)
{
  const int numberOfRepr = getView()->getNumberOfRepresentations();

  // check index
  if (reprIndex >= numberOfRepr)
  {
    OSS oss;
    oss << "PVXYChartViewWidget::getRepresentationLabels: the given representation index "
        << reprIndex
        << " is not valid. The number of representations is "
        << numberOfRepr;
    throw InvalidArgumentException(HERE) << oss.str();
  }
  // set internal parameter to be able to update automatically the plot
  chartXY_->GetScene()->SetDirty(true);

  getView()->getRepresentation(reprIndex)->setVisible(visibility);
}
}
