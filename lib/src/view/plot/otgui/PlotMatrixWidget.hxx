// PlotMatrixWidget.hxx

#ifndef PLOTMATRIXWIDGET_H
#define PLOTMATRIXWIDGET_H

#include "NumericalSample.hxx"

#include <QTableWidget>
#include <QLabel>

namespace OTGUI {
class PlotMatrixWidget : public QWidget
{
  Q_OBJECT

public:
  PlotMatrixWidget(const OT::NumericalSample & inputSample, const OT::NumericalSample & outputSample, QWidget * parent = 0);

  QString getTitle() const;
  void setTitle(const QString & title);
  QStringList getInputNames() const;
  QStringList getOutputNames() const;
  QImage getMatrixImage();

public slots:
  void exportPlot();
  void setInputsToDisplay(QStringList inputs);
  void setOutputsToDisplay(QStringList outputs);

private:
  QTableWidget * tableWidget_;
  QList<QImage> matrixImages_;
  QImage matrixImage_;
  QStringList inputNames_;
  QStringList outputNames_;
  QLabel * matrixTitleLabel_;
  int nbInputsToDisplay_;
  int nbOutputsToDisplay_;
};
}
#endif