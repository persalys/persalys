// SRCCalculusResultWindow.hxx

#ifndef SRCCALCULUSRESULTWINDOW_H
#define SRCCALCULUSRESULTWINDOW_H

#include "otgui/ResultWindow.hxx"
#include "otgui/SensitivityAnalysisItem.hxx"
#include "otgui/SRCCalculusResult.hxx"
#include "otgui/PlotWidget.hxx"

#include <QComboBox>
#include <QTableWidget>

namespace OTGUI {
class SRCCalculusResultWindow : public OTguiSubWindow //ResultWindow
{
  Q_OBJECT

public:
  SRCCalculusResultWindow(SensitivityAnalysisItem * item);

protected:
  void buildInterface();

public slots:
  void outputChanged(int index);
  void updateBarChart();
  void updateTable(int index);

private:
  SRCCalculusResult result_;
  std::vector<std::map<double, int> > indices_;
  QComboBox * outputsComboBox_;
  QComboBox * sortComboBox_;
  PlotWidget * barChart_;
  QTableWidget * table_;
};
}
#endif