// SRCResultWindow.hxx

#ifndef SRCCRESULTWINDOW_H
#define SRCRESULTWINDOW_H

#include "otgui/ResultWindow.hxx"
#include "otgui/SensitivityAnalysisItem.hxx"
#include "otgui/SRCResult.hxx"
#include "otgui/GraphConfigurationWidget.hxx"

#include <QStackedLayout>

namespace OTGUI {
class SRCResultWindow : public OTguiSubWindow //ResultWindow
{
  Q_OBJECT

public:
  SRCResultWindow(SensitivityAnalysisItem * item);

protected:
  void buildInterface();

public slots:
  void updateIndicesPlot(int section, Qt::SortOrder order);
  void showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates);
signals:
  void graphWindowActivated(QTabWidget*);
  void graphWindowDeactivated(QTabWidget*);

private:
  SRCResult result_;
  std::vector<std::map<double, int> > indices_;
  QStackedLayout * plotLayout_;
  GraphConfigurationWidget * plotsConfigurationWidget_;
  QVector<PlotWidget*> listPlotWidgets_;
};
}
#endif