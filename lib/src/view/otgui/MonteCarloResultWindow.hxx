// MonteCarloResultWindow.hxx

#ifndef MONTECARLORESULTWINDOW_H
#define MONTECARLORESULTWINDOW_H

#include "otgui/ResultWindow.hxx"
#include "otgui/CentralTendencyItem.hxx"
#include "otgui/MonteCarloResult.hxx"
#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/PlotMatrixConfigurationWidget.hxx"

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
  void showHideGraphConfigurationWidget(int indexTab);
  void showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates);
signals:
  void graphWindowActivated(QTabWidget*);
  void graphWindowDeactivated(QTabWidget*);

private:
  MonteCarloResult result_;
  PhysicalModel physicalModel_;
  bool isConfidenceIntervalRequired_;
  double levelConfidenceInterval_;
  QTabWidget * tabWidget_;
  QComboBox * outputsComboBoxFirstTab_;
  GraphConfigurationWidget * pdf_cdfPlotsConfigurationWidget_;
  GraphConfigurationWidget * boxPlotsConfigurationWidget_;
  GraphConfigurationWidget * scatterPlotsConfigurationWidget_;
  PlotMatrixConfigurationWidget * plotMatrixConfigurationWidget_;
  PlotMatrixConfigurationWidget * plotMatrix_X_X_ConfigurationWidget_;
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