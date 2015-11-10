// ParametricAnalysisResultWindow.hxx

#ifndef PARAMETRICANALYSISRESULTWINDOW_H
#define PARAMETRICANALYSISRESULTWINDOW_H

#include "otgui/ResultWindow.hxx"
#include "otgui/ParametricAnalysisItem.hxx"
#include "otgui/PlotWidget.hxx"
#include "otgui/ParametricAnalysisResult.hxx"

#include <QComboBox>

namespace OTGUI {
class ParametricAnalysisResultWindow : public OTguiSubWindow //ResultWindow
{
  Q_OBJECT

public:
  ParametricAnalysisResultWindow(ParametricAnalysisItem * item);

protected:
  void buildInterface();

public slots:
  void updateLabelsText(int indexOutput=0);
  void updateScatterPlot(int indexInput=0, int indexOutput=0);
  void inputScatterPlotChanged(int index);
  void outputScatterPlotChanged(int index);

private:
  ParametricAnalysisResult result_;
  QComboBox * outputsComboBoxFirstTab_;
  QLabel * minLabel_;
  QLabel * maxLabel_;
  PlotWidget * scatterPlot_;
  QComboBox * inputsComboBox_;
  QComboBox * outputsComboBoxSecondTab_;
};
}
#endif