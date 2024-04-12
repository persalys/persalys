#ifndef __PV_XYCHARTVIEW_WIDGET_HXX__
#define __PV_XYCHARTVIEW_WIDGET_HXX__

#include "PVViewWidget.hxx"

#include <vtkChartXY.h>
#include <vtkAxis.h>

#include <QSet>

namespace PERSALYS
{
class PERSALYS_PLOTPV_API PVXYChartViewWidget : public PVViewWidget
{
  Q_OBJECT

public:
  enum Type {Scatter, TrajectoriesPoints, Trajectories, BagChart, FunctionalBagChart};

  PVXYChartViewWidget(QWidget *parent, PVServerManagerInterface *smb, const Type type = Scatter);
  ~PVXYChartViewWidget();
  virtual void setData(const std::vector< std::vector<double> >& valuesByColumn, const std::vector<std::string>& columnNames) override;
  void setData(const OT::Sample& sample, const QColor color);
  const char *getRepresentationName() const override
  {
    return PV_REPRESENTATION_TYPE[type_];
  }
  void setXAxisData(const QString& varX);
  double getAxisRangeMinimum(const vtkAxis::Location ax);
  double getAxisRangeMaximum(const vtkAxis::Location ax);
  void setAxisRange(const vtkAxis::Location ax, const double minValue, const double maxValue);
  QString getAxisTitle(const vtkAxis::Location ax, const QString& varX, const QString& varY);
  void setAxisTitle(const vtkAxis::Location ax, const QString& title, const QString& varX = "", const QString& varY = "");

  QString getChartTitle(const QString& varX, const QString& varY);
  void setChartTitle(const QString& title, const QString& varX = "", const QString& varY = "");
  void setAxisTitles(const QStringList& variablesNames, const QStringList& titles);
  bool logScalingValidForAxis(const vtkAxis::Location ax);
  void setSerieColors(const QMap<QString, QColor>& colors);
  void setSerieLineStyles(const QMap<QString, int>& styles);
  void setRepresentationColor(const QColor& color, const int reprIndex = 0);
  QColor getRepresentationColor(const int reprIndex = 0) const;
  void setMarkerStyle(const int markerStyle);
  int getMarkerStyle() const;
  void setShowLegend(const bool show);

protected:
  void setPlotStyle();

public slots:
  virtual void showChart(const QString& varX, const QString& varY);
  virtual void setLogScale(const vtkAxis::Location ax, const bool scale);
  void setMarkerSize(const int markerSize);
signals:
  void axisHasBeenModified();

public:
  static const QMap<Type, const char*> PV_VIEW_TYPE;
  static const QMap<Type, const char*> PV_REPRESENTATION_TYPE;
  static const QColor DEFAULT_SCATTER_PLOT_COLOR;

protected:
  vtkChartXY * chartXY_;
  QMap<QPair<QString, QString>, QString > chartsTitle_;
  QMap<QString, QString> axisLabels_;
private:
  Type type_;
  QList<QColor> reprColors_;
};
}
#endif
