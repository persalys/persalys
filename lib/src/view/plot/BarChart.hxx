// BarChart.hxx


#ifndef BARCHART_HXX
#define BARCHART_HXX

#include "OtguiPlotWidget.hxx"

#include <qwt_plot.h>
#include <qwt_scale_draw.h>

#include "Description.hxx"
#include "NumericalPoint.hxx"

namespace OTGUI {

class BarChartHorizontalScaleDraw: public QwtScaleDraw
{
public:
  BarChartHorizontalScaleDraw(const OT::Description & labels):labels_(labels)
  {
    enableComponent(QwtScaleDraw::Backbone, false);
    enableComponent(QwtScaleDraw::Ticks, false);
  }

  virtual QwtText label(double value) const
  {
    QwtText lbl;

    const int index = qRound(value);
    if ( index>=0 && index<=labels_.getSize())
      lbl = QwtText(labels_[index].c_str());
        
    return lbl;
  }

private:
    const OT::Description labels_;
};

class BarChart : public OtguiPlotWidget
{
  Q_OBJECT

public:
  BarChart(QWidget * parent = 0);

  void plotSensitivityIndices(const OT::NumericalPoint firstOrder, const OT::NumericalPoint totalOrder,
                              const OT::Description inputNames);
};
}
#endif

