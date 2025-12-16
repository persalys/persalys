//                                               -*- C++ -*-
/**
 *  @brief QStackedWidget to define marginals parameters
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
#ifndef PERSALYS_MARGINALSWIDGET_HXX
#define PERSALYS_MARGINALSWIDGET_HXX

#include "persalys/PhysicalModelItem.hxx"
#include "persalys/PlotWidget.hxx"
#include "persalys/InputTableProbabilisticModel.hxx"
#include "persalys/CollapsibleGroupBox.hxx"
#include "persalys/ValueLineEdit.hxx"
#include "persalys/ErrorWidget.hxx"
#include "persalys/Study.hxx"
#include "persalys/ResizableStackedWidget.hxx"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QTableView>
#include <QComboBox>
#include <QCheckBox>
#include <QStackedWidget>
#include <QToolButton>
#include <QLabel>

namespace PERSALYS
{
class PERSALYS_VIEW_API MarginalsWidget : public QWidget
{
  Q_OBJECT

public :
  explicit MarginalsWidget(PhysicalModelItem * item, QWidget *parent = nullptr);
  explicit MarginalsWidget(const PhysicalModel& model, QWidget *parent = nullptr);

protected:
  void buildInterface();
  void updatePlots();

public slots:
  void openUrl();
  void updateDistributionWidgets(const QModelIndex & index, const QModelIndex & prevIndex = QModelIndex());
  void updateCurrentVariableDistributionWidgets();
  void updateDistributionParametersWidgets(const QModelIndex & index);
  void updateTruncationParametersWidgets(const QModelIndex & index);
  void updateProbabilisticModel();
  void distributionParametersChanged();
  void typeDistributionParametersChanged(int);
  void truncationParametersChanged();
  void truncationParametersStateChanged();
  void openWizardToChooseInferenceResult(const QModelIndex&);
#ifdef PERSALYS_HAVE_OTMORRIS
  void openWizardToChooseScreeningResult();
#endif
  void openValuesDefinitionWizard();
signals:
  void updateDependenciesRequested();

private:
  bool                          failSoftMode_                       = false;
  Study                         study_;
  PhysicalModel                 physicalModel_;
  QTableView                    * inputTableView_                   = nullptr;
  InputTableProbabilisticModel  * inputTableModel_                  = nullptr;
  ResizableStackedWidget        * rightSideOfSplitterStackedWidget_ = nullptr;
  ValueLineEdit                 * valueForDeterministicVariable_    = nullptr;
  PlotWidget                    * pdfPlot_                          = nullptr;
  PlotWidget                    * cdfPlot_                          = nullptr;
  PlotWidget                    * quantilePlot_                     = nullptr;
  PlotWidget                    * survivalPlot_                     = nullptr;
  QComboBox                     * selectParametersTypeCombo_        = nullptr;
  std::array<QLabel*, 5>        parameterValuesLabel_               = {nullptr, nullptr, nullptr, nullptr, nullptr};
  std::array<ValueLineEdit*, 5> parameterValuesEdit_                = {nullptr, nullptr, nullptr, nullptr, nullptr};
  QToolButton                   * editButton_                       = nullptr;
  CollapsibleGroupBox           * truncationParamGroupBox_          = nullptr;
  QCheckBox                     * lowerBoundCheckBox_               = nullptr;
  QCheckBox                     * upperBoundCheckBox_               = nullptr;
  ValueLineEdit                 * lowerBoundLineEdit_               = nullptr;
  ValueLineEdit                 * upperBoundLineEdit_               = nullptr;
  ErrorWidget                   * errorWidget_                = nullptr;
};
}
#endif
