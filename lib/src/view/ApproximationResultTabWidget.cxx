//                                               -*- C++ -*-
/**
 *  @brief QTabWidget for the FORM results
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#include "otgui/ApproximationResultTabWidget.hxx"

#include "otgui/FORMAnalysis.hxx"
#include "otgui/FORMImportanceSamplingAnalysis.hxx"
#include "otgui/AnalysisItem.hxx"
#include "otgui/ParametersTableView.hxx"
#include "otgui/ParametersWidget.hxx"
#include "otgui/PieChartView.hxx"

#include <QScrollArea>
#include <QHeaderView>
#include <QAbstractItemModel>
#include <QLabel>

using namespace OT;

namespace OTGUI {

ApproximationResultTabWidget::ApproximationResultTabWidget(const FORMResult& result,
                                                           const ReliabilityAnalysis& analysis,
                                                           QWidget* parent)
  : QTabWidget(parent)
  , result_(result)
  , parametersWidget_(0)
  , maximumIterationNumber_(0)
{
  // analysis parameters
  QStringList paramNames;
  QStringList paramValues;

  const FORMAnalysis * formAnalysis(dynamic_cast<const FORMAnalysis*>(&analysis));
  if (formAnalysis)
  {
    // Maximum iteration number : to add a warning if needed
    maximumIterationNumber_ = formAnalysis->getOptimizationAlgorithm().getMaximumIterationNumber();
    // analysis parameters
    AnalysisItem::GetAnalysisParameters(analysis, paramNames, paramValues);
  }
  const FORMImportanceSamplingAnalysis * formISAnalysis(dynamic_cast<const FORMImportanceSamplingAnalysis*>(&analysis));
  if (formISAnalysis)
  {
    // Maximum iteration number : to add a warning if needed
    maximumIterationNumber_ = formISAnalysis->getOptimizationAlgorithm().getMaximumIterationNumber();
    // analysis parameters
    FORMAnalysis aFakeAnalysis("Unnamed", formISAnalysis->getLimitState());
    aFakeAnalysis.setOptimizationAlgorithm(formISAnalysis->getOptimizationAlgorithm());
    aFakeAnalysis.setPhysicalStartingPoint(formISAnalysis->getPhysicalStartingPoint());
    AnalysisItem::GetAnalysisParameters(aFakeAnalysis, paramNames, paramValues);
  }

  // set parameters widget
  if (!paramNames.isEmpty() && paramNames.size() == paramValues.size())
  {
    parametersWidget_ = new QWidget;
    QGridLayout * parametersWidgetLayout = new QGridLayout(parametersWidget_);
    parametersWidgetLayout->addWidget(new ParametersWidget(tr("Threshold exceedance parameters"), paramNames, paramValues));
  }

  buildInterface();
}


void ApproximationResultTabWidget::buildInterface()
{
  setWindowTitle(tr("Threshold exceedance results"));

  // first tab summary --------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  tabLayout->setSizeConstraint(QLayout::SetFixedSize);

  // failure probability table
  QStringList namesList;
  namesList << tr("Failure probability")
            << tr("Hasofer reliability index");

  QStringList valuesList;
  valuesList << QString::number(result_.getEventProbability())
             << QString::number(result_.getHasoferReliabilityIndex());

  ParametersWidget * parametersTable = new ParametersWidget(tr("Failure probability"), namesList, valuesList, true, true);
  tabLayout->addWidget(parametersTable);

  // optimization result table
  QGroupBox * groupBox = new QGroupBox(tr("Optimization result"));
  QVBoxLayout * groupBoxLayout = new QVBoxLayout(groupBox);
  namesList.clear();
  namesList << tr("Iterations number")
            << tr("Calls number")
            << tr("Absolute error")
            << tr("Relative error")
            << tr("Residual error")
            << tr("Constraint error");

  valuesList.clear();
  valuesList << QString::number(result_.getOptimizationResult().getIterationNumber())
             << QString::number(result_.getLimitStateVariable().getFunction().getCallsNumber())
             << QString::number(result_.getOptimizationResult().getAbsoluteError())
             << QString::number(result_.getOptimizationResult().getRelativeError())
             << QString::number(result_.getOptimizationResult().getResidualError())
             << QString::number(result_.getOptimizationResult().getConstraintError());

  ParametersTableView * table = new ParametersTableView(namesList, valuesList, true, true);

  // add warning if Maximum iteration number reached
  if (result_.getOptimizationResult().getIterationNumber() == maximumIterationNumber_)
  {
    table->model()->setData(table->model()->index(0, 1), QIcon(":/images/task-attention.png"), Qt::DecorationRole);
    table->model()->setData(table->model()->index(0, 1), tr("Maximum iterations number reached"), Qt::ToolTipRole);
  }
  groupBoxLayout->addWidget(table);

  tabLayout->addWidget(groupBox);

  scrollArea->setWidget(tab);

  addTab(scrollArea, tr("Summary"));

  // second tab design point --------------------
  tab = new QWidget;
  tabLayout = new QVBoxLayout(tab);

  scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  tabLayout->setSizeConstraint(QLayout::SetFixedSize);

  groupBox = new QGroupBox(tr("Design point"));
  groupBoxLayout = new QVBoxLayout(groupBox);

  ResizableTableViewWithoutScrollBar * resultsTable = new ResizableTableViewWithoutScrollBar;
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
    resultsTableModel->setNotEditableItem(i+2, 0, QString::fromUtf8(inDescription[i].c_str()));

    // importance factor
    resultsTableModel->setNotEditableItem(i+2, 1, importanceFactors[i]);

    // design point standard space
    resultsTableModel->setNotEditableItem(i+2, 2, result_.getStandardSpaceDesignPoint()[i]);

    // design point physical space
    resultsTableModel->setNotEditableItem(i+2, 3, result_.getPhysicalSpaceDesignPoint()[i]);
  }

  // resize to contents
  resultsTable->resizeToContents();
  resultsTable->setSpan(0, 2, 1, 2);

  groupBoxLayout->addWidget(resultsTable);
  tabLayout->addWidget(groupBox);

  // importance factors
  groupBox = new QGroupBox(tr("Importance factors pie chart"));
  groupBoxLayout = new QVBoxLayout(groupBox);
  PieChartView * pieChart = new PieChartView(importanceFactors);
  pieChart->setPlotName(tr("importanceFactors"));
  groupBoxLayout->addWidget(pieChart, 0, Qt::AlignCenter);
  tabLayout->addWidget(groupBox);

  scrollArea->setWidget(tab);

  addTab(scrollArea, tr("Design point"));

  // sensitivity --------------------
  tab = new QWidget;
  tabLayout = new QVBoxLayout(tab);

  try
  {
    // compute sensitivities
    AnalyticalResult::Sensitivity eventProbaSensitivity(result_.getEventProbabilitySensitivity());
    AnalyticalResult::Sensitivity hasoferIndexSensitivity(result_.getHasoferReliabilityIndexSensitivity());

    // if the computation of sensitivities has succeeded
    scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    tabLayout->setSizeConstraint(QLayout::SetFixedSize);
    groupBox = new QGroupBox(tr("Sensitivities"));
    groupBoxLayout = new QVBoxLayout(groupBox);

    resultsTable = new ResizableTableViewWithoutScrollBar;
    resultsTable->horizontalHeader()->hide();
    resultsTable->verticalHeader()->hide();

    resultsTableModel = new CustomStandardItemModel(inDimension + 2, 4, resultsTable);
    resultsTable->setModel(resultsTableModel);

    // horizontal header
    resultsTableModel->setNotEditableHeaderItem(0, 0, tr("Variable"));
    resultsTableModel->setNotEditableHeaderItem(0, 1, tr("Distribution parameters"));
    resultsTableModel->setNotEditableHeaderItem(0, 2, tr("Failure probability"));
    resultsTableModel->setNotEditableHeaderItem(0, 3, tr("Reliability index"));

    // set values
    int row = 1;
    for (UnsignedInteger i = 0; i < inDimension; ++i)
    {
      // variable name
      const int varRow = row;
      resultsTableModel->setNotEditableItem(row, 0, inDescription[i].c_str());

      PointWithDescription pfSensitivity(eventProbaSensitivity[i]);
      const PointWithDescription betaSensitivity(hasoferIndexSensitivity[i]);

      for (UnsignedInteger j = 0; j < betaSensitivity.getDimension(); ++j)
      {
        int col = 0;
        // distribution parameter name
        String parameterName = betaSensitivity.getDescription()[j];
        parameterName = parameterName.substr(0, betaSensitivity.getDescription()[j].find("_marginal"));
        parameterName = parameterName.substr(0, betaSensitivity.getDescription()[j].find("_0"));
        resultsTableModel->setNotEditableItem(row, ++col, QString::fromUtf8(parameterName.c_str()));

        // sensitivity value pf
        if (pfSensitivity.getSize())
          resultsTableModel->setNotEditableItem(row, ++col, pfSensitivity[j]);

        // sensitivity value beta
        resultsTableModel->setNotEditableItem(row, ++col, betaSensitivity[j]);

        ++row;
      }
      resultsTable->setSpan(varRow, 0, betaSensitivity.getDimension(), 1);
    }

    // resize to contents
    resultsTable->resizeToContents();

    groupBoxLayout->addWidget(resultsTable);
    tabLayout->addWidget(groupBox);

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
