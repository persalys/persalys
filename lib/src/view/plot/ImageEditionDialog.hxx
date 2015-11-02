// ImageEditionDialog.hxx

#ifndef IMAGEEDITIONDIALOG_HXX
#define IMAGEEDITIONDIALOG_HXX

#include "OtguiPlotWidget.hxx"
#include <QDialog>
#include <QLineEdit>

namespace OTGUI {

class ImageEditionDialog : public QDialog
{
  Q_OBJECT

public:
  ImageEditionDialog(OtguiPlotWidget* plotWidget);

  void updateLineEdits();
  void setInitParameters();
  void resetParameters();

public slots:
  void updateTitle();
  void updateXLabel();
  void updateYLabel();
  void updateXrange();
  void updateYrange();
  void exportPlot();

private:
  OtguiPlotWidget * plotWidget_;
  QLineEdit * titleLineEdit_;
  QLineEdit * xlabelLineEdit_;
  QLineEdit * xmin_;
  QLineEdit * xmax_;
  QLineEdit * ylabelLineEdit_;
  QLineEdit * ymin_;
  QLineEdit * ymax_;
  QString initTitle_;
  QString initXlabel_;
  double initXmin_;
  double initXmax_;
  QString initYlabel_;
  double initYmin_;
  double initYmax_;
};
}
#endif