#ifndef __PV_XYCHARTVIEW_WIDGET_HXX__
#define __PV_XYCHARTVIEW_WIDGET_HXX__

#include "PVViewWidget.hxx"

#include <vtkChartXY.h>

#include <QSet>

namespace PERSALYS
{
class PERSALYS_API PVXYChartViewWidget : public PVViewWidget
{
  Q_OBJECT

public:
  enum Type {Scatter, Trajectories, BagChart, FunctionalBagChart};

  PVXYChartViewWidget(QWidget *parent, PVServerManagerInterface *smb, const Type type = Scatter);
  ~PVXYChartViewWidget();
  virtual void setData(const std::vector< std::vector<double> >& valuesByColumn, const std::vector<std::string>& columnNames);
  void setData(const OT::Sample& sample, const QColor color);
  virtual const char *getRepresentationName() const
  {
    return PV_REPRESENTATION_TYPE[type_];
  }
  void setXAxisData(const QString& varX);
  double getXAxisRangeMinimum();
  double getXAxisRangeMaximum();
  double getYAxisRangeMinimum();
  double getYAxisRangeMaximum();
  void setXAxisRange(const double minValue, const double maxValue);
  void setYAxisRange(const double minValue, const double maxValue);
  QString getChartTitle(const QString& varX, const QString& varY);
  QString getXAxisTitle(const QString& varX, const QString& varY);
  QString getYAxisTitle(const QString& varX, const QString& varY);
  void setChartTitle(const QString& varX, const QString& varY, const QString& title);
  void setXAxisTitle(const QString& varX, const QString& varY, const QString& title);
  void setYAxisTitle(const QString& varX, const QString& varY, const QString& title);
  void setAxisTitles(const QStringList& variablesNames, const QStringList& titles);
  bool logScalingValidForXAxis();
  bool logScalingValidForYAxis();
  void setRepresentationColor(const QColor& color, const int reprIndex = 0);
  QColor getRepresentationColor(const int reprIndex = 0) const;
  void setRepresentationLabels(const QStringList& labels, const int reprIndex = 0);
  QStringList getRepresentationLabels(const int reprIndex = 0) const;
  void setMarkerStyle(const int markerStyle);
  int getMarkerStyle() const;
  void setShowLegend(const bool show);
  int getNumberOfRepresentations() const;
  void setRepresentationVisibility(const bool visibility, const int reprIndex = 0);
  bool getRepresentationVisibility(const int reprIndex = 0);

protected:
  void setPlotStyle();

public slots:
  virtual void showChart(const QString& varX, const QString& varY);
  virtual void setXLogScale(const bool scale);
  virtual void setYLogScale(const bool scale);
  void setMarkerSize(const int markerSize);
  void setRepresentationVisibility(const QList<int>& indices);
signals:
  void reprVisibilityChanged(const QList<int>& visibleRepr);
  void selectedReprChanged(const QStringList& visibleRepr);

public:
  static const QMap<Type, const char*> PV_VIEW_TYPE;
  static const QMap<Type, const char*> PV_REPRESENTATION_TYPE;
  static const QColor DEFAULT_SCATTER_PLOT_COLOR;

protected:
  vtkChartXY * chartXY_;
  QMap<QPair<QString, QString>, QStringList > chartsTitle_;
private:
  Type type_;
  QList<QColor> reprColors_;
  QSet<int> visibleRepr_;
};
}
#endif
