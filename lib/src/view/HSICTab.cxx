//                                               -*- C++ -*-
/**
 *  @brief HSIC tab of the DataSensitivityAnalysisResultWindow
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

#include "persalys/HSICTab.hxx"
#include "persalys/ResizableStackedWidget.hxx"

using namespace OT;

namespace PERSALYS
{

HSICTab::HSICTab(
  const DataSensitivityAnalysisResult & result, 
  const DesignOfExperiment & design, 
  VariablesListWidget * outputsListWidget,
  Type type, 
  QWidget * parent)
  : QTabWidget(parent)
  , result_(result)
  , designOfExperiment_(design)
  , outputsListWidget_(outputsListWidget)
  , type_(type)
{
  inputNames_ = QtOT::DescriptionToStringList(designOfExperiment_.getInputSample().getDescription());
  outputNames_ = QtOT::DescriptionToStringList(designOfExperiment_.getOutputSample().getDescription());

  switch (type)
  {
    case Type::Global:
      addSubTab(result.getGlobalHSICIndices(), tr("Global HSIC indices"), SensitivityResultWidget::GlobalHSICIndices);
      addSubTab(result.getGlobalR2HSICIndices(), tr("Global HSIC R² indices"), SensitivityResultWidget::GlobalHSICR2Indices);
      if(result.computeHSICPValuesPermutation())
        addSubTab(result.getGlobalPValuesPermutation(), tr("Global HSIC p-values (permutation)"), SensitivityResultWidget::GlobalHSICPValuesPermutation);
      if(result.computeHSICPValuesAsymptotic())
        addSubTab(result.getGlobalPValuesAsymptotic(), tr("Global HSIC p-values (asymptotic)"), SensitivityResultWidget::GlobalHSICPValuesAsymptotic);
      break;
    
    default:
      // Not yet implemented for Target and Conditional HSIC
      break;
  }
}

void HSICTab::addSubTab(const Collection<Point> & values, const QString & title, SensitivityResultWidget::Type widgetType)
{
  const UnsignedInteger nbOutputs = designOfExperiment_.getOutputSample().getDimension();

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
    widgetType,
    designOfExperiment_.getType(),
    this
   );

   stackedWidget->addWidget(resultWidget);
  }

  addTab(stackedWidget, title);
}

} //namespace PERSALYS