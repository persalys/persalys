// ProbabilisticModelWindow.hxx

#ifndef PROBABILISTICMODELWINDOW_H
#define PROBABILISTICMODELWINDOW_H

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/ProbabilisticModelItem.hxx"
#include "otgui/PlotWidget.hxx"
#include "otgui/InputTableProbabilisticModel.hxx"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTableView>
#include <QComboBox>

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

public slots:
  void updateDistributionWidgets(const QModelIndex & index);
  void updateDistribution();
  void updateProbabilisticModel();
  void addInputRequested(int comboIndex);
  void removeInputRequested();

private:
  PhysicalModel physicalModel_;
  QTableView * inputTableView_;
  QComboBox * deterministicInputsComboBox_;
  QStandardItemModel * deterministicInputsComboBoxModel_;
  InputTableProbabilisticModel * inputTableModel_;
  PlotWidget * pdfPlot_;
  PlotWidget * cdfPlot_;
  QGroupBox * paramEditor_;
  QHBoxLayout * parameterLayout_;
  QLabel * parameterValuesLabel_[5];
  QLineEdit * parameterValuesEdit_[5];
};
}
#endif