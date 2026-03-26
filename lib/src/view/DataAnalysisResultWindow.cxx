//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of data analysis
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#include "persalys/DataAnalysisResultWindow.hxx"

#include "persalys/DataAnalysis.hxx"
#include "persalys/ErrorWidget.hxx"
#include "persalys/ParametersTableView.hxx"
#include "persalys/MomentsEstimatesTableGroupBox.hxx"
#include "persalys/MinMaxTableGroupBox.hxx"

#include <QGridLayout>

#ifdef PERSALYS_HAVE_PARAVIEW
#include "persalys/PVServerManagerSingleton.hxx"
#endif // PERSALYS_HAVE_PARAVIEW

using namespace OT;

namespace PERSALYS
{

DataAnalysisResultWindow::DataAnalysisResultWindow(AnalysisItem * item, QWidget * parent)
  : DataAnalysisWindow(item, parent)
{
  if (!dynamic_cast<DataAnalysis*>(item->getAnalysis().getImplementation().get()))
    throw InvalidArgumentException (HERE) << "Can NOT build the DataAnalysisResultWindow: The analysis of the item is not valid";

  initialize(item);

  buildInterface();
}


void DataAnalysisResultWindow::initialize(const AnalysisItem* item)
{
  DataAnalysis analysis(*dynamic_cast<DataAnalysis*>(item->getAnalysis().getImplementation().get()));

  isConfidenceIntervalRequired_ = analysis.isConfidenceIntervalRequired();
  levelConfidenceInterval_ = analysis.getLevelConfidenceInterval();

  result_ = analysis.getResult();
  designOfExperiment_ = result_.getDesignOfExperiment();

  titleLabel_->setText(tr("Data analysis"));
  titleLabel_->setDocLink("user_manual/graphical_interface/data_analysis/user_manual_data_analysis.html#dataanalysisresult");
}

void DataAnalysisResultWindow::addSummaryTab()
{
  auto * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  auto * tab = new QWidget;
  auto * tabLayout = new QGridLayout(tab);

  // -- results --
  int row = 0;

  auto * parametersGroupBox = new QGroupBox();
  auto * parametersGroupBoxLayout = new QVBoxLayout(parametersGroupBox);

  if (!analysisErrorMessage_.isEmpty())
  {
    auto * analysisErrorMessageLabel = new ErrorWidget;
    analysisErrorMessageLabel->setFramelessErrorMessage(analysisErrorMessage_);
    parametersGroupBoxLayout->addWidget(analysisErrorMessageLabel);
  }

  // parameters values
  QStringList namesList;
  QStringList valuesList;

  const OT::UnsignedInteger totalSampleSize = designOfExperiment_.getSample().getSize();
  const Point effetiveSize = result_.getEffectiveSize();

  summaryValuesListSampleSizeIndex_ = valuesList.size();
  const UnsignedInteger multivariateSampleSize = result_.getMultivariateDoE().getSample().getSize();

  if (effetiveSize.getDimension())
  {
    namesList << tr("Sample size");
    valuesList << QString::number(totalSampleSize);

    namesList << tr("Valid marginal size");
    valuesList << QString::number(effetiveSize[variablesListWidget_->item(0)->data(Qt::UserRole).toInt()]);

    namesList << tr("Valid multivariate size");
    valuesList << QString::number(multivariateSampleSize);
    doMultivariate_ = multivariateSampleSize > 0;
  }
  else // when displaying analysis in studies saved with a version of Persalys < 19.0
  {
    namesList << sampleSizeTitle_;
    valuesList << QString::number(totalSampleSize);
  }

  auto * table = new ParametersTableView(namesList, valuesList, true, true);
  auto updateEffectiveSize = [this, table, totalSampleSize] (int index) {
    int realIndex = variablesListWidget_->item(index)->data(Qt::UserRole).toInt();
    const auto effectiveSize = static_cast<UnsignedInteger>(result_.getEffectiveSize()[realIndex]);
    const QColor color = effectiveSize != totalSampleSize ? QColor("orange") : QColor();
    table->setValueAt(summaryValuesListSampleSizeIndex_, QString::number(effectiveSize), color);
  };
  if (effetiveSize.getDimension())
  {
    updateEffectiveSize(0);
    connect (variablesListWidget_, &VariablesListWidget::currentRowChanged, updateEffectiveSize);
  }

  if (multivariateSampleSize != totalSampleSize)
  {
    table->setValueAt(valuesList.size()-1, QString::number(multivariateSampleSize), QColor("orange"));
  }

  parametersGroupBoxLayout->addWidget(table);
  tabLayout->addWidget(parametersGroupBox, row, 0);

    // moments estimation
  if (result_.getMean().getSize())
  {
    // we want to display output results before the input results
    // input indices
    Indices inInd(inputNames_.size());
    inInd.fill();
    // output indices
    Indices ind(outputNames_.size());
    ind.fill(inputNames_.size());
    // indices with good order
    ind.add(inInd);

    auto * estimatesGroupBox = new MomentsEstimatesTableGroupBox(result_,
        isConfidenceIntervalRequired_,
        levelConfidenceInterval_,
        ind);

    tabLayout->addWidget(estimatesGroupBox, ++row, 0);
    connect(variablesListWidget_, &VariablesListWidget::currentRowChanged, estimatesGroupBox, &MomentsEstimatesTableGroupBox::setCurrentIndexStackedWidget);

  // min/max table
  if (doMultivariate_)
  {
    auto * minMaxTableGroupBox = new MinMaxTableGroupBox(result_.getMultivariateDoE(), false);
    tabLayout->addWidget(minMaxTableGroupBox, ++row, 0);
    connect(variablesListWidget_, &VariablesListWidget::currentRowChanged, minMaxTableGroupBox, &MinMaxTableGroupBox::setCurrentIndexStackedWidget);
  }

  tabLayout->setRowStretch(++row, 1);
  scrollArea->setWidget(tab);
  tabWidget_->addTab(scrollArea, tr("Summary"));
  }
}

void DataAnalysisResultWindow::addDependenceTab()
{
  if (!doMultivariate_)
    return;
  
  designOfExperiment_ = result_.getMultivariateDoE();
  DataAnalysisWindow::addDependenceTab();
  designOfExperiment_ = result_.getDesignOfExperiment();
}

void DataAnalysisResultWindow::addScatterPlotsTab()
{
  if (!doMultivariate_)
    return;
  
  designOfExperiment_ = result_.getMultivariateDoE();
  DataAnalysisWindow::addScatterPlotsTab();
  designOfExperiment_ = result_.getDesignOfExperiment();
}

void DataAnalysisResultWindow::addPlotMatrixTab()
{
  designOfExperiment_ = result_.getMultivariateDoE();
  DataAnalysisWindow::addPlotMatrixTab();
  designOfExperiment_ = result_.getDesignOfExperiment();
}

#ifdef PERSALYS_HAVE_PARAVIEW

void DataAnalysisResultWindow::addParaviewWidgetsTabs()
{
  if (!doMultivariate_)
    return;
  
  const Sample sample = result_.getMultivariateDoE().getSample();

  // table tab
  auto * pvSpreadSheetWidget = new PVSpreadSheetViewWidget(this, PVServerManagerSingleton::Get());
  tabWidget_->addTab(PVSpreadSheetViewWidget::GetSpreadSheetViewWidget(pvSpreadSheetWidget, sample, getItem()), tr("Table"));

  // if only one variable or if only one point : do not need the following graphs
  if (sample.getDimension() > 1 && sample.getSize() > 1)
    addParaviewPlotWidgetsTabs(pvSpreadSheetWidget, sample);
}

#endif // PERSALYS_HAVE_PARAVIEW

} // namespace PERSALYS
