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

namespace OTGUI {
class ProbabilisticModelWindow : public OTguiSubWindow
{
  Q_OBJECT

public :
  ProbabilisticModelWindow(ProbabilisticModelItem * item);

protected:
  void buildInterface();

public slots:
  void updateDistributionWidgets(const QModelIndex & index);
  void updateDistribution();

private:
  ProbabilisticModelItem * item_;
  PhysicalModel physicalModel_;
  QTableView * inputTableView_;
  InputTableProbabilisticModel * inputTableModel_;
  PlotWidget * pdfPlot_;
  PlotWidget * cdfPlot_;
  QGroupBox * paramEditor_;
  QHBoxLayout * parameterLayout_;
  QLabel * parameterValuesLabel_[2];
  QLineEdit * parameterValuesEdit_[2];
};
}
#endif