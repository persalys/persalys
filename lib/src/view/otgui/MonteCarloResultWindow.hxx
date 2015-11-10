// MonteCarloResultWindow.hxx

#ifndef MONTECARLORESULTWINDOW_H
#define MONTECARLORESULTWINDOW_H

#include "otgui/ResultWindow.hxx"
#include "otgui/CentralTendencyItem.hxx"
#include "otgui/MonteCarloResult.hxx"
#include "otgui/PlotWidget.hxx"

#include <QComboBox>
#include <QDoubleSpinBox>

namespace OTGUI {
class MonteCarloResultWindow : public OTguiSubWindow //ResultWindow
{
  Q_OBJECT

public:
  MonteCarloResultWindow(CentralTendencyItem * item);

protected:
  void buildInterface();
  void updateLabelsText(int indexOutput=0);
  void updateScatterPlot(int indexInput=0, int indexOutput=0);

public slots:
  void outputFirstTabChanged(int indexOutput);
  void probaValueChanged(double proba);
  void quantileValueChanged(double quantile);
  void outputBoxPlotChanged(int indexOutput);
  void inputScatterPlotChanged(int index);
  void outputScatterPlotChanged(int index);

private:
  MonteCarloResult result_;
  bool isConfidenceIntervalRequired_;
  double levelConfidenceInterval_;
  PlotWidget * pdfPlot_;
  PlotWidget * cdfPlot_;
  QComboBox * outputsComboBoxFirstTab_;
  PlotWidget * boxPlot_;
  QComboBox * outputsComboBoxSecondTab_;
  PlotWidget * scatterPlot_;
  QComboBox * inputsComboBox_;
  QComboBox * outputsComboBoxThirdTab_;
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