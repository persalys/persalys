//                                               -*- C++ -*-
/**
 *  @brief QWidget for Morris analysis result
 *
 *  Copyright 2015-2019 EDF-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/MorrisResultWidget.hxx"

#include "otgui/PlotWidget.hxx"
#include "otgui/QtTools.hxx"
#include "otgui/CheckableHeaderView.hxx"
#include "otgui/CopyableTableView.hxx"
#include "otgui/MorrisResultTableModel.hxx"
#include "otgui/WidgetBoundToDockWidget.hxx"
#include "otgui/GraphConfigurationWidget.hxx"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QSortFilterProxyModel>
#include <QScrollArea>

#include <qwt_legend.h>

#include <algorithm>

using namespace OT;

namespace OTGUI
{

MorrisResultWidget::MorrisResultWidget(MorrisResult &result, const int outIndex, QWidget *parent)
  : QWidget(parent)
  , result_(result)
  , outpuIndex_(outIndex)
  , markersMuSigma_()
  , markersMuMu_()
  , fakeSelectedMarkers_(2)
  , fakeUnselectedMarkers_(2)
{
  buildInterface();
}


void MorrisResultWidget::buildInterface()
{
  // get result info
  const UnsignedInteger nbInVar = result_.getMeanElementaryEffects(outpuIndex_).getSize();

  Point a = result_.getMeanAbsoluteElementaryEffects(outpuIndex_);
  std::pair<Collection<Scalar>::iterator, Collection<Scalar>::iterator> p = std::minmax_element(a.begin(), a.end());
  const Scalar maxMeanStarEffect = (*p.second);

  a = result_.getMeanElementaryEffects(outpuIndex_);
  p = std::minmax_element(a.begin(), a.end());
  const Scalar maxMeanEffect = (*p.second);

  a = result_.getStandardDeviationElementaryEffects(outpuIndex_);
  p = std::minmax_element(a.begin(), a.end());
  const Scalar maxStdEffect = (*p.second);

  const Scalar noEffectBoundary = result_.getNoEffectBoundary(outpuIndex_);

  // - var names
  const QStringList names(QtOT::DescriptionToStringList(result_.getDesignOfExperiment().getInputSample().getDescription()));

  // main layout
  QVBoxLayout * mainLayout = new QVBoxLayout(this);

  // right splitter
  QSplitter * mainSplitter = new QSplitter(Qt::Vertical);
  mainLayout->addWidget(mainSplitter);

  // first widget of mainSplitter : tabWidget
  QTabWidget * tabWidget = new QTabWidget;
  mainSplitter->addWidget(tabWidget);
  mainSplitter->setStretchFactor(0, 3);

  // tab plot (µ*, σ)
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  WidgetBoundToDockWidget * tab = new WidgetBoundToDockWidget(this);
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  PlotWidget * plotMuStarSigma = new PlotWidget(tr("morrisResult"));
  connect(this, SIGNAL(plotItemsChanged()), plotMuStarSigma, SLOT(replot()));

  GraphConfigurationWidget * graphSetting = new GraphConfigurationWidget(plotMuStarSigma,
      QStringList(),
      QStringList(),
      GraphConfigurationWidget::NoType,
      this);

  tab->setDockWidget(graphSetting);

  tabLayout->addWidget(plotMuStarSigma);
  scrollArea->setWidget(tab);
  tabWidget->addTab(scrollArea, tr("Graph (µ*, σ)"));

  // tab plot (µ*, µ)
  scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  tab = new WidgetBoundToDockWidget(this);
  tabLayout = new QVBoxLayout(tab);

  PlotWidget * plotMuStarMu = new PlotWidget(tr("morrisResultMuStarMu"));
  connect(this, SIGNAL(plotItemsChanged()), plotMuStarMu, SLOT(replot()));

  graphSetting = new GraphConfigurationWidget(plotMuStarMu,
      QStringList(),
      QStringList(),
      GraphConfigurationWidget::NoType,
      this);
  tab->setDockWidget(graphSetting);

  tabLayout->addWidget(plotMuStarMu);
  scrollArea->setWidget(tab);
  tabWidget->addTab(scrollArea, tr("Graph (µ*, µ)"));

  // set plot (µ*, σ)
  plotMuStarSigma->setTitle(tr("Standard deviation (σ) and mean of the absolute value (µ*) of the elementary effects"));
  plotMuStarSigma->setAxisTitle(QwtPlot::xBottom, tr("µ*"));
  plotMuStarSigma->setAxisTitle(QwtPlot::yLeft, tr("σ"));
  plotMuStarSigma->setMorrisPlotType(QPointF(noEffectBoundary, 0));
  QwtLegend * legend = new QwtLegend;
  legend->setToolTip(tr("Coefficient of variation cv = σ / µ*"));
  plotMuStarSigma->insertLegend(legend, QwtPlot::RightLegend);
  plotMuStarSigma->setContextMenuPolicy(Qt::NoContextMenu);
  connect(plotMuStarSigma, SIGNAL(selectedPointsChanged()), this, SLOT(updateSelectedPointsFromMuSigma()));
  connect(plotMuStarSigma, SIGNAL(verticalMarkerPositionChanged(double)), this, SLOT(updateNoEffectBoundary(double)));
  connect(this, SIGNAL(noEffectBoundaryChanged(QPointF)), plotMuStarSigma, SLOT(updateVerticalMarkerValue(QPointF)));

  // - curve cv = 1
  QVector<QPointF> points1(2);
  const Scalar maxPoint = std::max(maxStdEffect, maxMeanStarEffect) * 1.02;
  points1[0] = QPointF(0., 0.);
  points1[1] = QPointF(maxPoint, maxPoint);
  plotMuStarSigma->plotCurve(points1, QPen(Qt::blue), QwtPlotCurve::Lines, 0, tr("cv = 1"), true);
  // - curve cv = 0.5
  points1[1] = QPointF(maxPoint, 0.5 * maxPoint);
  plotMuStarSigma->plotCurve(points1, QPen(Qt::blue, 1, Qt::DashDotLine), QwtPlotCurve::Lines, 0, tr("cv = 0.5"), true);

  // - two fake markers to add items in the legend
  // -- first fake blue marker
  QwtPlotMarker * fakeMarker = new QwtPlotMarker;
  fakeMarker->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, QPen(Qt::blue), QSize(5, 5)));
  fakeMarker->setItemAttribute(QwtPlotItem::Legend, true);
  fakeMarker->attach(plotMuStarSigma);
  fakeMarker->setVisible(false);
  fakeSelectedMarkers_[0] = fakeMarker;
  // -- second fake red marker
  fakeMarker = new QwtPlotMarker;
  fakeMarker->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, QPen(Qt::red), QSize(5, 5)));
  fakeMarker->setItemAttribute(QwtPlotItem::Legend, true);
  fakeMarker->attach(plotMuStarSigma);
  fakeMarker->setVisible(false);
  fakeUnselectedMarkers_[0] = fakeMarker;

  // set plot (µ*, µ)
  plotMuStarMu->setTitle(tr("Mean (µ) and mean of the absolute value (µ*) of the elementary effects"));
  plotMuStarMu->setAxisTitle(QwtPlot::xBottom, tr("µ*"));
  plotMuStarMu->setAxisTitle(QwtPlot::yLeft, tr("µ"));
  plotMuStarMu->setMorrisPlotType(QPointF(noEffectBoundary, 0));
  plotMuStarMu->insertLegend(new QwtLegend, QwtPlot::RightLegend);
  plotMuStarMu->setContextMenuPolicy(Qt::NoContextMenu);
  connect(plotMuStarMu, SIGNAL(selectedPointsChanged()), this, SLOT(updateSelectedPointsFromMuMu()));
  connect(plotMuStarMu, SIGNAL(verticalMarkerPositionChanged(double)), this, SLOT(updateNoEffectBoundary(double)));
  connect(this, SIGNAL(noEffectBoundaryChanged(QPointF)), plotMuStarMu, SLOT(updateVerticalMarkerValue(QPointF)));

  // - curve µ = µ*
  QVector<QPointF> points(2);
  const Scalar maxPoint2 = std::max(maxMeanEffect, maxMeanStarEffect) * 1.02;
  points[0] = QPointF(0., 0.);
  points[1] = QPointF(maxPoint2, maxPoint2);
  plotMuStarMu->plotCurve(points, QPen(Qt::blue), QwtPlotCurve::Lines, 0, tr("µ = ± µ*"), true);
  // - curve µ = 0.5 µ*
  points[1] = QPointF(maxPoint2, maxPoint2 * 0.5);
  plotMuStarMu->plotCurve(points, QPen(Qt::blue, 1, Qt::DashDotLine), QwtPlotCurve::Lines, 0, tr("µ = ± 0.5 µ*"), true);
  // - curve µ = -µ*
  points[1] = QPointF(maxPoint2, - maxPoint2);
  plotMuStarMu->plotCurve(points, QPen(Qt::blue), QwtPlotCurve::Lines, 0, "", true);
  // - curve µ = -0.5 µ*
  points[1] = QPointF(maxPoint2, - maxPoint2 * 0.5);
  plotMuStarMu->plotCurve(points, QPen(Qt::blue, 1, Qt::DashDotLine), QwtPlotCurve::Lines, 0, "", true);

  // - two fake markers to add items in the legend
  // -- first fake blue marker
  fakeMarker = new QwtPlotMarker;
  fakeMarker->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, QPen(Qt::blue), QSize(5, 5)));
  fakeMarker->setItemAttribute(QwtPlotItem::Legend, true);
  fakeMarker->attach(plotMuStarMu);
  fakeMarker->setVisible(false);
  fakeSelectedMarkers_[1] = fakeMarker;
  // -- second fake red marker
  fakeMarker = new QwtPlotMarker;
  fakeMarker->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, QPen(Qt::red), QSize(5, 5)));
  fakeMarker->setItemAttribute(QwtPlotItem::Legend, true);
  fakeMarker->attach(plotMuStarMu);
  fakeMarker->setVisible(false);
  fakeUnselectedMarkers_[1] = fakeMarker;

  // points markers for plot (µ*, σ) and (µ*, µ)
  for (UnsignedInteger i = 0; i < nbInVar; ++i)
  {
    const QPen markerPen = (result_.getInputsSelection(outpuIndex_)[i] > 0 ? QPen(Qt::blue) : QPen(Qt::red));

    // markers graph (µ*, σ)
    const QPointF pt(result_.getMeanAbsoluteElementaryEffects(outpuIndex_)[i], result_.getStandardDeviationElementaryEffects(outpuIndex_)[i]);
    QwtPlotMarker * marker = new QwtPlotMarker;
    markersMuSigma_.append(marker);
    marker->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
    marker->setValue(pt);
    marker->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, markerPen, QSize(5, 5)));
    marker->setLabel(names[i]);
    marker->attach(plotMuStarSigma);

    // markers graph (µ*, µ)
    const QPointF pt2(result_.getMeanAbsoluteElementaryEffects(outpuIndex_)[i], result_.getMeanElementaryEffects(outpuIndex_)[i]);
    QwtPlotMarker * marker2 = new QwtPlotMarker;
    markersMuMu_.append(marker2);
    marker2->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
    marker2->setValue(pt2);
    marker2->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, markerPen, QSize(5, 5)));
    marker2->setLabel(names[i]);
    marker2->attach(plotMuStarMu);
  }

  // update labels with number of un/selected points
  updateLabels();

  // rescale plots
  QList<double> minorTicks(plotMuStarSigma->axisScaleDiv(QwtPlot::yLeft).ticks(QwtScaleDiv::MinorTick));
  double step = minorTicks[1] - minorTicks[0];
  plotMuStarSigma->setAxisScale(QwtPlot::yLeft, - 2 * step, maxPoint);

  minorTicks = plotMuStarSigma->axisScaleDiv(QwtPlot::xBottom).ticks(QwtScaleDiv::MinorTick);
  step = minorTicks[1] - minorTicks[0];
  plotMuStarSigma->setAxisScale(QwtPlot::xBottom, - 1 * step, maxMeanStarEffect + 3 * step);

  minorTicks = plotMuStarMu->axisScaleDiv(QwtPlot::yLeft).ticks(QwtScaleDiv::MinorTick);
  plotMuStarMu->setAxisScale(QwtPlot::xBottom, - 1 * step, maxMeanStarEffect + 3 * step);
  step = minorTicks[1] - minorTicks[0];
  const Scalar maxY = std::max(maxMeanEffect, maxMeanStarEffect);
  plotMuStarMu->setAxisScale(QwtPlot::yLeft, -maxY - 1 * step, maxY + 1 * step);

  // second widget of the splitter : tableView

  // table view
  CopyableTableView * tableView = new CopyableTableView;
  CheckableHeaderView * outputTableHeaderView = new CheckableHeaderView;
  tableView->setHorizontalHeader(outputTableHeaderView);
  tableView->horizontalHeader()->setStretchLastSection(true);
  tableView->verticalHeader()->hide();
  tableView->setSortingEnabled(true);

  // table model
  MorrisResultTableModel * model = new MorrisResultTableModel(result_, outpuIndex_, MorrisResultTableModel::Edition, tableView);
  connect(this, SIGNAL(resetTableModel()), model, SLOT(updateData()));
  connect(model, SIGNAL(selectionChanged()), this, SLOT(updateSelectedPointsFromTable()));

  // QSortFilterProxyModel
  QSortFilterProxyModel * proxyModel = new QSortFilterProxyModel(tableView);
  proxyModel->setSourceModel(model);
  proxyModel->setSortRole(Qt::UserRole);

  tableView->setModel(proxyModel);
  tableView->resizeColumnsToContents();

  mainSplitter->addWidget(tableView);
  mainSplitter->setStretchFactor(1, 2);
}


void MorrisResultWidget::updateSelectedPointsFromTable()
{
  for (UnsignedInteger i = 0; i < result_.getInputsSelection(outpuIndex_).getSize(); ++i)
  {
    const QPen markerPen = result_.getInputsSelection(outpuIndex_)[i] > 0 ? QPen(Qt::blue) : QPen(Qt::red);
    markersMuSigma_[i]->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, markerPen, QSize(5, 5)));
    markersMuMu_[i]->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, markerPen, QSize(5, 5)));
  }
  updateLabels();
}


void MorrisResultWidget::updateSelectedPointsFromMuSigma()
{
  Indices selection(markersMuSigma_.size());
  for (int i = 0; i < markersMuSigma_.size(); ++i)
  {
    selection[i] = markersMuSigma_[i]->symbol()->pen().color() == Qt::red ? 0 : 1;
    const QPen markerPen = selection[i] > 0 ? QPen(Qt::blue) : QPen(Qt::red);
    markersMuMu_[i]->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, markerPen, QSize(5, 5)));
  }
  result_.setInputsSelection(0, selection);
  emit resetTableModel();
  updateLabels();
}


void MorrisResultWidget::updateSelectedPointsFromMuMu()
{
  Indices selection(markersMuMu_.size());
  for (int i = 0; i < markersMuMu_.size(); ++i)
  {
    selection[i] = markersMuMu_[i]->symbol()->pen().color() == Qt::red ? 0 : 1;
    const QPen markerPen = selection[i] > 0 ? QPen(Qt::blue) : QPen(Qt::red);
    markersMuSigma_[i]->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, markerPen, QSize(5, 5)));
  }
  result_.setInputsSelection(0, selection);
  emit resetTableModel();
  updateLabels();
}


void MorrisResultWidget::updateNoEffectBoundary(const double value)
{
  result_.setNoEffectBoundary(outpuIndex_, value);

  Indices selection(markersMuSigma_.size());
  for (int i = 0; i < markersMuSigma_.size(); ++i)
  {
    if (markersMuSigma_[i]->xValue() <= value)
    {
      markersMuSigma_[i]->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, QPen(Qt::red), QSize(5, 5)));
      markersMuMu_[i]->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, QPen(Qt::red), QSize(5, 5)));
    }
    else
    {
      markersMuSigma_[i]->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, QPen(Qt::blue), QSize(5, 5)));
      markersMuMu_[i]->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, QPen(Qt::blue), QSize(5, 5)));
    }
    selection[i] = markersMuSigma_[i]->xValue() <= value ? 0 : 1;
  }
  result_.setInputsSelection(0, selection);

  emit resetTableModel();
  emit noEffectBoundaryChanged(QPointF(value, 0));
  updateLabels();
}


void MorrisResultWidget::updateLabels()
{
  UnsignedInteger selectedCounter = 0;
  UnsignedInteger unselectedCounter = 0;
  for (int i = 0; i < markersMuMu_.size(); ++i)
  {
    if (markersMuMu_[i]->symbol()->pen().color() == Qt::blue)
      ++selectedCounter;
    else
      ++unselectedCounter;
  }
  fakeSelectedMarkers_[0]->setTitle(tr("Selected points (%1)").arg(selectedCounter));
  fakeSelectedMarkers_[1]->setTitle(tr("Selected points (%1)").arg(selectedCounter));
  fakeUnselectedMarkers_[0]->setTitle(tr("Unselected points (%1)").arg(unselectedCounter));
  fakeUnselectedMarkers_[1]->setTitle(tr("Unselected points (%1)").arg(unselectedCounter));
  emit plotItemsChanged();
}
}
