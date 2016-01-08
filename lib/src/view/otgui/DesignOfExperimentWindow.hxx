// DesignOfExperimentWindow.hxx

#ifndef DESIGNOFEXPERIMENTWINDOW_H
#define DESIGNOFEXPERIMENTWINDOW_H

#include "OTguiSubWindow.hxx"
#include "otgui/DesignOfExperimentItem.hxx"
#include "OTguiTableView.hxx"
#include "otgui/PlotWidget.hxx"
#include "otgui/GraphConfigurationWidget.hxx"

#include <QTabWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>

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
  void showHideGraphConfigurationWidget(int indexTab);
  void showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates);
signals:
  void graphWindowActivated(QTabWidget*);
  void graphWindowDeactivated(QTabWidget*);

private:
  DesignOfExperiment designOfExperiment_;
  QTabWidget * tabWidget_;
  OTguiTableView * tableView_;
  QPushButton * evaluateButton_;
  QComboBox * outputsComboBoxFirstTab_;
  QLabel * minLabel_;
  QLabel * maxLabel_;
  GraphConfigurationWidget * graphConfigurationWidget_;
};
}
#endif