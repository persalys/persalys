// PlotMatrixConfigurationWidget.hxx

#ifndef PLOTMATRIXCONFIGURATIONWIDGET_H
#define PLOTMATRIXCONFIGURATIONWIDGET_H

#include "otgui/PlotMatrixWidget.hxx"

#include <QTabWidget>
#include <QLineEdit>

namespace OTGUI {

class PlotMatrixConfigurationWidget : public QTabWidget
{
  Q_OBJECT

public:
  PlotMatrixConfigurationWidget(PlotMatrixWidget * plotMatrix);

public slots:
  void updateLineEdits();
  void updateTitle();
  void exportPlot();

private:
  PlotMatrixWidget * plotMatrix_;
  QLineEdit * titleLineEdit_;
};
}
#endif