#ifndef __PV_BAGCHARTVIEW_WIDGET_HXX__
#define __PV_BAGCHARTVIEW_WIDGET_HXX__

#include "PVXYChartViewWidget.hxx"

class pqPipelineSource;

namespace PERSALYS
{
class PERSALYS_PLOTPV_API PVBagChartViewWidget : public PVXYChartViewWidget
{
  Q_OBJECT

public:
  PVBagChartViewWidget(QWidget *parent, PVServerManagerInterface *smb, pqPipelineSource* filtersource = 0);

  ~PVBagChartViewWidget();

  virtual void setData(const std::vector< std::vector<double> >& valuesByColumn, const std::vector<std::string>& columnNames);

  int getUserQuantile() const;
  void setUserQuantile(const int quantile);

  pqPipelineSource * getFilterSource();

private:
  pqPipelineSource* filtersource_;
};
}
#endif
