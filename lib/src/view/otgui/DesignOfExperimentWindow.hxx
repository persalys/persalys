// DesignOfExperimentWindow.hxx

#ifndef DESIGNOFEXPERIMENTWINDOW_H
#define DESIGNOFEXPERIMENTWINDOW_H

#include "OTguiSubWindow.hxx"
#include "otgui/DesignOfExperimentItem.hxx"
#include "OTguiTableView.hxx"
#include "otgui/PlotWidget.hxx"

#include <QTabWidget>
#include <QPushButton>
#include <QComboBox>

namespace OTGUI {
class DesignOfExperimentWindow : public OTguiSubWindow
{
  Q_OBJECT

public:
  DesignOfExperimentWindow(DesignOfExperimentItem * item);

protected:
  void buildInterface();
  void addTabsForOutputs();

public slots:
  void evaluateOutputs();
  void updateLabelsText(int indexOutput=0);
  void updateScatterPlot(int indexInput=0, int indexOutput=0);
  void inputScatterPlotChanged(int index);
  void outputScatterPlotChanged(int index);

private:
  DesignOfExperiment designOfExperiment_;
  QTabWidget * tabWidget_;
  OTguiTableView * tableView_;
  QPushButton * evaluateButton_;
  QComboBox * outputsComboBoxFirstTab_;
  QLabel * minLabel_;
  QLabel * maxLabel_;
  PlotWidget * scatterPlot_;
  QComboBox * inputsComboBox_;
  QComboBox * outputsComboBoxSecondTab_;
};
}
#endif