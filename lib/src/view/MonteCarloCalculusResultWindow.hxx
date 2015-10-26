// MonteCarloCalculusResultWindow.hxx

#ifndef MONTECARLOCALCULUSRESULTWINDOW_H
#define MONTECARLOCALCULUSRESULTWINDOW_H

#include "ResultWindow.hxx"
#include "DistributionAnalysisItem.hxx"
#include "MonteCarloResult.hxx"
#include "PlotWidget.hxx"

#include <QComboBox>
#include <QDoubleSpinBox>

namespace OTGUI {
class MonteCarloCalculusResultWindow : public OTguiSubWindow //ResultWindow
{
  Q_OBJECT

public:
  MonteCarloCalculusResultWindow(DistributionAnalysisItem * item);

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