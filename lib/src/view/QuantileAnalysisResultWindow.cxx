//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of quantile analysis
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
#include "persalys/QuantileAnalysisResultWindow.hxx"

#include "persalys/QuantileAnalysis.hxx"
#include "persalys/ExportableTableView.hxx"
#include "persalys/CustomStandardItemModel.hxx"
#include "persalys/ResizableStackedWidget.hxx"
#include "persalys/PlotWidget.hxx"
#include "persalys/GraphConfigurationWidget.hxx"
#include "persalys/WidgetBoundToDockWidget.hxx"

#include <openturns/UserDefined.hxx>

#include <QSplitter>
#include <QVBoxLayout>
#include <QHeaderView>

using namespace OT;

namespace PERSALYS
{

  QuantileAnalysisResultWindow::QuantileAnalysisResultWindow(AnalysisItem * item, QWidget * parent)
    : ResultWindow(item, parent)
  {
    const QuantileAnalysis * analysis = dynamic_cast<const QuantileAnalysis*>(item->getAnalysis().getImplementation().get());
    if (!analysis)
      throw InvalidArgumentException(HERE) << "QuantileAnalysisResultWindow: the analysis is not a QuantileAnalysis";

    result_ = analysis->getResult();
    // title
    titleLabel_ = new TitleLabel(tr("Quantile analysis"), "user_manual/graphical_interface/data_analysis/user_manual_data_analysis.html#quantileresult");

    buildInterface(analysis->getInterestVariables());
  }


  void QuantileAnalysisResultWindow::buildInterface(const Description & varNames)
  {
    QVBoxLayout * widgetLayout = new QVBoxLayout(this);

    widgetLayout->addWidget(titleLabel_);

    // main splitter
    QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

    // - list outputs
    QGroupBox * outputsGroupBox = new QGroupBox(tr("Output"));
    QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);

    variablesListWidget_ = new VariablesListWidget;
    variablesListWidget_->addItems(QStringList() << QtOT::DescriptionToStringList(varNames));
    variablesListWidget_->setCurrentRow(0);
    outputsLayoutGroupBox->addWidget(variablesListWidget_);

    mainWidget->addWidget(outputsGroupBox);
    mainWidget->setStretchFactor(0, 1);

    tabWidget_ = new QTabWidget;
    fillTabWidget();

    mainWidget->addWidget(tabWidget_);
    mainWidget->setStretchFactor(1, 10);

    widgetLayout->addWidget(mainWidget, 1);

  }

  void QuantileAnalysisResultWindow::fillTabWidget()
  {
    if (result_.getDesignOfExperiment().getSample().getSize() > 1)
    {
      // tab: QuantileTable
      addQuantileTab();
      // tab: CDF/QuantilePlots
      addPlotTab();
    }
  }

  void QuantileAnalysisResultWindow::addQuantileTab()
  {
    ResizableStackedWidget * tabStackedWidget = new ResizableStackedWidget;
    connect(variablesListWidget_, SIGNAL(currentRowChanged(int)),
            tabStackedWidget, SLOT(setCurrentIndex(int)));

    const AnalysisItem * item_ptr = dynamic_cast<const AnalysisItem*>(getItem());
    if (!item_ptr)
      throw InvalidArgumentException(HERE) << "QuantileAnalysisResultWindow::addQuantileTab the item is not an AnalysisItem";

    const QuantileAnalysis * analysis = dynamic_cast<const QuantileAnalysis*>(item_ptr->getAnalysis().getImplementation().get());

    const Collection<Point> targetProbas = analysis->getTargetProbabilities();
    const Indices tailTypes = analysis->getTailTypes();
    for (UnsignedInteger iMarg = 0; iMarg < analysis->getInterestVariables().getSize(); ++iMarg)
    {
      const String varName = analysis->getInterestVariables()[iMarg];
      Collection<Sample> quantiles = result_.getQuantiles(varName);
      QWidget * tabWidget = new QWidget;
      QVBoxLayout * tabLayout = new QVBoxLayout(tabWidget);

      // table widget
      ExportableTableView * tableView = new ExportableTableView;
      tableView->horizontalHeader()->hide();
      tableView->verticalHeader()->hide();

      // Reorder data
      Collection<Sample> ordered = Collection(targetProbas[iMarg].getSize(), Sample(0,3));
      int nLines = 1;
      for (UnsignedInteger iTail = 0; iTail < quantiles.getSize(); ++iTail)
      {
        for (UnsignedInteger iProba = 0; iProba < targetProbas[iMarg].getSize(); ++iProba)
        {
          ordered[iProba].add(quantiles[iTail][iProba]);
          nLines++;
        }
      }

      // account each individual tail type
      // also need real proba for probaStr generation
      Indices fullTailTypes;
      Indices probaModifier;
      if (tailTypes[iMarg] & QuantileAnalysisResult::Lower)
      {
        fullTailTypes.add(QuantileAnalysisResult::Lower);
        probaModifier.add(1);
      }
      if (tailTypes[iMarg] & QuantileAnalysisResult::Upper)
      {
        fullTailTypes.add(QuantileAnalysisResult::Upper);
        probaModifier.add(2);
      }
      if (tailTypes[iMarg] & QuantileAnalysisResult::Bilateral)
      {
        fullTailTypes.add(QuantileAnalysisResult::Lower);
        fullTailTypes.add(QuantileAnalysisResult::Upper);
        probaModifier.add(3);
        probaModifier.add(4);
      }

      CustomStandardItemModel * tableModel;
      tableModel = new CustomStandardItemModel(nLines, 4, tableView);

      tableView->setModel(tableModel);
      tableModel->setNotEditableHeaderItem(0, 0, tr("Target probability"));
      tableModel->setNotEditableHeaderItem(0, 1, tr("Type"));
      tableModel->setNotEditableHeaderItem(0, 2, tr("Quantile"));
      tableModel->setNotEditableHeaderItem(0, 3, tr("Confidence Interval @") + QString::number(analysis->getConfidenceIntervalLevel()*100) + "%");

      // fill the table
      int row = 1;
      for (UnsignedInteger i=0; i<ordered.getSize(); ++i)
      {
        int rowBlock = row;
        tableModel->setNotEditableItem(rowBlock, 0, QString::number(targetProbas[iMarg][i]));
        for (UnsignedInteger j=0; j<ordered[i].getSize(); ++j)
        {
          if (fullTailTypes.getSize() != ordered[i].getSize())
            throw InvalidArgumentException(HERE) << "Incompatible tail types and quantiles numbers.";

          Scalar proba;
          switch (probaModifier[j])
          {
          case 1:
            proba = targetProbas[iMarg][i];
            break;
          case 2:
            proba = 1. - targetProbas[iMarg][i];
            break;
          case 3:
            proba = 0.5*targetProbas[iMarg][i];
            break;
          case 4:
            proba = 1. - 0.5*targetProbas[iMarg][i];
            break;
          default:
            throw;
          }
          const int prec = (int)(std::abs(std::log10(targetProbas[iMarg][i])) + 2);
          const QString sign = fullTailTypes[j] == QuantileAnalysisResult::Lower ? " < q) = " : " > q) = ";
          const QString probaStr = "P(" + QString(varName.c_str()) + sign + QString::number(proba, 'g', prec);
          tableModel->setNotEditableItem(row, 1, probaStr);
          tableModel->setNotEditableItem(row, 2, QString::number(ordered[i][j][1]));
          tableModel->setNotEditableItem(row, 3, "[" + QString::number(ordered[i][j][0]) + ";" + QString::number(ordered[i][j][2]) + "]");
          row++;
        }
        // merge cells with at least 2 identical target proba
        if (fullTailTypes.getSize() > 1)
          tableView->setSpan(rowBlock, 0, fullTailTypes.getSize(), 1);
      }

      tableView->resizeColumnsToContents();
      tabLayout->addWidget(tableView);

      // validity for GDP
      if (analysis->getType() == QuantileAnalysisResult::GeneralizedPareto)
      {
        // table widget
        ExportableTableView * tableViewVal = new ExportableTableView;
        tableViewVal->horizontalHeader()->hide();
        tableViewVal->verticalHeader()->hide();

        CustomStandardItemModel * tableModelVal;
        if (fullTailTypes.getSize() == 1)
          tableModelVal = new CustomStandardItemModel(2, 2, tableView);
        else
          tableModelVal = new CustomStandardItemModel(3, 2, tableView);
        tableViewVal->setModel(tableModelVal);
        tableModelVal->setNotEditableHeaderItem(0, 0, tr("Tail type"));
        tableModelVal->setNotEditableHeaderItem(0, 1, tr("GPD P-value"));
        row = 1;
        if (tailTypes[iMarg] & QuantileAnalysisResult::Lower ||
            tailTypes[iMarg] & QuantileAnalysisResult::Bilateral)
        {
          tableModelVal->setNotEditableHeaderItem(row, 0, tr("Lower"));
          tableModelVal->setNotEditableItem(row++, 1, analysis->getResult().getPValue(varName, QuantileAnalysisResult::Lower));
        }
        if (tailTypes[iMarg] & QuantileAnalysisResult::Upper ||
            tailTypes[iMarg] & QuantileAnalysisResult::Bilateral)
        {
          tableModelVal->setNotEditableHeaderItem(row, 0, tr("Upper"));
          tableModelVal->setNotEditableItem(row++, 1, analysis->getResult().getPValue(varName, QuantileAnalysisResult::Upper));
        }

        tableViewVal->resizeColumnsToContents();
        tabLayout->addWidget(tableViewVal);
        tabLayout->setStretch(0, 4);
      }
      tabStackedWidget->addWidget(tabWidget);


    }
    //scrollArea->setWidget(tabStackedWidget);
    tabWidget_->addTab(tabStackedWidget, tr("Quantiles"));
  }

  void QuantileAnalysisResultWindow::addPlotTab()
  {
    ResizableStackedWidget * tabStackedWidget = new ResizableStackedWidget;
    connect(variablesListWidget_, SIGNAL(currentRowChanged(int)),
            tabStackedWidget, SLOT(setCurrentIndex(int)));

    const AnalysisItem * item_ptr = dynamic_cast<const AnalysisItem*>(getItem());
    if (!item_ptr)
      throw InvalidArgumentException(HERE) << "QuantileAnalysisResultWindow::addQuantileTab the item is not an AnalysisItem";

    QuantileAnalysis * analysis = dynamic_cast<QuantileAnalysis*>(item_ptr->getAnalysis().getImplementation().get());

    const Collection<Point> targetProbas = analysis->getTargetProbabilities();

    const Sample sample = result_.getDesignOfExperiment().getSample().getMarginal(analysis->getInterestVariables());

    const Scalar qmin = ResourceMap::GetAsScalar("Distribution-QMin");
    const Scalar qmax = ResourceMap::GetAsScalar("Distribution-QMax");
    const UnsignedInteger nPoints = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber");

    for (UnsignedInteger iMarg = 0; iMarg < sample.getDimension(); ++iMarg)
    {
      const String varName = sample.getDescription()[iMarg];
      const Collection<Sample> quantiles = result_.getQuantiles(varName);

      // tail type to decide which plot to draw
      int currentType = analysis->getTailTypes()[iMarg];
      const Bool drawLower = (currentType & QuantileAnalysisResult::Lower ||
                              currentType & QuantileAnalysisResult::Bilateral);
      const Bool drawUpper = (currentType & QuantileAnalysisResult::Upper ||
                              currentType & QuantileAnalysisResult::Bilateral);

      QWidget * tabWidget = new QWidget;
      QGridLayout * tabLayout = new QGridLayout(tabWidget);
      QComboBox * tailComboBox = new QComboBox;
      tabLayout->addWidget(tailComboBox, 0, 0, 1, 1);

      ResizableStackedWidget * stackedPlots = new ResizableStackedWidget;
      tabLayout->addWidget(stackedPlots, 1, 0, 1, 2);

      tabLayout->setColumnStretch(1, 3);
      connect(tailComboBox, SIGNAL(currentIndexChanged(int)),
              stackedPlots, SLOT(setCurrentIndex(int)));


      const Point::const_iterator minProba_it = std::min_element(targetProbas[iMarg].begin(),
                                                                 targetProbas[iMarg].end());
      Scalar minProba = *minProba_it * 0.5;
      if (currentType & QuantileAnalysisResult::Bilateral)
        minProba *= 0.5;

      // Get CDF x-range
      ResourceMap::SetAsScalar("Distribution-QMin", 0.5 * minProba);
      ResourceMap::SetAsScalar("Distribution-QMax", 1 - 0.5 * minProba);
      ResourceMap::SetAsUnsignedInteger("Distribution-DefaultPointNumber", 513);

      // get data
      const Distribution dist = UserDefined(sample.getMarginal(iMarg));
      const Sample rawData = dist.drawCDF().getDrawable(0).getData();

      // remove 0 for y-log scale
      // cdf
      Sample cleanedCdfData(0, 2);
      // 1 - cdf
      Sample cleanedSurData(0, 2);
      for (UnsignedInteger j=0; j<rawData.getSize(); ++j)
      {
        if (rawData(j, 1) != 0)
          cleanedCdfData.add(rawData[j]);
        if (rawData(j, 1) != 1)
        {
          Point point = Point(2);
          point[0] = rawData(j, 0);
          point[1] = 1 - rawData(j,1);
          cleanedSurData.add(point);
        }
      }

      // y-log scale
      QwtLogScaleEngine * ylogScaleLower = new QwtLogScaleEngine();
      QwtLogScaleEngine * ylogScaleUpper = new QwtLogScaleEngine();
      if (currentType & QuantileAnalysisResult::Bilateral)
        minProba *= 0.5;
      QwtScaleDiv yDivLower = ylogScaleLower->divideScale(minProba, 1., 2, 10, 1.0);
      QwtScaleDiv yDivUpper = ylogScaleUpper->divideScale(minProba, 1., 2, 10, 1.0);

      // plot empirical CDF/SurvFct
      PlotWidget * plotWidgetLower = new PlotWidget;
      plotWidgetLower->setAxisAutoScale(QwtPlot::yLeft, false);
      plotWidgetLower->setAxisScaleEngine(QwtPlot::yLeft, ylogScaleLower);
      plotWidgetLower->setAxisScaleDiv(QwtPlot::yLeft, yDivLower);


      if (drawLower)
        plotWidgetLower->plotCurve(cleanedCdfData, QPen(Qt::black, 2));

      PlotWidget * plotWidgetUpper = new PlotWidget;
      plotWidgetUpper->setAxisAutoScale(QwtPlot::yLeft, false);
      plotWidgetUpper->setAxisScaleEngine(QwtPlot::yLeft, ylogScaleUpper);
      plotWidgetUpper->setAxisScaleDiv(QwtPlot::yLeft, yDivUpper);

      if (drawUpper)
        plotWidgetUpper->plotCurve(cleanedSurData, QPen(Qt::black, 2));

      Point probaModifier;
      Indices fullTailTypes;
      if (currentType & QuantileAnalysisResult::Lower)
      {
        probaModifier.add(1.);
        fullTailTypes.add(QuantileAnalysisResult::Lower);
      }
      if (currentType & QuantileAnalysisResult::Upper)
      {
        probaModifier.add(1.);
        fullTailTypes.add(QuantileAnalysisResult::Upper);
      }
      if (currentType & QuantileAnalysisResult::Bilateral)
      {
        probaModifier.add(.5);
        fullTailTypes.add(QuantileAnalysisResult::Lower);
        probaModifier.add(.5);
        fullTailTypes.add(QuantileAnalysisResult::Upper);
      }

      if (fullTailTypes.getSize() != quantiles.getSize())
        throw InvalidArgumentException(HERE) << "QuantileAnalysisResultWindow::addPlotTab : Quantiles size incompatible with number of tails";

      // Add Q-/Q/Q+, and threshold/GPD cdf if any
      for (UnsignedInteger iTail = 0; iTail < quantiles.getSize(); ++iTail)
      {
        for (UnsignedInteger iProba = 0; iProba < targetProbas[iMarg].getSize(); ++iProba)
        {
          Sample qData = Sample::BuildFromPoint(quantiles[iTail][iProba]);
          qData.stack(Sample::BuildFromPoint(Point(3, targetProbas[iMarg][iProba] * probaModifier[iTail])));
          QwtSymbol* symbol = new QwtSymbol(QwtSymbol::Rect, QBrush(Qt::blue), QPen(Qt::blue), QSize(2, 6));
          if (fullTailTypes[iTail] == QuantileAnalysisResult::Lower)
          {

            plotWidgetLower->plotCurve(qData, QPen(Qt::blue, 2), QwtPlotCurve::Lines, symbol);
            if (analysis->getType() == QuantileAnalysisResult::GeneralizedPareto)
            {
              plotWidgetLower->plotCurve(analysis->plotGPD(iMarg, 0, minProba).getDrawable(0).getData(),
                                         QPen(Qt::red, 2));
              plotWidgetLower->plotScatter(Sample(1, Point(1, analysis->getThreshold()(0, iMarg))),
                                           Sample(1, Point(1, analysis->getCDFThreshold()(0, iMarg))),
                                           QPen(Qt::blue, 6));
            }
          }
          else if (fullTailTypes[iTail] == QuantileAnalysisResult::Upper)
          {
            plotWidgetUpper->plotCurve(qData, QPen(Qt::blue, 2), QwtPlotCurve::Lines, symbol);
            if (analysis->getType() == QuantileAnalysisResult::GeneralizedPareto)
            {
              plotWidgetUpper->plotCurve(analysis->plotGPD(iMarg, 1, minProba).getDrawable(0).getData(),
                                         QPen(Qt::red, 2));
              plotWidgetUpper->plotScatter(Sample(1, Point(1, analysis->getThreshold()(1, iMarg))),
                                           Sample(1, Point(1, 1.-analysis->getCDFThreshold()(1, iMarg))),
                                           QPen(Qt::blue, 6));
            }
          }
        }
      }


      if (drawLower)
      {
        tailComboBox->addItems(QStringList() << tr("Lower"));
        SimpleGraphSetting * graphSettingWidgetLower = new SimpleGraphSetting(plotWidgetLower, this);
        plotWidgetLower->setAxisTitle(QwtPlot::yLeft, QString("P(" + QString(varName.c_str()) + " < x)"));
        plotWidgetLower->setAxisTitle(QwtPlot::xBottom, QString(varName.c_str()));
        stackedPlots->addWidget(new WidgetBoundToDockWidget(plotWidgetLower, graphSettingWidgetLower, this));
      }
      if (drawUpper)
      {
        tailComboBox->addItems(QStringList() << tr("Upper"));
        SimpleGraphSetting * graphSettingWidgetUpper = new SimpleGraphSetting(plotWidgetUpper, this);
        plotWidgetUpper->setAxisTitle(QwtPlot::yLeft, QString("P(" + QString(varName.c_str()) + " > x)"));
        plotWidgetUpper->setAxisTitle(QwtPlot::xBottom, QString(varName.c_str()));
        stackedPlots->addWidget(new WidgetBoundToDockWidget(plotWidgetUpper, graphSettingWidgetUpper, this));
      }
      tabStackedWidget->addWidget(tabWidget);
    } // end for marginal

    ResourceMap::SetAsScalar("Distribution-QMin", qmin);
    ResourceMap::SetAsScalar("Distribution-QMax", qmax);
    ResourceMap::SetAsUnsignedInteger("Distribution-DefaultPointNumber", nPoints);

    tabWidget_->addTab(tabStackedWidget, tr("CDF"));

  }

}
