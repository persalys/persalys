// SobolResultWindow.hxx

#ifndef SOBOLRESULTWINDOW_H
#define SOBOLRESULTWINDOW_H

#include "otgui/ResultWindow.hxx"
#include "otgui/SensitivityAnalysisItem.hxx"
#include "otgui/SobolResult.hxx"
#include "otgui/GraphConfigurationWidget.hxx"

#include <QStackedLayout>

namespace OTGUI {
class SobolResultWindow : public OTguiSubWindow //ResultWindow
{
  Q_OBJECT

public:
  SobolResultWindow(SensitivityAnalysisItem * item);

protected:
  void buildInterface();

public slots:
  void updateIndicesPlot(int section, Qt::SortOrder order);
  void showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates);
signals:
  void graphWindowActivated(QTabWidget*);
  void graphWindowDeactivated(QTabWidget*);

private:
  SobolResult result_;
  std::vector<std::map<double, int> > firstOrderIndices_;
  std::vector<std::map<double, int> > totalOrderIndices_;
  QStackedLayout * frameLayout_;
  GraphConfigurationWidget * plotsConfigurationWidget_;
  QVector<PlotWidget*> listPlotWidgets_;
};
}
#endif