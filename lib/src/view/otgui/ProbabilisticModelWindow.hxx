// ProbabilisticModelWindow.hxx

#ifndef PROBABILISTICMODELWINDOW_H
#define PROBABILISTICMODELWINDOW_H

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/ProbabilisticModelItem.hxx"
#include "otgui/PlotWidget.hxx"
#include "otgui/InputTableProbabilisticModel.hxx"
#include "otgui/CorrelationTableModel.hxx"
#include "otgui/GraphConfigurationWidget.hxx"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTableView>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QStackedLayout>

namespace OTGUI {
class ProbabilisticModelWindow : public OTguiSubWindow
{
  Q_OBJECT

public :
  ProbabilisticModelWindow(ProbabilisticModelItem * item);

protected:
  void buildInterface();
  void updateStochasticInputsTable();
  void updatePlots(OT::Distribution inputDistribution);

public slots:
  void updateDistributionWidgets(const QModelIndex & index);
  void updateDistribution();
  void updatePhysicalModel(const PhysicalModel &);
  void updateProbabilisticModel();
  void updateCorrelationTable();
  void showHideAdvancedWidgets(bool);
  void truncationParametersChanged();
  void truncationParametersStateChanged();
  void showHideGraphConfigurationWidget(int indexTab);
  void showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates);
signals:
  void graphWindowActivated(QTabWidget*);
  void graphWindowDeactivated(QTabWidget*);

private:
  PhysicalModel physicalModel_;
  int currentIndexTab_;
  QTableView * inputTableView_;
  InputTableProbabilisticModel * inputTableModel_;
  QStackedLayout * rightSideOfSplitterStackedLayout_;
  PlotWidget * pdfPlot_;
  PlotWidget * cdfPlot_;
  GraphConfigurationWidget * pdf_cdfPlotsConfigurationWidget_;
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
  CorrelationTableModel * correlationTableModel_;
  QTableView * correlationTableView_;
};
}
#endif