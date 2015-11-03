// SobolCalculusResultWindow.hxx

#ifndef SOBOLCALCULUSRESULTWINDOW_H
#define SOBOLCALCULUSRESULTWINDOW_H

#include "ResultWindow.hxx"
#include "SensitivityAnalysisItem.hxx"
#include "SobolCalculusResult.hxx"
#include "PlotWidget.hxx"

#include <QComboBox>
#include <QTableWidget>

namespace OTGUI {
class SobolCalculusResultWindow : public OTguiSubWindow //ResultWindow
{
  Q_OBJECT

public:
  SobolCalculusResultWindow(SensitivityAnalysisItem * item);

protected:
  void buildInterface();

public slots:
  void outputChanged(int index);
  void updateBarChart();
  void updateTable(int index);

private:
  SobolCalculusResult result_;
  std::vector<std::map<double, int> > firstOrderIndices_;
  std::vector<std::map<double, int> > totalOrderIndices_;
  QComboBox * outputsComboBox_;
  QComboBox * sortComboBox_;
  PlotWidget * barChart_;
  QTableWidget * table_;
};
}
#endif