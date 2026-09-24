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

  int hsicType = -1;
  int r2Type = -1;
  int permType = -1;
  int asymType = -1;
  switch (type)
  {
    case DataSensitivityAnalysisResult::Global:
      hsicType = SensitivityResultWidget::GlobalHSICIndices;
      r2Type = SensitivityResultWidget::GlobalHSICR2Indices;
      permType = SensitivityResultWidget::GlobalHSICPValuesPermutation;
      asymType = SensitivityResultWidget::GlobalHSICPValuesAsymptotic;
      break;
    case DataSensitivityAnalysisResult::Target:
      hsicType = SensitivityResultWidget::TargetHSICIndices;
      r2Type = SensitivityResultWidget::TargetHSICR2Indices;
      permType = SensitivityResultWidget::TargetHSICPValuesPermutation;
      asymType = SensitivityResultWidget::TargetHSICPValuesAsymptotic;
      break;
    case DataSensitivityAnalysisResult::Conditional:
      hsicType = SensitivityResultWidget::ConditionalHSICIndices;
      r2Type = SensitivityResultWidget::ConditionalHSICR2Indices;
      permType = SensitivityResultWidget::ConditionalHSICPValuesPermutation;
      asymType = SensitivityResultWidget::ConditionalHSICPValuesAsymptotic;
      break;
  }

  if (result.computeHSICPValuesPermutation(type))
    addSubTab(result.getPValuesPermutation(type), tr("HSIC permutations p-values"), permType);
  if (result.computeHSICPValuesAsymptotic(type))
    addSubTab(result.getPValuesAsymptotic(type), tr("HSIC asymptotic p-values"), asymType);
  addSubTab(result.getR2HSICIndices(type), tr("R2-HSIC Indices"), r2Type);
  addSubTab(result.getHSICIndices(type), tr("HSIC Indices"), hsicType);
}

void HSICTab::addSubTab(const Collection<Point> & values, const QString & title, int widgetType)
{
  const UnsignedInteger nbOutputs = values.getSize();
  const UnsignedInteger nbInputs = QtOT::StringListToDescription(inputNames_).getSize();

  auto * stackedWidget = new ResizableStackedWidget;
  connect(outputsListWidget_, &VariablesListWidget::currentRowChanged, stackedWidget, &ResizableStackedWidget::setCurrentIndex);

  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
   // Defensive check: skip inconsistent results instead of crashing (out-of-bound access)
   if (values[i].getSize() != nbInputs)
     throw InvalidArgumentException(HERE) << "HSIC results are inconsistent with the inputs: got " << values[i].getSize()
       << " indices for " << nbInputs << " input variables (output index " << i << "). The analysis may have failed (e.g. constant input variable).";
   if (i >= (UnsignedInteger)outputNames_.size())
     throw InvalidArgumentException(HERE) << "HSIC results are inconsistent with the outputs: got " << nbOutputs
       << " outputs but only " << (UnsignedInteger)outputNames_.size() << " output names.";
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
