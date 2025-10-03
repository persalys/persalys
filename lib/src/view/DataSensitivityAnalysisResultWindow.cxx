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
#include "persalys/ParametersTableView.hxx"

#include <QVBoxLayout>
#include <QSplitter>
#include <QScrollArea>
#include <QComboBox>
#include <QTextEdit>

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

void DataSensitivityAnalysisResultWindow::initialize(const AnalysisItem* item)
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

  auto * widgetLayout = new QVBoxLayout(this);

  widgetLayout->addWidget(new TitleLabel(tr("Sensitivity analysis results"), "user_manual/graphical_interface/data_analysis/user_manual_data_analysis.html#datasensitivityanalysisresult"));

  auto * mainWidget = new QSplitter(Qt::Horizontal);

  auto outputsGroupBox = new QGroupBox(tr("Variables"));
  auto * groupBoxLayout = new QVBoxLayout(outputsGroupBox);

  outputsListWidget_ = new VariablesListWidget;
  outputsListWidget_->addItems(outputNames_);
  groupBoxLayout->addWidget(outputsListWidget_);

  mainWidget->addWidget(outputsGroupBox);
  mainWidget->setStretchFactor(0, 1);

  auto tabWidget = new QTabWidget;
  addSobolTab(tabWidget);
  addSRCTab(tabWidget);

  auto * widget = new QWidget;
  auto * vbox = new QVBoxLayout(widget);

  vbox->addWidget(tabWidget);

  mainWidget->addWidget(widget);
  mainWidget->setStretchFactor(1, 10);

  widgetLayout->addWidget(mainWidget, 1);

  // Display warning message if not empty
  if (!result_.isIndependent())
  {
    auto message = QString::fromStdString(result_.getIndependenceWarningMessage());
    auto warningTextEdit = new QTextEdit();
    warningTextEdit->setPlainText(message);
    warningTextEdit->setReadOnly(true);
    warningTextEdit->setStyleSheet("QTextEdit { color : orange; font-weight: bold; background-color: transparent; border: 1px solid orange; }");
    warningTextEdit->setFixedHeight(80);
    warningTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    warningTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    widgetLayout->addWidget(warningTextEdit);
  }
}

void DataSensitivityAnalysisResultWindow::addSobolTab(QTabWidget * tabWidget)
{
  auto * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  auto * stackedWidget = new ResizableStackedWidget;
  connect(outputsListWidget_, &VariablesListWidget::currentRowChanged, stackedWidget, &ResizableStackedWidget::setCurrentIndex);

  const auto firstOrderIndices = result_.getFirstOrderSobolIndices();
  const auto firstOrderIndicesInterval = result_.getFirstOrderSobolIndicesInterval();
  const UnsignedInteger nbOutputs = firstOrderIndices.getSize();
  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    auto * indicesResultWidget = new SensitivityResultWidget(
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

  scrollArea->setWidget(stackedWidget);
  tabWidget->addTab(scrollArea, tr("Rank Sobol'"));
}

/*adapted from SRCResultWindow::buildInterface*/
void DataSensitivityAnalysisResultWindow::addSRCTab(QTabWidget * tabWidget)
{
  const UnsignedInteger nbOutputs = designOfExperiment_.getOutputSample().getDimension();

  auto * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  auto * widget = new QWidget;
  auto * vbox = new QVBoxLayout(widget);

  // - indices graph and table
  auto * stackedWidget = new ResizableStackedWidget;
  connect(outputsListWidget_, &VariablesListWidget::currentRowChanged, stackedWidget, &ResizableStackedWidget::setCurrentIndex);

  const Sample signedSRCIndices{result_.getSignedSRCIndices()};
  const Sample SRCIndices{result_.getSRCIndices()};
  const auto signedSRCIndicesInterval = result_.getSignedSRCIndicesInterval();
  const auto SRCIndicesInterval = result_.getSRCIndicesInterval();
  const Point r2 = result_.getR2();

  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    auto * indicesWidget = new QWidget;
    auto * indicesLayout = new QVBoxLayout(indicesWidget);

    // table
    if (r2.getSize() == nbOutputs)
    {
      QStringList namesList;
      namesList << tr("R2");

      QStringList valuesList;
      valuesList << QString::number(r2[i]);
      auto * basisTableView = new ParametersTableView(namesList, valuesList, true, true);
      indicesLayout->addWidget(basisTableView);
    }

    // indices graph and table
    Interval indicesInterval = SRCIndicesInterval.getSize() == nbOutputs ? SRCIndicesInterval[i] : Interval();
    Interval signedIndicesInterval = signedSRCIndicesInterval.getSize() == nbOutputs ? signedSRCIndicesInterval[i] : Interval();
    auto * indicesResultWidget = new SensitivityResultWidget(SRCIndices[i],
        indicesInterval,
        signedSRCIndices.getSize() == nbOutputs ? signedSRCIndices[i] : Point(),
        signedIndicesInterval,
        QtOT::StringListToDescription(inputNames_),
        outputNames_[i].toStdString(),
        SensitivityResultWidget::SRC,
        this);
    indicesLayout->addWidget(indicesResultWidget);
    stackedWidget->addWidget(indicesWidget);
  }
  vbox->addWidget(stackedWidget);

  vbox->setContentsMargins(0, 0, 0, 0);
  scrollArea->setWidget(widget);
  tabWidget->addTab(scrollArea, tr("SRC"));
}

} // namespace PERSALYS