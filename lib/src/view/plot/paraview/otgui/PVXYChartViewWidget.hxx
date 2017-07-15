#ifndef __PV_XYCHARTVIEW_WIDGET_HXX__
#define __PV_XYCHARTVIEW_WIDGET_HXX__

#include "PVViewWidget.hxx"
#include "otgui/OTGuiprivate.hxx"

#include <vtkChartXY.h>

namespace OTGUI {
class OTGUI_API PVXYChartViewWidget : public PVViewWidget
{
  Q_OBJECT

public:
  PVXYChartViewWidget(QWidget *parent, PVServerManagerInterface *smb);
  ~PVXYChartViewWidget();
  virtual void setData(const std::vector< std::vector<double> >& valuesByColumn, const std::vector<std::string>& columnNames);
  void setData(const OT::Sample& sample, const QColor color);
  virtual const char *getRepresentationName() const { return PV_REPRESENTATION_TYPE; }
  double getXAxisRangeMinimum();
  double getXAxisRangeMaximum();
  double getYAxisRangeMinimum();
  double getYAxisRangeMaximum();
  void setXAxisRange(const double minValue, const double maxValue);
  void setYAxisRange(const double minValue, const double maxValue);
  QString getChartTitle(const QString& varX, const QString& varY);
  QString getXAxisTitle(const QString& varX, const QString& varY);
  QString getYAxisTitle(const QString& varX, const QString& varY);
  void setChartTitle(const QString& varX, const QString& varY);
  void setXAxisTitle(const QString& varX, const QString& varY);
  void setYAxisTitle(const QString& varX, const QString& varY);
  void setChartTitle(const QString& varX, const QString& varY, const QString& title);
  void setXAxisTitle(const QString& varX, const QString& varY, const QString& title);
  void setYAxisTitle(const QString& varX, const QString& varY, const QString& title);
  void setAxisTitles(const QStringList& variablesNames, const QStringList& titles);
  bool logScalingValidForXAxis();
  bool logScalingValidForYAxis();
  void setRepresentationColor(const QColor& color, const int reprIndex=0);
  QColor getRepresentationColor(const int reprIndex=0) const;
  void setRepresentationLabels(const QStringList& labels, const int reprIndex=0);
  QStringList getRepresentationLabels(const int reprIndex=0) const;
  void setMarkerStyle(const int markerStyle);
  void setMarkerSize(const int markerSize);
  void setShowLegend(const bool show);
  int getNumberOfRepresentations() const;
  void setRepresentationVisibility(const bool visibility, const int reprIndex=0);

protected:
  void setScatterPlotStyle();

public slots:
  virtual void showChart(const QString& varX, const QString& varY);
  virtual void setXLogScale(const bool scale);
  virtual void setYLogScale(const bool scale);

public:
  static const char PV_VIEW_TYPE[];
  static const char PV_REPRESENTATION_TYPE[];
  static const QColor DEFAULT_SCATTER_PLOT_COLOR;
private:
  vtkChartXY * chartXY_;
  QList<QColor> reprColors_;
  int markerStyle_;
  QMap<QPair<QString, QString>, QString> chartsTitle_;
  QMap<QPair<QString, QString>, QString> XAxisTitle_;
  QMap<QPair<QString, QString>, QString> YAxisTitle_;
};
}
#endif
