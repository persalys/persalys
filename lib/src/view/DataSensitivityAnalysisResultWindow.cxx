//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the sensitivity analysis of a data model
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
#include "persalys/DataSensitivityAnalysisResultWindow.hxx"
#include "persalys/DataSensitivityAnalysis.hxx"
#include "persalys/ResizableStackedWidget.hxx"
#include "persalys/SensitivityResultWidget.hxx"

#include <QVBoxLayout>
#include <QSplitter>
#include <QScrollArea>
#include <QComboBox>

using namespace OT;

namespace PERSALYS
{

DataSensitivityAnalysisResultWindow::DataSensitivityAnalysisResultWindow(AnalysisItem * item, QWidget *parent)
  : ResultWindow(item, parent)
{
  if(!dynamic_cast<DataSensitivityAnalysis*>(item->getAnalysis().getImplementation().get()))
  {
    throw InvalidArgumentException (HERE) << "Can NOT build the DataSensitivityAnalysisResultWindow: The analysis of the item is not valid";
  }
  initialize(item);
  buildInterface();
}

void DataSensitivityAnalysisResultWindow::initialize(AnalysisItem* item)
{
  DataSensitivityAnalysis analysis = *dynamic_cast<DataSensitivityAnalysis*>(item->getAnalysis().getImplementation().get());
  result_ = analysis.getResult();
  designOfExperiment_ = result_.getDesignOfExperiment();
}

void DataSensitivityAnalysisResultWindow::initializeVariablesNames()
{
  PhysicalModel model(designOfExperiment_.getPhysicalModel());

  // inputs
  if (designOfExperiment_.getInputSample().getSize())
  {
    inputNames_ = QtOT::DescriptionToStringList(designOfExperiment_.getInputSample().getDescription());
    inAxisTitles_ = QtOT::GetVariableAxisLabels(model, designOfExperiment_.getInputSample().getDescription());
  }
  // outputs
  if (designOfExperiment_.getOutputSample().getSize())
  {
    outputNames_ = QtOT::DescriptionToStringList(designOfExperiment_.getOutputSample().getDescription());
    outAxisTitles_ = QtOT::GetVariableAxisLabels(model, designOfExperiment_.getOutputSample().getDescription());
  }
}

void DataSensitivityAnalysisResultWindow::buildInterface()
{
  // get output info
  initializeVariablesNames();

  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  widgetLayout->addWidget(new TitleLabel(tr("Sensitivity analysis results"), "user_manual/graphical_interface/data_analysis/user_manual_data_analysis.html#datasensitivityanalysisresult"));

  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  QWidget * widget = new QWidget;
  QVBoxLayout * vbox = new QVBoxLayout(widget);

  QComboBox * outputsListWidget = new QComboBox;
  outputsListWidget->addItems(outputNames_);
  vbox->addWidget(outputsListWidget);

  // Display warning message if not empty
  if (!result_.isIndependent())
  {
    auto message = QString::fromStdString(result_.getIndependenceWarningMessage());
    auto warningLabel = new QLabel(message);
    warningLabel->setStyleSheet("QLabel { color : orange; font-weight: bold; }");
    warningLabel->setWordWrap(true);
    vbox->addWidget(warningLabel);
  }

  ResizableStackedWidget * stackedWidget = new ResizableStackedWidget;
  connect(outputsListWidget, SIGNAL(currentIndexChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));

  const auto firstOrderIndices = result_.getFirstOrderIndices();
  const auto firstOrderIndicesInterval = result_.getFirstOrderIndicesInterval();
  const UnsignedInteger nbOutputs = firstOrderIndices.getSize();
  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    SensitivityResultWidget * indicesResultWidget = new SensitivityResultWidget(
        firstOrderIndices[i],
        firstOrderIndicesInterval[i],
        Point(),
        Interval(),
        QtOT::StringListToDescription(inputNames_),
        outputNames_[i].toStdString(),
        SensitivityResultWidget::Sobol,
        this);
    stackedWidget->addWidget(indicesResultWidget);
  }
  vbox->addWidget(stackedWidget, 1);

  scrollArea->setWidget(widget);

  mainWidget->addWidget(scrollArea);
  mainWidget->setStretchFactor(0, 1);

  widgetLayout->addWidget(mainWidget, 1);
}

} // namespace PERSALYS