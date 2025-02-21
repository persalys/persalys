//                                               -*- C++ -*-
/**
 *  @brief QWidget for Morris analysis result
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "persalys/MorrisResultWidget.hxx"

#include "persalys/MorrisPlot.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/CheckableHeaderView.hxx"
#include "persalys/CopyableTableView.hxx"
#include "persalys/MorrisResultTableModel.hxx"
#include "persalys/WidgetBoundToDockWidget.hxx"
#include "persalys/GraphConfigurationWidget.hxx"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QSortFilterProxyModel>
#include <QScrollArea>

#include <qwt_legend.h>
#include <qwt_slider.h>

#include <algorithm>

using namespace OT;

namespace PERSALYS
{

MorrisResultWidget::MorrisResultWidget(MorrisResult &result, const int outIndex, QWidget *parent)
  : QWidget(parent)
  , result_(result)
  , outputIndex_(outIndex)
  , markersMuSigma_()
  , markersMuMu_()
{
  buildInterface();
}


void MorrisResultWidget::buildInterface()
{
  // get result info
  const UnsignedInteger nbInVar = result_.getMeanElementaryEffects(outputIndex_).getSize();

  Point a = result_.getMeanAbsoluteElementaryEffects(outputIndex_);
  std::pair<Collection<Scalar>::iterator, Collection<Scalar>::iterator> p = std::minmax_element(a.begin(), a.end());
  const Scalar maxMeanStarEffect = (*p.second);

  a = result_.getMeanElementaryEffects(outputIndex_);
  p = std::minmax_element(a.begin(), a.end());
  const Scalar maxMeanEffect = (*p.second);

  a = result_.getStandardDeviationElementaryEffects(outputIndex_);
  p = std::minmax_element(a.begin(), a.end());
  const Scalar maxStdEffect = (*p.second);

  const Scalar noEffectBoundary = result_.getNoEffectBoundary(outputIndex_);

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

  MorrisPlot * plotMuStarSigma = new MorrisPlot(tr("morrisResult"), QPointF(noEffectBoundary, 0));
  connect(this, SIGNAL(plotItemsChanged()), plotMuStarSigma, SLOT(replot()));

  SimpleGraphSetting * graphSetting = new SimpleGraphSetting(plotMuStarSigma, this);

  scrollArea->setWidget(new WidgetBoundToDockWidget(plotMuStarSigma, graphSetting, this));
  tabWidget->addTab(scrollArea, tr("Graph (µ*, σ)"));

  // tab plot (µ*, µ)
  scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  MorrisPlot * plotMuStarMu = new MorrisPlot(tr("morrisResultMuStarMu"), QPointF(noEffectBoundary, 0));
  connect(this, SIGNAL(plotItemsChanged()), plotMuStarMu, SLOT(replot()));

  graphSetting = new SimpleGraphSetting(plotMuStarMu, this);

  scrollArea->setWidget(new WidgetBoundToDockWidget(plotMuStarMu, graphSetting, this));
  tabWidget->addTab(scrollArea, tr("Graph (µ*, µ)"));

  // set plot (µ*, σ)
  plotMuStarSigma->setTitle(tr("Standard deviation (σ) and mean of the absolute value (µ*) of the elementary effects"));
  plotMuStarSigma->setAxisTitle(QwtPlot::yLeft, tr("σ"));
  plotMuStarSigma->legend()->setToolTip(tr("Coefficient of variation cv = σ / µ*"));
  connect(plotMuStarSigma, SIGNAL(selectedPointsChanged()), this, SLOT(updateSelectedPointsFromMuSigma()));
  connect(this, SIGNAL(noEffectBoundaryChanged(QPointF)), plotMuStarSigma, SLOT(updateVerticalMarkerValue(QPointF)));
  connect(this, &MorrisResultWidget::updateSelectedMarkerRequested, plotMuStarSigma, &MorrisPlot::updateFakeSelectedMarkerTitle);
  connect(this, &MorrisResultWidget::updateUnselectedMarkerRequested, plotMuStarSigma, &MorrisPlot::updateFakeUnSelectedMarkerTitle);

  // - curve cv = 1
  QVector<QPointF> points1(2);
  const Scalar maxPoint = std::max(maxStdEffect, maxMeanStarEffect) * 1.02;
  points1[0] = QPointF(0., 0.);
  points1[1] = QPointF(maxPoint, maxPoint);
  plotMuStarSigma->plotCurve(points1, QPen(Qt::blue), QwtPlotCurve::Lines, 0, tr("cv = 1"), true);
  // - curve cv = 0.5
  points1[1] = QPointF(maxPoint, 0.5 * maxPoint);
  plotMuStarSigma->plotCurve(points1, QPen(Qt::blue, 1, Qt::DashDotLine), QwtPlotCurve::Lines, 0, tr("cv = 0.5"), true);

  // set plot (µ*, µ)
  plotMuStarMu->setTitle(tr("Mean (µ) and mean of the absolute value (µ*) of the elementary effects"));
  plotMuStarMu->setAxisTitle(QwtPlot::yLeft, tr("µ"));
  connect(plotMuStarMu, SIGNAL(selectedPointsChanged()), this, SLOT(updateSelectedPointsFromMuMu()));
  connect(this, SIGNAL(noEffectBoundaryChanged(QPointF)), plotMuStarMu, SLOT(updateVerticalMarkerValue(QPointF)));
  connect(this, &MorrisResultWidget::updateSelectedMarkerRequested, plotMuStarMu, &MorrisPlot::updateFakeSelectedMarkerTitle);
  connect(this, &MorrisResultWidget::updateUnselectedMarkerRequested, plotMuStarMu, &MorrisPlot::updateFakeUnSelectedMarkerTitle);

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

  // points markers for plot (µ*, σ) and (µ*, µ)
  for (UnsignedInteger i = 0; i < nbInVar; ++i)
  {
    const QPen markerPen = (result_.getInputsSelection(outputIndex_)[i] > 0 ? QPen(Qt::blue) : QPen(Qt::red));

    // markers graph (µ*, σ)
    const QPointF pt(result_.getMeanAbsoluteElementaryEffects(outputIndex_)[i], result_.getStandardDeviationElementaryEffects(outputIndex_)[i]);
    markersMuSigma_.append(new PlotMarker(plotMuStarSigma, markerPen, pt, names[i]));

    // markers graph (µ*, µ)
    const QPointF pt2(result_.getMeanAbsoluteElementaryEffects(outputIndex_)[i], result_.getMeanElementaryEffects(outputIndex_)[i]);
    markersMuMu_.append(new PlotMarker(plotMuStarMu, markerPen, pt2, names[i]));
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


  // second widget of the splitter : Slider

  QwtSlider* muStarSlider = new QwtSlider(Qt::Horizontal, this);
  muStarSlider->setScalePosition(QwtSlider::LeadingScale);
  muStarSlider->setScale(0, noEffectBoundary / 0.05);
  muStarSlider->setTotalSteps(100);
  muStarSlider->setValue(noEffectBoundary);
  mainSplitter->addWidget(muStarSlider);

  connect(muStarSlider, SIGNAL(valueChanged(double)),
          this, SLOT(updateNoEffectBoundary(double)));


  // third widget of the splitter : tableView

  // table view
  CopyableTableView * tableView = new CopyableTableView;
  CheckableHeaderView * outputTableHeaderView = new CheckableHeaderView;
  tableView->setHorizontalHeader(outputTableHeaderView);
  tableView->horizontalHeader()->setStretchLastSection(true);
  tableView->verticalHeader()->hide();
  tableView->setSortingEnabled(true);

  // table model
  MorrisResultTableModel * model = new MorrisResultTableModel(result_, outputIndex_, MorrisResultTableModel::Edition, tableView);
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
  for (UnsignedInteger i = 0; i < result_.getInputsSelection(outputIndex_).getSize(); ++i)
  {
    const QPen markerPen = result_.getInputsSelection(outputIndex_)[i] > 0 ? QPen(Qt::blue) : QPen(Qt::red);
    markersMuSigma_[i]->updateSymbolColor(markerPen);
    markersMuMu_[i]->updateSymbolColor(markerPen);
  }
  updateLabels();
}


void MorrisResultWidget::updateSelectedPointsFromMuSigma()
{
  Indices selection(markersMuSigma_.size());
  for (int i = 0; i < markersMuSigma_.size(); ++i)
  {
    selection[i] = markersMuSigma_[i]->symbol()->pen().color() == Qt::red ? 0 : 1;
    markersMuMu_[i]->updateSymbolColor(selection[i] > 0 ? QPen(Qt::blue) : QPen(Qt::red));
  }
  result_.setInputsSelection(outputIndex_, selection);
  emit resetTableModel();
  updateLabels();
}


void MorrisResultWidget::updateSelectedPointsFromMuMu()
{
  Indices selection(markersMuMu_.size());
  for (int i = 0; i < markersMuMu_.size(); ++i)
  {
    selection[i] = markersMuMu_[i]->symbol()->pen().color() == Qt::red ? 0 : 1;
    markersMuSigma_[i]->updateSymbolColor(selection[i] > 0 ? QPen(Qt::blue) : QPen(Qt::red));
  }
  result_.setInputsSelection(outputIndex_, selection);
  emit resetTableModel();
  updateLabels();
}


void MorrisResultWidget::updateNoEffectBoundary(const double value)
{
  result_.setNoEffectBoundary(outputIndex_, value);

  Indices selection(markersMuSigma_.size());
  for (int i = 0; i < markersMuSigma_.size(); ++i)
  {
    selection[i] = markersMuSigma_[i]->xValue() <= value ? 0 : 1;
    markersMuSigma_[i]->updateSymbolColor(selection[i] > 0 ? QPen(Qt::blue) : QPen(Qt::red));
    markersMuMu_[i]->updateSymbolColor(selection[i] > 0 ? QPen(Qt::blue) : QPen(Qt::red));
  }
  result_.setInputsSelection(outputIndex_, selection);

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
  emit updateSelectedMarkerRequested(tr("Selected points (%1)").arg(selectedCounter));
  emit updateUnselectedMarkerRequested(tr("Unselected points (%1)").arg(unselectedCounter));
  emit plotItemsChanged();
}
}
