// ParametricCalculusResultWindow.hxx

#ifndef PARAMETRICCALCULUSRESULTWINDOW_H
#define PARAMETRICCALCULUSRESULTWINDOW_H

#include "ResultWindow.hxx"
#include "ParametricCalculusItem.hxx"
#include "PlotWidget.hxx"
#include <QComboBox>

namespace OTGUI {
class ParametricCalculusResultWindow : public OTguiSubWindow //ResultWindow
{
  Q_OBJECT

public:
  ParametricCalculusResultWindow(ParametricCalculusItem * item);

  virtual ~ParametricCalculusResultWindow();

protected:
  void buildInterface();

public slots:
  void updatePlot(int indexInput=0, int indexOutput=0);
  void inputChanged(int index);
  void outputChanged(int index);

private:
  OT::NumericalSample inputSample_;
  ParametricCalculusResult result_;
  PlotWidget * scatterPlot_;
  QComboBox * inputsComboBox_;
  QComboBox * outputsComboBox_;
};
}
#endif