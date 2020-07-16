#include "persalys/PVXYChartViewWidget.hxx"

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
#include <pqCoreUtilities.h>

using namespace OT;

namespace PERSALYS
{
const QMap<PVXYChartViewWidget::Type, const char*> PVXYChartViewWidget::PV_VIEW_TYPE{
  {PVXYChartViewWidget::Scatter, "XYChartView"},
  {PVXYChartViewWidget::TrajectoriesPoints, "XYChartView"},
  {PVXYChartViewWidget::Trajectories, "XYChartView"},
  {PVXYChartViewWidget::BagChart, "XYBagChartView"},
  {PVXYChartViewWidget::FunctionalBagChart, "XYFunctionalBagChartView"}};

const QMap<PVXYChartViewWidget::Type, const char*> PVXYChartViewWidget::PV_REPRESENTATION_TYPE{
  {PVXYChartViewWidget::Scatter, "XYChartRepresentation"},
  {PVXYChartViewWidget::TrajectoriesPoints, "XYChartRepresentation"},
  {PVXYChartViewWidget::Trajectories, "XYChartRepresentationColumns"},
  {PVXYChartViewWidget::BagChart, "XYBagChartRepresentation"},
  {PVXYChartViewWidget::FunctionalBagChart, "XYFunctionalBagChartRepresentation"}};

const QColor PVXYChartViewWidget::DEFAULT_SCATTER_PLOT_COLOR = Qt::blue;


PVXYChartViewWidget::PVXYChartViewWidget(QWidget *parent, PVServerManagerInterface *smb, const Type type)
  : PVViewWidget(parent, smb, PV_VIEW_TYPE[type])
  , chartXY_(0)
  , chartsTitle_()
  , axisLabels_()
  , type_(type)
  , reprColors_()
{
  pqContextView * viewC(dynamic_cast<pqContextView *>(getView()));

  if (!viewC)
    return ;
  vtkAbstractContextItem * contextItem(viewC->getContextViewProxy()->GetContextItem());
  chartXY_ = vtkChartXY::SafeDownCast(contextItem);

  int selectionType(vtkChart::SELECT_RECTANGLE);
  int selectionModifier(vtkContextScene::SELECTION_DEFAULT);
  chartXY_->SetActionToButton(selectionType, vtkContextMouseEvent::RIGHT_BUTTON);
  chartXY_->SetSelectionMode(selectionModifier);
  pqCoreUtilities::connect(chartXY_->GetAxis(vtkAxis::BOTTOM), vtkCommand::ModifiedEvent, this, SIGNAL(axisHasBeenModified()));
  pqCoreUtilities::connect(chartXY_->GetAxis(vtkAxis::LEFT), vtkCommand::ModifiedEvent, this, SIGNAL(axisHasBeenModified()));
}


PVXYChartViewWidget::~PVXYChartViewWidget()
{
  chartXY_ = 0;
}


void PVXYChartViewWidget::setData(const std::vector< std::vector<double> >& valuesByColumn, const std::vector<std::string>& columnNames)
{
  // set data
  PVViewWidget::setData(valuesByColumn, columnNames);

  // set default axis/title properties
  for (std::size_t cc = 0; cc < columnNames.size(); cc++)
  {
    axisLabels_[columnNames[cc].c_str()] = columnNames[cc].c_str();
    for (std::size_t cc2 = 0; cc2 < columnNames.size(); cc2++)
    {
      QPair<QString, QString> varPair(columnNames[cc].c_str(), columnNames[cc2].c_str());
      if (!chartsTitle_.contains(varPair))
      {
        const QString title((type_ == Scatter ? tr("Scatter plot") + ": " : "") + columnNames[cc2].c_str() + " " + tr("vs") + " " + columnNames[cc].c_str());
        chartsTitle_[varPair] = title;
      }
    }
  }

  // set scatter plot style
  setPlotStyle();
}


void PVXYChartViewWidget::setData(const Sample& sample, const QColor color)
{
  // set data
  PVViewWidget::setData(sample);

  // set color
  setRepresentationColor(color, getView()->getNumberOfRepresentations() - 1);
}


void PVXYChartViewWidget::setXAxisData(const QString& varX)
{
  for (int reprInd = 0; reprInd < getView()->getNumberOfRepresentations(); reprInd++)
  {
    // set x axis
    vtkSMProperty * idvpXName(getView()->getRepresentation(reprInd)->getProxy()->GetProperty("XArrayName"));
    if (!idvpXName)
      return;
    vtkSMPropertyHelper(idvpXName).Set(varX.toStdString().c_str());
    getView()->getRepresentation(reprInd)->getProxy()->UpdateProperty("XArrayName");
  }
  chartXY_->GetAxis(vtkAxis::BOTTOM)->SetTitle(axisLabels_[varX].toStdString());
}


void PVXYChartViewWidget::showChart(const QString& varX, const QString& varY)
{
  setLogScale(vtkAxis::BOTTOM, false);
  setLogScale(vtkAxis::LEFT, false);

  setXAxisData(varX);

  // set y axis
  std::vector<std::string> sts(1, varY.toStdString());
  setAxisToShow(sts);

  // pair
  QPair<QString, QString> varPair(varX, varY);
  if (chartsTitle_.contains(varPair))
    chartXY_->SetTitle(chartsTitle_[varPair].toStdString());
  if (axisLabels_.contains(varX))
    chartXY_->GetAxis(vtkAxis::BOTTOM)->SetTitle(axisLabels_[varX].toStdString());
  if (axisLabels_.contains(varY))
    chartXY_->GetAxis(vtkAxis::LEFT)->SetTitle(axisLabels_[varY].toStdString());

  // recalculate automatically the axis bounds
  chartXY_->GetAxis(vtkAxis::LEFT)->SetBehavior(0);
  chartXY_->GetAxis(vtkAxis::BOTTOM)->SetBehavior(0);
}


double PVXYChartViewWidget::getAxisRangeMinimum(const vtkAxis::Location ax)
{
  return chartXY_->GetAxis(ax)->GetMinimum();
}


double PVXYChartViewWidget::getAxisRangeMaximum(const vtkAxis::Location ax)
{
  return chartXY_->GetAxis(ax)->GetMaximum();
}


void PVXYChartViewWidget::setAxisRange(const vtkAxis::Location ax, const double minValue, const double maxValue)
{
  // set range
  chartXY_->GetAxis(ax)->SetBehavior(1);
  chartXY_->GetAxis(ax)->SetMinimum(minValue);
  chartXY_->GetAxis(ax)->SetMaximum(maxValue);
  getView()->resetDisplay();
}


void PVXYChartViewWidget::setPlotStyle()
{
  for (int reprInd = 0; reprInd < getView()->getNumberOfRepresentations(); reprInd++)
  {
    if (type_ == Scatter || type_ == TrajectoriesPoints)
    {
      // line style : none
      vtkSMProperty * idvpLineStyle(getView()->getRepresentation(reprInd)->getProxy()->GetProperty("SeriesLineStyle"));
      QList<QVariant> lineStyles = pqSMAdaptor::getMultipleElementProperty(idvpLineStyle);

      vtkSMPropertyHelper smphLineStyle(idvpLineStyle);

      for (int cc = 0; cc < lineStyles.size() / 2; cc++)
      {
        smphLineStyle.Set(2 * cc + 1, 0);
      }

      getView()->getRepresentation(reprInd)->getProxy()->UpdateProperty("SeriesLineStyle");

      // marker style : circle
      setMarkerStyle(vtkPlotPoints::CIRCLE);
    }

    // UseIndexForXAxis : no
    vtkSMProperty * idvpUseIndex(getView()->getRepresentation(reprInd)->getProxy()->GetProperty("UseIndexForXAxis"));
    vtkSMPropertyHelper(idvpUseIndex).Set(false);
    getView()->getRepresentation(reprInd)->getProxy()->UpdateProperty("UseIndexForXAxis");
  }

  // ChartTitleBold : yes
  vtkSMProperty * idvpTitleBold(getView()->getProxy()->GetProperty("ChartTitleBold"));
  vtkSMPropertyHelper(idvpTitleBold).Set(true);
  getView()->getProxy()->UpdateProperty("ChartTitleBold");

  // ShowLegend : no if only one representation
  setShowLegend(getView()->getNumberOfRepresentations() > 1 || type_ == Trajectories || type_ == TrajectoriesPoints);

  // set color
  if (type_ == Scatter)
  {
    reprColors_.append(DEFAULT_SCATTER_PLOT_COLOR);
    setRepresentationColor(DEFAULT_SCATTER_PLOT_COLOR, getView()->getNumberOfRepresentations() - 1);
  }
}


QString PVXYChartViewWidget::getChartTitle(const QString& varX, const QString& varY)
{
  QPair<QString, QString> pair(varX, varY);
  if (chartsTitle_.contains(pair))
    return chartsTitle_[pair];
  else
    return chartXY_->GetTitle().c_str();
}


QString PVXYChartViewWidget::getAxisTitle(const vtkAxis::Location ax, const QString& varX, const QString& varY)
{
  if (axisLabels_.contains(ax == vtkAxis::BOTTOM ? varX : varY))
    return axisLabels_[ax == vtkAxis::BOTTOM ? varX : varY];
  else
    return chartXY_->GetAxis(ax)->GetTitle().c_str();
}


void PVXYChartViewWidget::setChartTitle(const QString &title, const QString &varX, const QString &varY)
{
  // set internal parameter to be able to update automatically the plot
  chartXY_->GetScene()->SetDirty(true);
  // chart title
  QPair<QString, QString> pair(varX, varY);
  if (chartsTitle_.contains(pair))
    chartsTitle_[pair] = title;
  chartXY_->SetTitle(title.toStdString());
}


void PVXYChartViewWidget::setAxisTitle(const vtkAxis::Location ax, const QString &title, const QString &varX, const QString &varY)
{
  // set internal parameter to be able to update automatically the plot
  chartXY_->GetScene()->SetDirty(true);
  if (axisLabels_.contains(ax == vtkAxis::BOTTOM ? varX : varY))
    axisLabels_[ax == vtkAxis::BOTTOM ? varX : varY] = title;
  chartXY_->GetAxis(ax)->SetTitle(title.toStdString());
}


void PVXYChartViewWidget::setAxisTitles(const QStringList &variablesNames, const QStringList &titles)
{
  Q_ASSERT(variablesNames.size() == titles.size());
  for (int i = 0; i < variablesNames.size(); ++i)
    axisLabels_[variablesNames[i]] = titles[i];
}


void PVXYChartViewWidget::setLogScale(const vtkAxis::Location ax, const bool scale)
{
  // set internal parameter to be able to update automatically the plot
  chartXY_->GetScene()->SetDirty(true);

  // axis scale
  chartXY_->GetAxis(ax)->SetBehavior(1);
  chartXY_->GetAxis(ax)->SetLogScale(scale);
}


bool PVXYChartViewWidget::logScalingValidForAxis(const vtkAxis::Location ax)
{
  double bounds[2];
  chartXY_->GetAxis(ax)->GetUnscaledRange(bounds);
  return bounds[0] * bounds[1] > 0.;
}


void PVXYChartViewWidget::setSerieColors(const QMap<QString, QColor>& colors)
{
  for (int repr_ind = 0; repr_ind < getView()->getNumberOfRepresentations(); repr_ind++)
  {
    // get property
    vtkSMProperty* idvp(getView()->getRepresentation(repr_ind)->getProxy()->GetProperty("SeriesColor"));
    QList<QVariant> value = pqSMAdaptor::getMultipleElementProperty(idvp);
    vtkSMPropertyHelper smph(idvp);

    for (int cc = 0; cc < value.size()/4; cc++)
    {
      QString name = value[4 * cc].toString();
      if (colors.contains(name))
      {
        QColor color = colors[name];
        smph.Set(4 * cc + 1, (OSS() << color.redF()).str().c_str());
        smph.Set(4 * cc + 2, (OSS() << color.greenF()).str().c_str());
        smph.Set(4 * cc + 3, (OSS() << color.blueF()).str().c_str());
      }
    }

    // update property
    getView()->getRepresentation(repr_ind)->getProxy()->UpdateProperty("SeriesColor");
  }
  getView()->resetDisplay();
}


void PVXYChartViewWidget::setSerieLineStyles(const QMap<QString, int>& styles)
{
  for (int repr_ind = 0; repr_ind < getView()->getNumberOfRepresentations(); repr_ind++)
  {
    // get property
    vtkSMProperty* idvp(getView()->getRepresentation(repr_ind)->getProxy()->GetProperty("SeriesLineStyle"));
    QList<QVariant> value = pqSMAdaptor::getMultipleElementProperty(idvp);
    vtkSMPropertyHelper smph(idvp);

    for (int cc = 0; cc < value.size()/2; cc++)
    {
      QString name = value[2 * cc].toString();
      if (styles.contains(name))
      {
        smph.Set(2 * cc + 1, styles[name]);
      }
    }

    // update property
    getView()->getRepresentation(repr_ind)->getProxy()->UpdateProperty("SeriesLineStyle");
  }
  getView()->resetDisplay();
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
  vtkSMPropertyHelper smph(idvp);
  QList<QVariant> colors = pqSMAdaptor::getMultipleElementProperty(idvp);

  for (int cc = 0; cc < colors.size() / 4; cc++)
  {
    smph.Set(4 * cc + 1, (OSS() << color.redF()).str().c_str());
    smph.Set(4 * cc + 2, (OSS() << color.greenF()).str().c_str());
    smph.Set(4 * cc + 3, (OSS() << color.blueF()).str().c_str());
  }
  getView()->getRepresentation(reprIndex)->getProxy()->UpdateProperty("SeriesColor");

  if (reprColors_.size() == numberOfRepr)
    reprColors_[reprIndex] = color;
}


QColor PVXYChartViewWidget::getRepresentationColor(const int reprIndex) const
{
  if (!reprColors_.size())
    return DEFAULT_SCATTER_PLOT_COLOR;
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


void PVXYChartViewWidget::setMarkerStyle(const int markerStyle)
{
  // set internal parameter to be able to update automatically the plot
  chartXY_->GetScene()->SetDirty(true);

  for (int reprInd = 0; reprInd < getView()->getNumberOfRepresentations(); reprInd++)
  {
    // set marker style property
    vtkSMProperty * idvp(getView()->getRepresentation(reprInd)->getProxy()->GetProperty("SeriesMarkerStyle"));
    QList<QVariant> markerStyles = pqSMAdaptor::getMultipleElementProperty(idvp);

    vtkSMPropertyHelper smph(idvp);

    for (int cc = 0; cc < markerStyles.size() / 2; cc++)
    {
      smph.Set(2 * cc + 1, markerStyle);
    }

    getView()->getRepresentation(reprInd)->getProxy()->UpdateProperty("SeriesMarkerStyle");
  }
}


int PVXYChartViewWidget::getMarkerStyle() const
{
  if (!getView()->getNumberOfRepresentations())
    return vtkPlotPoints::NONE;

  // get marker style property
  vtkSMProperty * idvp(getView()->getRepresentation(0)->getProxy()->GetProperty("SeriesMarkerStyle"));
  return vtkSMPropertyHelper(idvp).GetAsInt(1);
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

    vtkSMPropertyHelper smph(idvp);

    for (int cc = 0; cc < markerSizes.size() / 2; cc++)
    {
      smph.Set(2 * cc + 1, markerSize);
    }

    getView()->getRepresentation(reprInd)->getProxy()->UpdateProperty("SeriesLineThickness");
  }
}


void PVXYChartViewWidget::setShowLegend(const bool show)
{
  chartXY_->SetShowLegend(show);
}
}
