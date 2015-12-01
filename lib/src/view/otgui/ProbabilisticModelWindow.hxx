// ProbabilisticModelWindow.hxx

#ifndef PROBABILISTICMODELWINDOW_H
#define PROBABILISTICMODELWINDOW_H

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/ProbabilisticModelItem.hxx"
#include "otgui/PlotWidget.hxx"
#include "otgui/InputTableProbabilisticModel.hxx"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTableView>
#include <QComboBox>
#include <QCheckBox>

namespace OTGUI {
class ProbabilisticModelWindow : public OTguiSubWindow
{
  Q_OBJECT

public :
  ProbabilisticModelWindow(ProbabilisticModelItem * item);

protected:
  void buildInterface();
  void updateDeterministicInputsComboBox();
  void populateDeterministicInputsComboBox();
  void updatePlots(OT::Distribution inputDistribution);

public slots:
  void updateDistributionWidgets(const QModelIndex & index);
  void updateDistribution();
  void updateProbabilisticModel();
  void addInputRequested(int comboIndex);
  void removeInputRequested();
  void showHideAdvancedWidgets(bool);
  void truncationParametersChanged();
  void truncationParametersStateChanged();

private:
  PhysicalModel physicalModel_;
  QTableView * inputTableView_;
  QComboBox * deterministicInputsComboBox_;
  QStandardItemModel * deterministicInputsComboBoxModel_;
  InputTableProbabilisticModel * inputTableModel_;
  QFrame * rightFrame_;
  PlotWidget * pdfPlot_;
  PlotWidget * cdfPlot_;
  QGroupBox * paramEditor_;
  QVBoxLayout * parameterLayout_;
  QLabel * parameterValuesLabel_[5];
  QLineEdit * parameterValuesEdit_[5];
  QGroupBox * advancedGroup_;
  QWidget * advancedWidgets_;
  QCheckBox * lowerBoundCheckBox_;
  QCheckBox * upperBoundCheckBox_;
  QLineEdit * lowerBoundLineEdit_;
  QLineEdit * upperBoundLineEdit_;
};
}
#endif