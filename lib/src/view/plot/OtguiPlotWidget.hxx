// OtguiPlotWidget.hxx


#ifndef OTGUIPLOTWIDGET_HXX
#define OTGUIPLOTWIDGET_HXX

#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <QLabel>

namespace OTGUI {

class ImageEditionDialog;

class OtguiPlotWidget : public QwtPlot
{
  Q_OBJECT

public:
  OtguiPlotWidget(QWidget * parent = 0);

  /// clear plot
  void clear();
  void replot();
  void updatePlotLabel();
  QLabel * getPlotLabel() const;

public slots:
  void contextMenu(const QPoint & pos);
  void editImage();

private:
  QLabel * plotLabel_;
  ImageEditionDialog * dialog_;
  QwtPlotGrid * grid_;
  QAction * copyImageAction_;
};
}
#endif
