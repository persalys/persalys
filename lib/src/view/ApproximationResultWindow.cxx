//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of ApproximationAnalysis
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
  QString outputName;
  ApproximationResultTabWidget * tabWidget = 0;

  // get analysis pointer
  FORMAnalysis * formAnalysis_ptr = dynamic_cast<FORMAnalysis*>(item->getAnalysis().getImplementation().get());
  SORMAnalysis * sormAnalysis_ptr = dynamic_cast<SORMAnalysis*>(item->getAnalysis().getImplementation().get());

  // FORM result widget
  if (formAnalysis_ptr)
  {
    outputName = QString::fromUtf8(formAnalysis_ptr->getLimitState().getOutputName().c_str());
    tabWidget = new ApproximationResultTabWidget(formAnalysis_ptr->getResult().getFORMResult(), *formAnalysis_ptr, this);
  }
  // SORM result widget
  else if (sormAnalysis_ptr)
  {
    outputName = QString::fromUtf8(sormAnalysis_ptr->getLimitState().getOutputName().c_str());
    tabWidget = new ApproximationResultTabWidget(sormAnalysis_ptr->getResult().getSORMResult(), *sormAnalysis_ptr, this);
  }
  else
  {
    qDebug() << "In ApproximationResultWindow: the given analysis is not a FORMAnalysis or a SORMAnalysis";
    throw std::exception();
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

  // - output
  QGroupBox * outputsGroupBox = new QGroupBox(tr("Output"));
  QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);

  VariablesListWidget * outputsListWidget = new VariablesListWidget;
  outputsListWidget->addItems(QStringList() << outputName);
  outputsListWidget->setCurrentRow(0);
  outputsLayoutGroupBox->addWidget(outputsListWidget);

  mainWidget->addWidget(outputsGroupBox);
  mainWidget->setStretchFactor(0, 1);

  if (tabWidget)
  {
    mainWidget->addWidget(tabWidget);
    mainWidget->setStretchFactor(1, 10);
  }
  widgetLayout->addWidget(mainWidget, 1);
}
}
