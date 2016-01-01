// MonteCarloResultWindow.hxx

#ifndef MONTECARLORESULTWINDOW_H
#define MONTECARLORESULTWINDOW_H

#include "otgui/ResultWindow.hxx"
#include "otgui/CentralTendencyItem.hxx"
#include "otgui/MonteCarloResult.hxx"
#include "otgui/PlotWidget.hxx"
#include "otgui/GraphConfigurationWidget.hxx"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>

namespace OTGUI {
class MonteCarloResultWindow : public OTguiSubWindow //ResultWindow
{
  Q_OBJECT

public:
  MonteCarloResultWindow(CentralTendencyItem * item);

protected:
  void buildInterface();
  void updateLabelsText(int indexOutput=0);

public slots:
  void outputFirstTabChanged(int indexOutput);
  void probaValueChanged(double proba);
  void quantileValueChanged(double quantile);
  void outputBoxPlotChanged(int indexOutput);
  void showHideGraphConfigurationWidget(int indexTab);
  void showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates);
signals:
  void graphWindowActivated(GraphConfigurationWidget*);
  void graphWindowDeactivated(GraphConfigurationWidget*);

private:
  MonteCarloResult result_;
  PhysicalModel physicalModel_;
  bool isConfidenceIntervalRequired_;
  double levelConfidenceInterval_;
  QTabWidget * tabWidget_;
  PlotWidget * pdfPlot_;
  PlotWidget * cdfPlot_;
  QComboBox * outputsComboBoxFirstTab_;
  PlotWidget * boxPlot_;
  QComboBox * outputsComboBoxSecondTab_;
  GraphConfigurationWidget * graphConfigurationWidget_;
  QLabel * meanLabel_;
  QLabel * stdLabel_;
  QLabel * skewnessLabel_;
  QLabel * kurtosisLabel_;
  QLabel * minLabel_;
  QLabel * maxLabel_;
  QLabel * firstQuartileLabel_;
  QLabel * thirdQuartileLabel_;
  QDoubleSpinBox * probaSpinBox_;
  QDoubleSpinBox * quantileSpinBox_;
};
}
#endif