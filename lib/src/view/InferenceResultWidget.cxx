//                                               -*- C++ -*-
/**
 *  @brief QWidget for the inference results
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/InferenceResultWidget.hxx"

#include "otgui/ResizableStackedWidget.hxx"
#include "otgui/RadioButtonDelegate.hxx"

#include <openturns/VisualTest.hxx>

#include <QScrollArea>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QHeaderView>

using namespace OT;

namespace OTGUI {

InferenceResultWidget::InferenceResultWidget(const bool displayPDF_QQPlot, QWidget* parent)
  : QWidget(parent)
  , displayPDF_QQPlot_(displayPDF_QQPlot)
  , tabWidget_(0)
  , currentFittingTestResult_()
  , distTableView_(0)
  , distTableModel_(0)
  , distParamTableView_(0)
  , distParamTableModel_(0)
  , pdfPlot_(0)
  , cdfPlot_(0)
  , qqPlot_(0)
{
  buildInterface();
}


void InferenceResultWidget::buildInterface()
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  QWidget * distTabListWidget = new QWidget;
  QHBoxLayout * distTabListWidgetLayout = new QHBoxLayout(distTabListWidget);

  // -- distributions table
  QGroupBox * distGroupBox = new QGroupBox(tr("Distributions"));
  QVBoxLayout * distGroupBoxLayout = new QVBoxLayout(distGroupBox);

  // --- table view
  distTableView_ = new ResizableTableViewWithoutScrollBar;
  RadioButtonDelegate * delegate = new RadioButtonDelegate(2, distTableView_);
  distTableView_->setItemDelegateForColumn(0, delegate);
  distTableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
  distTableView_->verticalHeader()->hide();
  distTableView_->horizontalHeader()->hide();

  connect(distTableView_, SIGNAL(clicked(QModelIndex)), this, SLOT(updateParametersTable(QModelIndex)));
  connect(distTableView_, SIGNAL(clicked(QModelIndex)), this, SLOT(updateGraphs(QModelIndex)));
  connect(distTableView_, SIGNAL(clicked(QModelIndex)), this, SLOT(updateRadioButtonsDistributionTable(QModelIndex)));

  // --- table model
  distTableModel_ = new CustomStandardItemModel(0, 3, distTableView_);
  distTableView_->setModel(distTableModel_);

  distGroupBoxLayout->addWidget(distTableView_);
  distGroupBoxLayout->addStretch();

  distTabListWidgetLayout->addWidget(distGroupBox);

  // -- distribution parameters table
  // --- table view
  distParamTableView_ = new ResizableTableViewWithoutScrollBar;
  distParamTableView_->horizontalHeader()->hide();
  distParamTableView_->verticalHeader()->hide();

  // --- table model
  distParamTableModel_ = new CustomStandardItemModel(4, 2, distParamTableView_);
  distParamTableView_->setModel(distParamTableModel_);

  // -- pdf/cdf qqPlot
  if (displayPDF_QQPlot_)
  {
    tabWidget_ = new QTabWidget;

    // tab PDF/CDF
    ResizableStackedWidget * pdf_cdfStackedWidget = new ResizableStackedWidget;
    // --- pdf
    pdfPlot_ = new PlotWidget;
    pdf_cdfStackedWidget->addWidget(pdfPlot_);
    // --- cdf
    cdfPlot_ = new PlotWidget;
    pdf_cdfStackedWidget->addWidget(cdfPlot_);
    // --- GraphConfigurationWidget
    QVector<PlotWidget*> listpdf_cdfPlot;
    listpdf_cdfPlot.append(pdfPlot_);
    listpdf_cdfPlot.append(cdfPlot_);
    pdf_cdfPlotGraphConfigWidget_ = new GraphConfigurationWidget(listpdf_cdfPlot, QStringList(), QStringList(), GraphConfigurationWidget::PDF, this);
    pdf_cdfPlotGraphConfigWidget_->hide();
    connect(pdf_cdfPlotGraphConfigWidget_, SIGNAL(currentPlotChanged(int)), pdf_cdfStackedWidget, SLOT(setCurrentIndex(int)));

    tabWidget_->addTab(pdf_cdfStackedWidget, tr("PDF/CDF"));

    // tab QQ plot
    QWidget * qqPlotTab = new QWidget;
    QHBoxLayout * qqPlotTabLayout = new QHBoxLayout(qqPlotTab);

    // --- qq plot
    qqPlot_ = new PlotWidget;
    qqPlotTabLayout->addWidget(qqPlot_);
    // --- GraphConfigurationWidget
    QVector<PlotWidget*> listPlot;
    listPlot.append(qqPlot_);
    qqPlotGraphConfigWidget_ = new GraphConfigurationWidget(listPlot, QStringList(), QStringList(), GraphConfigurationWidget::NoType, this);
    qqPlotGraphConfigWidget_->hide();

    tabWidget_->addTab(qqPlotTab, tr("Q-Q Plot"));

    // tab Parameters
    QWidget * paramWidget = new QWidget;
    QVBoxLayout * paramGroupBoxLayout = new QVBoxLayout(paramWidget);
    paramGroupBoxLayout->addWidget(distParamTableView_);
    tabWidget_->addTab(distParamTableView_, tr("Parameters"));

    connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));

    distTabListWidgetLayout->addWidget(tabWidget_);
  }
  else
  {
    QGroupBox * paramGroupBox = new QGroupBox(tr("Distribution parameters"));
    QVBoxLayout * paramGroupBoxLayout = new QVBoxLayout(paramGroupBox);
    paramGroupBoxLayout->addWidget(distParamTableView_);
    paramGroupBoxLayout->addStretch();
    distTabListWidgetLayout->setSizeConstraint(QLayout::SetFixedSize);
    distTabListWidgetLayout->addWidget(paramGroupBox);
  }

  scrollArea->setWidget(distTabListWidget);
  mainLayout->addWidget(scrollArea);
}


void InferenceResultWidget::updateDistributionTable(const InferenceResult& result, const QString& variableName)
{
  // check
  if (!(distTableModel_ && distTableView_))
    return;
  if (variableName.isEmpty())
    return;

  // reset
  distTableModel_->clear();

  // horizontal header
  distTableModel_->setNotEditableHeaderItem(0, 0, tr("Distributions"));
  distTableModel_->setNotEditableHeaderItem(0, 1, tr("Kolmogorov Smirnov"));
  distTableModel_->setNotEditableHeaderItem(1, 1, tr("p-value"));
  distTableModel_->setNotEditableHeaderItem(1, 2, tr("Acceptation"));
  distTableView_->setSpan(0, 0, 2, 1);

  // table
  // -- get results of the variable
  currentFittingTestResult_ = result.getFittingTestResultForVariable(variableName.toStdString());

  NumericalPoint pValues(currentFittingTestResult_.getKolmogorovTestResults().getSize());
  for (UnsignedInteger i=0; i<currentFittingTestResult_.getKolmogorovTestResults().getSize(); ++i)
    pValues[i] = currentFittingTestResult_.getKolmogorovTestResults()[i].getPValue();

  // -- sort indices list
  Indices indices(pValues.getSize());
  if (pValues.getSize() > 1)
  {
    indices.fill();
    for (int i=(pValues.getSize()-1); i>=0; --i)
    {
      for (int l=1; l<=i; ++l)
      {
        if (pValues[l-1] < pValues[l])
        {
          NumericalScalar temp = pValues[l-1];
          pValues[l-1] = pValues[l];
          pValues[l] = temp;
          UnsignedInteger ind_temp = indices[l-1];
          indices[l-1] = indices[l];
          indices[l] = ind_temp;
        }
      }
    }
  }

  // -- fill table
  for (UnsignedInteger i=0; i<pValues.getSize(); ++i)
  {
    distTableModel_->setNotEditableItem(i+2, 0, currentFittingTestResult_.getTestedDistributions()[indices[i]].getImplementation()->getClassName().c_str());
    QVariant aVariant = QVariant::fromValue(currentFittingTestResult_.getTestedDistributions()[indices[i]]);
    distTableModel_->setData(distTableModel_->index(i+2, 0), aVariant, Qt::UserRole);
    distTableModel_->setNotEditableItem(i+2, 1, pValues[i], 3);
    const bool isAccepted = currentFittingTestResult_.getKolmogorovTestResults()[indices[i]].getBinaryQualityMeasure();
    const QString text = isAccepted? tr("yes") : tr("no");
    const QColor color = isAccepted? Qt::green : QColor();
    distTableModel_->setNotEditableItem(i+2, 2, text, color);
  }
  // -- update parameters table
  distTableView_->selectRow(0+2);
  updateParametersTable(distTableModel_->index(0+2, 0));
  updateGraphs(distTableModel_->index(0+2, 0));
  distTableModel_->setData(distTableModel_->index(0+2, 0), true, Qt::CheckStateRole);

  // resize
  distTableView_->resizeColumnsToContents();
  int titleWidth = distTableView_->horizontalHeader()->sectionSize(1);
  // first: clear item at (0,1) because the text is too wide:
  // resizeColumnsToContents takes into account the text of item at (0,1)
  // to resize the column 1, even if there is a setSpan(0, 1, 1, 2)
  distTableModel_->setItem(0, 1, new QStandardItem);
  distTableView_->resizeToContents();
  distTableModel_->setNotEditableHeaderItem(0, 1, tr("Kolmogorov Smirnov"));
  distTableView_->setSpan(0, 1, 1, 2);
  const int subTitlesWidth = distTableView_->horizontalHeader()->sectionSize(1) + distTableView_->horizontalHeader()->sectionSize(2);
  const int widthCorrection = titleWidth - subTitlesWidth;
  if (widthCorrection > 0)
  {
    // fix the table width
    distTableView_->horizontalHeader()->resizeSection(2, distTableView_->horizontalHeader()->sectionSize(2) + widthCorrection);
    distTableView_->setMinimumWidth(distTableView_->minimumWidth() + widthCorrection);
  }
}


void InferenceResultWidget::updateRadioButtonsDistributionTable(QModelIndex current)
{
  // check
  if (!distTableModel_ || current.row() < 2)
    return;

  // set CheckStateRole of distTableModel_
  for (int i=2; i<distTableModel_->rowCount(); ++i)
  {
    if (distTableModel_->index(i, 0).row() == current.row())
      distTableModel_->setData(distTableModel_->index(i, 0), true, Qt::CheckStateRole);
    else
      distTableModel_->setData(distTableModel_->index(i, 0), false, Qt::CheckStateRole);
  }
}


void InferenceResultWidget::updateParametersTable(QModelIndex current)
{
  // check
  if (!(distTableModel_ && distParamTableModel_ && distParamTableView_))
    return;
  if (current.row() < 2)
    return;

  // reset
  distParamTableModel_->clear();
  distParamTableModel_->setColumnCount(2);

  QFont font;
  font.setBold(true);

  // headers
  distParamTableModel_->setNotEditableItem(0, 0, tr("Moments"));
  distParamTableView_->setSpan(0, 0, 1, 2);
  distParamTableModel_->setData(distParamTableModel_->index(0, 0), font, Qt::FontRole);

  distParamTableModel_->setNotEditableHeaderItem(1, 0, tr("Mean"));
  distParamTableModel_->setNotEditableHeaderItem(2, 0, tr("Standard deviation"));
  distParamTableModel_->setNotEditableHeaderItem(3, 0, tr("Skewness"));
  distParamTableModel_->setNotEditableHeaderItem(4, 0, tr("Kurtosis"));

  // fill
  if (current.isValid())
  {
    // -- get distribution
    const QVariant variant = distTableModel_->data(distTableModel_->index(current.row(), 0), Qt::UserRole);
    Distribution distribution = variant.value<Distribution>();

    // -- set titles
    if (distribution.getImplementation()->getClassName() != "Normal") // mean and std already displayed
    {
      UnsignedInteger nbParamDesc = distribution.getParameterDescription().getSize();
      if (distribution.getImplementation()->getClassName() == "Student")
        nbParamDesc = 1; // display only nu (mean and std already displayed)

      distParamTableModel_->setNotEditableItem(5, 0, tr("Native parameters"));
      distParamTableView_->setSpan(5, 0, 1, 2);
      distParamTableModel_->setData(distParamTableModel_->index(5, 0), font, Qt::FontRole);

      for (UnsignedInteger i=0; i<nbParamDesc; ++i)
        distParamTableModel_->setNotEditableHeaderItem(6+i, 0, distribution.getParameterDescription()[i].c_str());
    }

    // -- set parameters values
    distParamTableModel_->setNotEditableItem(1, 1, distribution.getMean()[0]);
    distParamTableModel_->setNotEditableItem(2, 1, distribution.getStandardDeviation()[0]);
    try
    {
      distParamTableModel_->setNotEditableItem(3, 1, distribution.getSkewness()[0]);
    }
    catch (std::exception & ex)
    {
      distParamTableModel_->setNotEditableItem(3, 1, tr("-"));
    }
    try
    {
      distParamTableModel_->setNotEditableItem(4, 1, distribution.getKurtosis()[0]);
    }
    catch (std::exception & ex)
    {
      distParamTableModel_->setNotEditableItem(4, 1, tr("-"));
    }

    if (distribution.getImplementation()->getClassName() != "Normal") // mean and std already displayed
    {
      UnsignedInteger nbParamVal = distribution.getParameterDescription().getSize();
      if (distribution.getImplementation()->getClassName() == "Student")
        nbParamVal = 1; // display only nu (mean and std already displayed)

      for (UnsignedInteger i=0; i<nbParamVal; ++i)
        distParamTableModel_->setNotEditableItem(6+i, 1, distribution.getParameter()[i]);
    }
  }
  else
  {
    for (UnsignedInteger i=0; i<4; ++i)
     distParamTableModel_->setNotEditableItem(1+i, 1, tr("-"));
  }
  // resize
  distParamTableView_->resizeToContents();
}


void InferenceResultWidget::updateGraphs(QModelIndex current)
{
  // check
  if (!(current.isValid() && distTableModel_ && currentFittingTestResult_.getValues().getSize()))
    return;
  if (!(pdfPlot_ && cdfPlot_ && qqPlot_))
    return;
  if (current.row() < 2)
    return;

  // reset
  pdfPlot_->clear();
  cdfPlot_->clear();
  qqPlot_->clear();

  // update
  // -- get distribution
  const QVariant variant = distTableModel_->data(distTableModel_->index(current.row(), 0), Qt::UserRole);
  Distribution distribution = variant.value<Distribution>();

  // -- pdf
  pdfPlot_->plotHistogram(currentFittingTestResult_.getValues());
  pdfPlot_->plotPDFCurve(distribution);
  pdfPlot_->setTitle(tr("PDF") + " " + distribution.getImplementation()->getName().c_str());

  // -- cdf
  cdfPlot_->plotHistogram(currentFittingTestResult_.getValues(), 1);
  cdfPlot_->plotCDFCurve(distribution);
  cdfPlot_->setTitle(tr("CDF") + " " + distribution.getImplementation()->getName().c_str());

  // -- qq plot
  Graph qqPlotGraph(VisualTest::DrawQQplot(currentFittingTestResult_.getValues(), distribution));
  qqPlot_->setTitle(tr("Q-Q Plot") + " " + distribution.getImplementation()->getName().c_str());
  qqPlot_->setAxisTitle(QwtPlot::yLeft, tr("Data quantiles"));
  qqPlot_->setAxisTitle(QwtPlot::xBottom, tr("%1 theoretical quantiles").arg(distribution.getImplementation()->getName().c_str()));
  qqPlot_->plotCurve(qqPlotGraph.getDrawable(1).getData(), QPen(Qt::blue, 5), QwtPlotCurve::Dots);
  qqPlot_->plotCurve(qqPlotGraph.getDrawable(0).getData());
}


Distribution InferenceResultWidget::getDistribution() const
{
  // check
  if (!(distTableModel_ && distTableView_))
    throw InternalException(HERE) << "Error in InferenceResultWidget::getDistribution";

  // get current distribution
  // loop begins at 2 because the two first rows are the table titles
  QModelIndex selectedDistributionIndex;
  for (int i=2; i<distTableModel_->rowCount(); ++i)
    if (distTableModel_->data(distTableModel_->index(i, 0), Qt::CheckStateRole).toBool())
      selectedDistributionIndex = distTableModel_->index(i, 0);

  Distribution distribution;
  if (selectedDistributionIndex.isValid())
  {
    const QVariant variant = distTableModel_->data(selectedDistributionIndex, Qt::UserRole);
    distribution = variant.value<Distribution>();
  }

  return distribution;
}


void InferenceResultWidget::showHideGraphConfigurationWidget()
{
  showHideGraphConfigurationWidget(tabWidget_->currentIndex());
}


void InferenceResultWidget::showHideGraphConfigurationWidget(int indexTab)
{
  switch (indexTab)
  {
    // if a plotWidget is visible
    case 0: // PDF-CDF graph
      if (pdf_cdfPlotGraphConfigWidget_)
        if (!pdf_cdfPlotGraphConfigWidget_->isVisible())
          emit graphWindowActivated(pdf_cdfPlotGraphConfigWidget_);
      break;
    case 1: // qq plot
      if (qqPlotGraphConfigWidget_)
        if (!qqPlotGraphConfigWidget_->isVisible())
          emit graphWindowActivated(qqPlotGraphConfigWidget_);
      break;
    // if no plotWidget is visible
    default:
    {
      emit graphWindowDeactivated();
      break;
    }
  }
}
}