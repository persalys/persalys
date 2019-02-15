//                                               -*- C++ -*-
/**
 *  @brief QTabWidget for the FORM results
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/ApproximationResultTabWidget.hxx"

#include "otgui/FORMAnalysis.hxx"
#include "otgui/SORMAnalysis.hxx"
#include "otgui/FORMImportanceSamplingAnalysis.hxx"
#include "otgui/ParametersTableView.hxx"
#include "otgui/ParametersWidget.hxx"
#include "otgui/PieChartView.hxx"
#include "otgui/TranslationManager.hxx"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QHeaderView>
#include <QAbstractItemModel>
#include <QLabel>

using namespace OT;

namespace OTGUI
{

ApproximationResultTabWidget::ApproximationResultTabWidget(const FORMResult& result,
    const ReliabilityAnalysis& analysis,
    QWidget* parent)
  : QTabWidget(parent)
  , method_(FORM)
  , formResult_(result)
  , sormResult_()
  , result_(result)
  , parametersWidget_(0)
  , maximumEvaluationNumber_(0)
{
  // analysis parameters
  Parameters analysisParameters;

  const FORMAnalysis * formAnalysis(dynamic_cast<const FORMAnalysis*>(&analysis));
  if (formAnalysis)
  {
    // Maximum iteration number : to add a warning if needed
    maximumEvaluationNumber_ = formAnalysis->getOptimizationAlgorithm().getMaximumEvaluationNumber();

    // analysis parameters
    analysisParameters = analysis.getParameters();
  }
  const FORMImportanceSamplingAnalysis * formISAnalysis(dynamic_cast<const FORMImportanceSamplingAnalysis*>(&analysis));
  if (formISAnalysis)
  {
    // Maximum iteration number : to add a warning if needed
    maximumEvaluationNumber_ = formISAnalysis->getOptimizationAlgorithm().getMaximumEvaluationNumber();

    // analysis parameters
    FORMAnalysis aFakeAnalysis("Unnamed", formISAnalysis->getLimitState());
    aFakeAnalysis.setOptimizationAlgorithm(formISAnalysis->getOptimizationAlgorithm());
    aFakeAnalysis.setPhysicalStartingPoint(formISAnalysis->getPhysicalStartingPoint());
    analysisParameters = aFakeAnalysis.getParameters();
  }

  // set parameters widget
  if (analysisParameters.getSize())
  {
    parametersWidget_ = new QScrollArea;
    QWidget * paramWidget = new QWidget;
    QVBoxLayout * parametersWidgetLayout = new QVBoxLayout(paramWidget);
    parametersWidgetLayout->addWidget(new ParametersWidget(tr("Threshold exceedance parameters"), analysisParameters), 0, Qt::AlignTop);
    parametersWidget_->setWidget(paramWidget);
  }

  buildInterface();
}


ApproximationResultTabWidget::ApproximationResultTabWidget(const SORMResult& result,
    const ReliabilityAnalysis& analysis,
    QWidget* parent)
  : QTabWidget(parent)
  , method_(SORM)
  , formResult_()
  , sormResult_(result)
  , result_(result)
  , parametersWidget_(0)
  , maximumEvaluationNumber_(0)
{
  // analysis parameters
  Parameters analysisParameters;

  const SORMAnalysis * sormAnalysis(dynamic_cast<const SORMAnalysis*>(&analysis));
  if (sormAnalysis)
  {
    // Maximum iteration number : to add a warning if needed
    maximumEvaluationNumber_ = sormAnalysis->getOptimizationAlgorithm().getMaximumEvaluationNumber();

    // analysis parameters
    analysisParameters = analysis.getParameters();
  }
  // set parameters widget
  if (analysisParameters.getSize())
  {
    parametersWidget_ = new QScrollArea;
    QWidget * paramWidget = new QWidget;
    QVBoxLayout * parametersWidgetLayout = new QVBoxLayout(paramWidget);
    parametersWidgetLayout->addWidget(new ParametersWidget(tr("Threshold exceedance parameters"), analysisParameters), 0, Qt::AlignTop);
    parametersWidget_->setWidget(paramWidget);
  }

  buildInterface();
}


void ApproximationResultTabWidget::buildInterface()
{
  // first tab summary --------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  QStringList namesList;
  QStringList valuesList;

  // FORM
  if (method_ == FORM)
  {
    // failure probability table
    namesList << tr("Failure probability")
              << tr("Hasofer reliability index");

    valuesList << QString::number(formResult_.getEventProbability())
               << QString::number(result_.getHasoferReliabilityIndex());

    ParametersWidget * parametersTable = new ParametersWidget(tr("Failure probability"), namesList, valuesList, true, true);
    tabLayout->addWidget(parametersTable, 0, Qt::AlignTop);
  }
  // SORM
  else
  {
    QGroupBox * groupBox = new QGroupBox(tr("Failure probability"));
    QVBoxLayout * groupBoxLayout = new QVBoxLayout(groupBox);

    // evaluate the event probability FORM
    // in the standard space all marginals of the standard distribution are identical
    Scalar eventProba = sormResult_.getLimitStateVariable().getImplementation()->getAntecedent().getDistribution().getStandardDistribution().getMarginal(0).computeCDF(Point(1, -sormResult_.getHasoferReliabilityIndex()));
    if (sormResult_.getIsStandardPointOriginInFailureSpace())
    {
      // isStandardPointOriginInFailureSpace is true: unusual case
      eventProba = 1.0 - eventProba;
    }

    // failure probability table

    // -- Hasofer (FORM)
    namesList << tr("Failure probability (FORM)")
              << tr("Reliability index");
    valuesList << QString::number(eventProba)
               << QString::number(result_.getHasoferReliabilityIndex());

    ParametersWidget * parametersTable = new ParametersWidget(tr("Hasofer's formula"), namesList, valuesList, true, true);
    groupBoxLayout->addWidget(parametersTable);

    QString errorMessage;

    namesList[0] = tr("Failure probability");

    // -- Breitung
    try
    {
      valuesList[0] = QString::number(sormResult_.getEventProbabilityBreitung());
      valuesList[1] = QString::number(sormResult_.getGeneralisedReliabilityIndexBreitung());
    }
    catch (std::exception &ex)
    {
      valuesList[0] = "-";
      valuesList[1] = "-";
      errorMessage = ex.what();
    }

    parametersTable = new ParametersWidget(tr("Breitung's formula"), namesList, valuesList, true, true);
    groupBoxLayout->addWidget(parametersTable);
    if (!errorMessage.isEmpty())
    {
      groupBoxLayout->addWidget(new QLabel(QString("<font color=red>%1</font>").arg(errorMessage)));
      errorMessage.clear();
    }

    // -- HohenBichler
    try
    {
      valuesList[0] = QString::number(sormResult_.getEventProbabilityHohenBichler());
      valuesList[1] = QString::number(sormResult_.getGeneralisedReliabilityIndexHohenBichler());
    }
    catch (std::exception &ex)
    {
      valuesList[0] = "-";
      valuesList[1] = "-";
      errorMessage = ex.what();
    }

    parametersTable = new ParametersWidget(tr("Hohen Bichler's formula"), namesList, valuesList, true, true);
    groupBoxLayout->addWidget(parametersTable);
    if (!errorMessage.isEmpty())
    {
      groupBoxLayout->addWidget(new QLabel(QString("<font color=red>%1</font>").arg(errorMessage)));
      errorMessage.clear();
    }

    // -- Tvedt
    try
    {
      valuesList[0] = QString::number(sormResult_.getEventProbabilityTvedt());
      valuesList[1] = QString::number(sormResult_.getGeneralisedReliabilityIndexTvedt());
    }
    catch (std::exception &ex)
    {
      valuesList[0] = "-";
      valuesList[1] = "-";
      errorMessage = ex.what();
    }

    parametersTable = new ParametersWidget(tr("Tvedt's formula"), namesList, valuesList, true, true);
    groupBoxLayout->addWidget(parametersTable);
    if (!errorMessage.isEmpty())
    {
      groupBoxLayout->addWidget(new QLabel(QString("<font color=red>%1</font>").arg(errorMessage)));
      errorMessage.clear();
    }

    groupBoxLayout->addStretch();
    tabLayout->addWidget(groupBox, 0, Qt::AlignTop);
  }

  // optimization result table
  QGroupBox * groupBox = new QGroupBox(tr("Optimization result"));
  QVBoxLayout * groupBoxLayout = new QVBoxLayout(groupBox);
  namesList.clear();
  namesList << tr("Evaluation number")
            << tr("Absolute error")
            << tr("Relative error")
            << tr("Residual error")
            << tr("Constraint error");

  valuesList.clear();
  valuesList << QString::number(result_.getOptimizationResult().getEvaluationNumber())
             << QString::number(result_.getOptimizationResult().getAbsoluteError())
             << QString::number(result_.getOptimizationResult().getRelativeError())
             << QString::number(result_.getOptimizationResult().getResidualError())
             << QString::number(result_.getOptimizationResult().getConstraintError());

  ParametersTableView * table = new ParametersTableView(namesList, valuesList, true, true);

  // add warning if Maximum iteration number reached
  if (result_.getOptimizationResult().getEvaluationNumber() == maximumEvaluationNumber_)
  {
    table->model()->setData(table->model()->index(0, 1), QIcon(":/images/task-attention.png"), Qt::DecorationRole);
    table->model()->setData(table->model()->index(0, 1), tr("Maximum iterations number reached"), Qt::ToolTipRole);
  }
  groupBoxLayout->addWidget(table);

  tabLayout->addWidget(groupBox, 0, Qt::AlignTop);
  tabLayout->addStretch();

  scrollArea->setWidget(tab);

  addTab(scrollArea, tr("Summary"));

  // second tab design point --------------------
  tab = new QWidget;
  tabLayout = new QVBoxLayout(tab);

  scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  groupBox = new QGroupBox(tr("Design point"));
  groupBoxLayout = new QVBoxLayout(groupBox);

  CopyableTableView * resultsTable = new CopyableTableView;
  resultsTable->horizontalHeader()->hide();
  resultsTable->verticalHeader()->hide();

  const Description inDescription(result_.getLimitStateVariable().getFunction().getInputDescription());
  const UnsignedInteger inDimension = inDescription.getSize();
  CustomStandardItemModel * resultsTableModel = new CustomStandardItemModel(inDimension + 2, 4, resultsTable);
  resultsTable->setModel(resultsTableModel);

  // horizontal header
  resultsTableModel->setNotEditableHeaderItem(0, 0, tr("Variable"));
  resultsTable->setSpan(0, 0, 2, 1);
  resultsTableModel->setNotEditableHeaderItem(0, 1, tr("Importance factors"));
  resultsTable->setSpan(0, 1, 2, 1);
  resultsTableModel->setNotEditableHeaderItem(0, 2, tr("Coordinates"));
  resultsTableModel->setNotEditableHeaderItem(1, 2, tr("Standard space"));
  resultsTableModel->setNotEditableHeaderItem(1, 3, tr("Physical space"));

  // importance factors
  PointWithDescription importanceFactors(result_.getImportanceFactors());
  importanceFactors.setDescription(inDescription);

  // set values
  for (UnsignedInteger i = 0; i < inDimension; ++i)
  {
    // variable name
    resultsTableModel->setNotEditableItem(i + 2, 0, QString::fromUtf8(inDescription[i].c_str()));

    // importance factor
    resultsTableModel->setNotEditableItem(i + 2, 1, importanceFactors[i]);

    // design point standard space
    resultsTableModel->setNotEditableItem(i + 2, 2, result_.getStandardSpaceDesignPoint()[i]);

    // design point physical space
    resultsTableModel->setNotEditableItem(i + 2, 3, result_.getPhysicalSpaceDesignPoint()[i]);
  }

  // resize to contents
  resultsTable->resizeToContents();
  resultsTable->setSpan(0, 2, 1, 2);

  groupBoxLayout->addWidget(resultsTable);
  tabLayout->addWidget(groupBox, 0, Qt::AlignTop);

  // importance factors
  groupBox = new QGroupBox(tr("Importance factors pie chart"));
  groupBoxLayout = new QVBoxLayout(groupBox);
  PieChartView * pieChart = new PieChartView(importanceFactors);
  pieChart->setPlotName(tr("importanceFactors"));
  groupBoxLayout->addWidget(pieChart, 0, Qt::AlignCenter);
  tabLayout->addWidget(groupBox, 0, Qt::AlignTop);
  tabLayout->addStretch();

  scrollArea->setWidget(tab);

  addTab(scrollArea, tr("Design point"));

  // sensitivity --------------------
  tab = new QWidget;
  tabLayout = new QVBoxLayout(tab);

  try
  {
    // compute sensitivities
    AnalyticalResult::Sensitivity eventProbaSensitivity;
    if (method_ == FORM)
      eventProbaSensitivity = formResult_.getEventProbabilitySensitivity();
    AnalyticalResult::Sensitivity hasoferIndexSensitivity(result_.getHasoferReliabilityIndexSensitivity());

    // if the computation of sensitivities has succeeded
    scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    groupBox = new QGroupBox(tr("Sensitivities"));
    groupBoxLayout = new QVBoxLayout(groupBox);

    resultsTable = new CopyableTableView;
    resultsTable->horizontalHeader()->hide();
    resultsTable->verticalHeader()->hide();

    resultsTableModel = new CustomStandardItemModel(inDimension + 2, 3, resultsTable);
    resultsTable->setModel(resultsTableModel);

    // horizontal header
    resultsTableModel->setNotEditableHeaderItem(0, 0, tr("Variable"));
    resultsTableModel->setNotEditableHeaderItem(0, 1, tr("Distribution parameters"));
    resultsTableModel->setNotEditableHeaderItem(0, 2, tr("Reliability index"));
    if (method_ == FORM)
      resultsTableModel->setNotEditableHeaderItem(0, 3, tr("Failure probability"));

    // set values
    int row = 1;
    for (UnsignedInteger i = 0; i < inDimension; ++i)
    {
      // variable name
      const int varRow = row;
      resultsTableModel->setNotEditableItem(row, 0, inDescription[i].c_str());

      PointWithDescription pfSensitivity;
      if (method_ == FORM)
        pfSensitivity = eventProbaSensitivity[i];
      const PointWithDescription betaSensitivity(hasoferIndexSensitivity[i]);

      for (UnsignedInteger j = 0; j < betaSensitivity.getDimension(); ++j)
      {
        int col = 0;
        // distribution parameter name
        String parameterName = betaSensitivity.getDescription()[j];
        parameterName = parameterName.substr(0, betaSensitivity.getDescription()[j].find("_marginal"));
        QString paramName = TranslationManager::GetTranslatedDistributionParameterName(parameterName);
        resultsTableModel->setNotEditableItem(row, ++col, paramName);

        // sensitivity value beta
        resultsTableModel->setNotEditableItem(row, ++col, betaSensitivity[j]);

        // sensitivity value pf
        if (pfSensitivity.getSize())
          resultsTableModel->setNotEditableItem(row, ++col, pfSensitivity[j]);

        ++row;
      }
      resultsTable->setSpan(varRow, 0, betaSensitivity.getDimension(), 1);
    }

    // resize to contents
    resultsTable->resizeToContents();

    groupBoxLayout->addWidget(resultsTable);
    tabLayout->addWidget(groupBox, 0, Qt::AlignTop);

    scrollArea->setWidget(tab);

    addTab(scrollArea, tr("Sensitivities"));
  }
  catch (std::exception& ex)
  {
    QLabel * errorLabel = new QLabel;
    errorLabel->setStyleSheet("QLabel { color : red; }");
    errorLabel->setText(tr("Internal error during the computation of the sensitivities.\n\n%1").arg(ex.what()));
    errorLabel->setWordWrap(true);

    tabLayout->addWidget(errorLabel);
    tabLayout->addStretch();
    addTab(tab, tr("Sensitivities"));
  }

  // parameters widget --------------------
  if (parametersWidget_)
    addTab(parametersWidget_, tr("Parameters"));
}
}
