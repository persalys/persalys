//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of ApproximationAnalysis
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
#include "persalys/ApproximationResultWindow.hxx"

#include "persalys/ApproximationResultTabWidget.hxx"
#include "persalys/TranslationManager.hxx"
#include "persalys/FORMAnalysis.hxx"
#include "persalys/SORMAnalysis.hxx"

#include <openturns/Less.hxx>
#include <openturns/LessOrEqual.hxx>
#include <openturns/Greater.hxx>
#include <openturns/GreaterOrEqual.hxx>

#include <QSet>
#include <QSplitter>
#include <QVBoxLayout>
#include <QDebug>
#include <QTabWidget>

using namespace OT;

namespace PERSALYS
{
ApproximationResultWindow::ApproximationResultWindow(AnalysisItem* item, QWidget * parent)
  : ResultWindow(item, parent)
{
  QStringList limitStateNameList;
  ApproximationResultTabWidget * tabWidget = nullptr;
  bool isSystem = false;

  // get analysis pointer
  const auto * formAnalysis_ptr = dynamic_cast<FORMAnalysis*>(item->getAnalysis().getImplementation().get());
  const auto * sormAnalysis_ptr = dynamic_cast<SORMAnalysis*>(item->getAnalysis().getImplementation().get());

  // FORM result widget
  if (formAnalysis_ptr)
  {
    const LimitState & ls = formAnalysis_ptr->getLimitState();
    const Description outputNames = ls.getOutputNames();
    isSystem = ls.isSystemLimitState();
    for (UnsignedInteger i = 0; i < outputNames.getSize(); ++i)
      limitStateNameList << FormatLimitStateEntry(outputNames[i], ls.getOperator(i), ls.getThreshold(i));

    if (isSystem)
      tabWidget = new ApproximationResultTabWidget(formAnalysis_ptr->getResult().getMultiFORMResult(), *formAnalysis_ptr, outputNames, this);
    else
      tabWidget = new ApproximationResultTabWidget(formAnalysis_ptr->getResult().getFORMResult(), *formAnalysis_ptr, this);
  }
  // SORM result widget
  else if (sormAnalysis_ptr)
  {
    const LimitState & ls = sormAnalysis_ptr->getLimitState();
    const Description outputNames = ls.getOutputNames();
    limitStateNameList << FormatLimitStateEntry(outputNames[0], ls.getOperator(0), ls.getThreshold(0));
    tabWidget = new ApproximationResultTabWidget(sormAnalysis_ptr->getResult().getSORMResult(), *sormAnalysis_ptr, this);
  }
  else
  {
    throw InvalidArgumentException(HERE) << "The given analysis is not a FORMAnalysis or a SORMAnalysis";
  }

  // tab : model description --------------------------------
  if (modelDescriptionWidget_)
    tabWidget->addTab(modelDescriptionWidget_, tr("Model"));

  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  // title
  const QString methodName = TranslationManager::GetTranslatedParameterName(item->getAnalysis().getImplementation()->getParameters()[0].second);
  widgetLayout->addWidget(new TitleLabel(methodName, "user_manual/graphical_interface/probabilistic_analysis/user_manual_probabilistic_analysis.html#" + methodName.toLower() + "result"));

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // Limit state selection groupbox — only for system events (>1 limit state)
  if (isSystem)
  {
    auto * outputsGroupBox       = new QGroupBox(tr("Limit state"));
    auto * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);
    auto * outputsListWidget     = new VariablesListWidget;
    outputsListWidget->addItems(limitStateNameList);
    outputsListWidget->setCurrentRow(0);
    outputsLayoutGroupBox->addWidget(outputsListWidget);

    mainWidget->addWidget(outputsGroupBox);
    mainWidget->setStretchFactor(0, 1);

    // Wire limit state selection to dynamic tabs
    connect(outputsListWidget, &VariablesListWidget::currentRowChanged,
            tabWidget, &ApproximationResultTabWidget::setCurrentEvent);

    // Per-event tabs in system FORM: Summary (0), Design point (1)
    const QSet<int> perEventTabs{0, 1};
    connect(tabWidget, &QTabWidget::currentChanged, outputsGroupBox,
            [outputsGroupBox, perEventTabs](int idx) {
              outputsGroupBox->setVisible(perEventTabs.contains(idx));
            });
    // Initial state: tab 0 (Summary) is per-event → visible (default)
  }

  if (tabWidget)
  {
    mainWidget->addWidget(tabWidget);
    mainWidget->setStretchFactor(isSystem ? 1 : 0, 10);
  }
  widgetLayout->addWidget(mainWidget, 1);
}
}
