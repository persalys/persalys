//                                               -*- C++ -*-
/**
 *  @brief QStackedWidget to define marginals parameters
 *
 *  Copyright 2015-2019 EDF-Phimeca
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

#include "persalys/ProbabilisticModelItem.hxx"
#include "persalys/PlotWidget.hxx"
#include "persalys/InputTableProbabilisticModel.hxx"
#include "persalys/CollapsibleGroupBox.hxx"
#include "persalys/ValueLineEdit.hxx"
#include "persalys/TemporaryLabel.hxx"
#include "persalys/Study.hxx"
#include "persalys/ResizableStackedWidget.hxx"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QTableView>
#include <QComboBox>
#include <QCheckBox>
#include <QStackedWidget>

namespace PERSALYS
{
class PERSALYS_API MarginalsWidget : public QWidget
{
  Q_OBJECT

public :
  MarginalsWidget(ProbabilisticModelItem * item, QWidget * parent = 0);
  MarginalsWidget(const PhysicalModel& model, QWidget * parent = 0);

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
signals:
  void updateDependenciesRequested();

private:
  bool failSoftMode_;
  Study study_;
  PhysicalModel physicalModel_;
  QTableView * inputTableView_;
  InputTableProbabilisticModel * inputTableModel_;
  ResizableStackedWidget * rightSideOfSplitterStackedWidget_;
  ValueLineEdit * valueForDeterministicVariable_;
  PlotWidget * pdfPlot_;
  PlotWidget * cdfPlot_;
  PlotWidget * quantilePlot_;
  PlotWidget * survivalPlot_;
  QComboBox * selectParametersTypeCombo_;
  QLabel * parameterValuesLabel_[5];
  ValueLineEdit * parameterValuesEdit_[5];
  CollapsibleGroupBox * truncationParamGroupBox_;
  QCheckBox * lowerBoundCheckBox_;
  QCheckBox * upperBoundCheckBox_;
  ValueLineEdit * lowerBoundLineEdit_;
  ValueLineEdit * upperBoundLineEdit_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif
