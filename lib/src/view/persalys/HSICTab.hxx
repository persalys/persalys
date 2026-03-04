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

#ifndef PERSALYS_HSICTAB_HXX
#define PERSALYS_HSICTAB_HXX

#include "persalys/DataSensitivityAnalysisResult.hxx"
#include "persalys/DesignOfExperiment.hxx"
#include "persalys/ResultWindow.hxx"
#include "persalys/SensitivityResultWidget.hxx"

#include <QTabWidget>

namespace PERSALYS
{
class PERSALYS_VIEW_API HSICTab : public QTabWidget
{
  Q_OBJECT

public:
  enum class Type {Global, Target, Conditional};

  HSICTab(
    const DataSensitivityAnalysisResult & result, 
    const DesignOfExperiment & design, 
    VariablesListWidget * outputsListWidget,
    Type type, 
    QWidget * parent = nullptr);

private:
  void addSubTab(const OT::Collection<OT::Point> & values, const QString & title, SensitivityResultWidget::Type widgetType);

private:
  DataSensitivityAnalysisResult result_;
  DesignOfExperiment designOfExperiment_;
  VariablesListWidget * outputsListWidget_ = nullptr;
  Type type_;
  QStringList inputNames_;
  QStringList outputNames_;
};

} //namespace PERSALYS

#endif // PERSALYS_HSICTAB_HXX