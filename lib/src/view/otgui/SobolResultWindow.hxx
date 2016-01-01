// SobolResultWindow.hxx

#ifndef SOBOLRESULTWINDOW_H
#define SOBOLRESULTWINDOW_H

#include "otgui/ResultWindow.hxx"
#include "otgui/SensitivityAnalysisItem.hxx"
#include "otgui/SobolResult.hxx"
#include "otgui/PlotWidget.hxx"

#include <QComboBox>
#include <QTableWidget>

namespace OTGUI {
class SobolResultWindow : public OTguiSubWindow //ResultWindow
{
  Q_OBJECT

public:
  SobolResultWindow(SensitivityAnalysisItem * item);

protected:
  void buildInterface();

public slots:
  void outputChanged(int index);
  void updateIndicesPlot(int section, Qt::SortOrder order);
  void updateTable(int index);

private:
  SobolResult result_;
  std::vector<std::map<double, int> > firstOrderIndices_;
  std::vector<std::map<double, int> > totalOrderIndices_;
  QComboBox * outputsComboBox_;
  PlotWidget * indicesPlot_;
  QTableWidget * table_;
};
}
#endif