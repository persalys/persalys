//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define a probabilistic model
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
#ifndef OTGUI_PROBABILISTICMODELWINDOW_HXX
#define OTGUI_PROBABILISTICMODELWINDOW_HXX

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/ProbabilisticModelItem.hxx"
#include "otgui/PlotWidget.hxx"
#include "otgui/InputTableProbabilisticModel.hxx"
#include "otgui/CopyableTableView.hxx"
#include "otgui/CollapsibleGroupBox.hxx"
#include "otgui/ValueLineEdit.hxx"
#include "otgui/OTStudy.hxx"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QTableView>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QStackedWidget>

namespace OTGUI
{
class OTGUI_API ProbabilisticModelWindow : public OTguiSubWindow
{
  Q_OBJECT

public :
  ProbabilisticModelWindow(ProbabilisticModelItem * item, QWidget * parent = 0);

protected:
  void buildInterface();
  void updatePlots();

public slots:
  void openUrl();
  void updateDistributionWidgets(const QModelIndex & index);
  void updateCurrentVariableDistributionWidgets();
  void updateDistributionParametersWidgets(const QModelIndex & index);
  void updateTruncationParametersWidgets(const QModelIndex & index);
  void updateProbabilisticModel();
  void setCorrelationTabErrorMessage(const QString &);
  void distributionParametersChanged();
  void typeDistributionParametersChanged(int);
  void truncationParametersChanged();
  void truncationParametersStateChanged();
  void reInitCorrelationErrorMessage(QTimeLine::State);
  void openWizardToChooseInferenceResult(const QModelIndex&);
#ifdef OTGUI_HAVE_OTMORRIS
  void openWizardToChooseScreeningResult();
#endif
signals:
  void updateCorrelationTableData();

private:
  PhysicalModel physicalModel_;
  QTableView * inputTableView_;
  InputTableProbabilisticModel * inputTableModel_;
  QStackedWidget * rightSideOfSplitterStackedWidget_;
  ValueLineEdit * valueForDeterministicVariable_;
  PlotWidget * pdfPlot_;
  PlotWidget * cdfPlot_;
  QGroupBox * paramEditor_;
  QVBoxLayout * parameterLayout_;
  QLabel * parameterValuesLabel_[5];
  ValueLineEdit * parameterValuesEdit_[5];
  CollapsibleGroupBox * truncationParamGroupBox_;
  QCheckBox * lowerBoundCheckBox_;
  QCheckBox * upperBoundCheckBox_;
  ValueLineEdit * lowerBoundLineEdit_;
  ValueLineEdit * upperBoundLineEdit_;
  QLabel * correlationErrorMessage_;
  QList<QTimeLine*> qtimelineList_;
};
}
#endif
