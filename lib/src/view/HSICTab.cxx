//                                               -*- C++ -*-
/**
 *  @brief HSIC tab of the DataSensitivityAnalysisResultWindow
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

#include "persalys/HSICTab.hxx"
#include "persalys/ResizableStackedWidget.hxx"

using namespace OT;

namespace PERSALYS
{

HSICTab::HSICTab(
  const DataSensitivityAnalysisResult & result, 
  const DesignOfExperiment & design, 
  VariablesListWidget * outputsListWidget,
  DataSensitivityAnalysisResult::HSICType type, 
  QWidget * parent)
  : QTabWidget(parent)
  , designOfExperiment_(design)
  , outputsListWidget_(outputsListWidget)
{
  inputNames_ = QtOT::DescriptionToStringList(designOfExperiment_.getInputSample().getDescription());
  outputNames_ = QtOT::DescriptionToStringList(designOfExperiment_.getOutputSample().getDescription());

  int widgetType = -1;
  switch (type)
  {
    case DataSensitivityAnalysisResult::Global:
      widgetType = SensitivityResultWidget::GlobalHSICIndices;
      break;
    case DataSensitivityAnalysisResult::Target:
      widgetType = SensitivityResultWidget::TargetHSICIndices;
      break;
    case DataSensitivityAnalysisResult::Conditional:
      widgetType = SensitivityResultWidget::ConditionalHSICIndices;
  }

  addSubTab(result.getHSICIndices(type), tr("HSIC Indices"), widgetType);
  ++widgetType;
  addSubTab(result.getR2HSICIndices(type), tr("R2-HSIC Indices"), widgetType);
  ++widgetType;
  if (result.computeHSICPValuesPermutation(type))
    addSubTab(result.getPValuesPermutation(type), tr("HSIC permutations p-values"), widgetType);
  ++widgetType;
  if (result.computeHSICPValuesAsymptotic(type))
    addSubTab(result.getPValuesAsymptotic(type), tr("HSIC asymptotic p-values"), widgetType);
}

void HSICTab::addSubTab(const Collection<Point> & values, const QString & title, int widgetType)
{
  const UnsignedInteger nbOutputs = values.getSize();

  auto * stackedWidget = new ResizableStackedWidget;
  connect(outputsListWidget_, &VariablesListWidget::currentRowChanged, stackedWidget, &ResizableStackedWidget::setCurrentIndex);

  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
   auto * resultWidget = new SensitivityResultWidget(
    values[i],
    Interval(),
    Point(),
    Interval(),
    QtOT::StringListToDescription(inputNames_),
    outputNames_[i].toStdString(),
    static_cast<SensitivityResultWidget::Type>(widgetType),
    designOfExperiment_.getType(),
    this
   );

   stackedWidget->addWidget(resultWidget);
  }

  addTab(stackedWidget, title);
}

} //namespace PERSALYS