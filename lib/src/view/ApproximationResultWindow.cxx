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

#include <QSplitter>
#include <QVBoxLayout>
#include <QDebug>
#include <QTabWidget>

using namespace OT;

namespace PERSALYS
{

/** Format a single limit-state event as "outputName op threshold". */
static QString formatLimitStateEntry(const String & outputName,
                                     const ComparisonOperator & op,
                                     double threshold)
{
  QString opStr;
  const String opName = op.getImplementation()->getClassName();
  if      (opName == "LessOrEqual")    opStr = "<=";
  else if (opName == "Greater")        opStr = ">";
  else if (opName == "GreaterOrEqual") opStr = ">=";
  else                                 opStr = "<";
  return QString("%1 %2 %3")
    .arg(QString::fromUtf8(outputName.c_str()), opStr, QString::number(threshold));
}

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
      limitStateNameList << formatLimitStateEntry(outputNames[i], ls.getOperator(i), ls.getThreshold(i));

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
    limitStateNameList << formatLimitStateEntry(outputNames[0], ls.getOperator(0), ls.getThreshold(0));
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

  // - limit state list
  QGroupBox * outputsGroupBox = new QGroupBox(tr("Limit state"));
  QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);

  VariablesListWidget * outputsListWidget = new VariablesListWidget;
  outputsListWidget->addItems(limitStateNameList);
  outputsListWidget->setCurrentRow(0);
  outputsLayoutGroupBox->addWidget(outputsListWidget);

  mainWidget->addWidget(outputsGroupBox);
  mainWidget->setStretchFactor(0, 1);

  if (tabWidget)
  {
    // Wire limit state selection to dynamic tabs (system FORM only)
    if (isSystem)
      connect(outputsListWidget, &VariablesListWidget::currentRowChanged,
              tabWidget, &ApproximationResultTabWidget::setCurrentEvent);

    mainWidget->addWidget(tabWidget);
    mainWidget->setStretchFactor(1, 10);
  }
  widgetLayout->addWidget(mainWidget, 1);
}
}
