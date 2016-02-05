// GraphConfigurationWidget.hxx

#ifndef GRAPHCONFIGURATIONWIDGET_H
#define GRAPHCONFIGURATIONWIDGET_H

#include "PlotWidget.hxx"

#include <QLineEdit>
#include <QComboBox>
#include <QButtonGroup>

namespace OTGUI {

class GraphConfigurationWidget : public QWidget
{
  Q_OBJECT

public:
  enum Type {NoType, Scatter, PDF, CDF, PDFResult, CDFResult, BoxPlot, SensitivityIndices};

  GraphConfigurationWidget(QVector<PlotWidget *> plotWidgets, QStringList inputNames, QStringList outputNames, Type plotType);

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
  Type plotType_;
  int currentPlotIndex_;
  QButtonGroup * pdf_cdfGroup_;
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