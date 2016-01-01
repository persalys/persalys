// GraphConfigurationWidget.hxx

#ifndef GRAPHCONFIGURATIONWIDGET_H
#define GRAPHCONFIGURATIONWIDGET_H

#include "PlotWidget.hxx"
#include "otgui/Input.hxx"
#include "otgui/Output.hxx"

#include <QTabWidget>
#include <QLineEdit>
#include <QComboBox>

namespace OTGUI {

class GraphConfigurationWidget : public QTabWidget
{
  Q_OBJECT

public:
  GraphConfigurationWidget(QVector<PlotWidget *> plotWidgets, QStringList inputNames, QStringList outputNames);

public slots:
  void updateLineEdits();
  void plotChanged();
  void updateTitle();
  void updateXLabel();
  void updateYLabel();
  void updateXrange();
  void updateYrange();
  void exportPlot();
signals:
  void currentPlotChanged(int);

private:
  QVector<PlotWidget *> plotWidgets_;
  int currentPlotIndex_;
  QComboBox * xAxisComboBox_;
  QComboBox * yAxisComboBox_;
  QLineEdit * titleLineEdit_;
  QLineEdit * xlabelLineEdit_;
  QLineEdit * xmin_;
  QLineEdit * xmax_;
  QLineEdit * ylabelLineEdit_;
  QLineEdit * ymin_;
  QLineEdit * ymax_;
};
}
#endif