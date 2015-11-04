// ParametricCalculusResultWindow.hxx

#ifndef PARAMETRICCALCULUSRESULTWINDOW_H
#define PARAMETRICCALCULUSRESULTWINDOW_H

#include "otgui/ResultWindow.hxx"
#include "otgui/ParametricCalculusItem.hxx"
#include "otgui/PlotWidget.hxx"
#include "otgui/ParametricCalculusResult.hxx"

#include <QComboBox>

namespace OTGUI {
class ParametricCalculusResultWindow : public OTguiSubWindow //ResultWindow
{
  Q_OBJECT

public:
  ParametricCalculusResultWindow(ParametricCalculusItem * item);

protected:
  void buildInterface();

public slots:
  void updateLabelsText(int indexOutput=0);
  void updateScatterPlot(int indexInput=0, int indexOutput=0);
  void inputScatterPlotChanged(int index);
  void outputScatterPlotChanged(int index);

private:
  ParametricCalculusResult result_;
  QComboBox * outputsComboBoxFirstTab_;
  QLabel * minLabel_;
  QLabel * maxLabel_;
  PlotWidget * scatterPlot_;
  QComboBox * inputsComboBox_;
  QComboBox * outputsComboBoxSecondTab_;
};
}
#endif