// SRCResultWindow.hxx

#ifndef SRCCRESULTWINDOW_H
#define SRCRESULTWINDOW_H

#include "otgui/ResultWindow.hxx"
#include "otgui/SensitivityAnalysisItem.hxx"
#include "otgui/SRCResult.hxx"
#include "otgui/PlotWidget.hxx"

#include <QComboBox>
#include <QTableWidget>

namespace OTGUI {
class SRCResultWindow : public OTguiSubWindow //ResultWindow
{
  Q_OBJECT

public:
  SRCResultWindow(SensitivityAnalysisItem * item);

protected:
  void buildInterface();

public slots:
  void outputChanged(int index);
  void updateBarChart();
  void updateTable(int index);

private:
  SRCResult result_;
  std::vector<std::map<double, int> > indices_;
  QComboBox * outputsComboBox_;
  QComboBox * sortComboBox_;
  PlotWidget * barChart_;
  QTableWidget * table_;
};
}
#endif