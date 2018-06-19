#ifndef __PV_BAGCHARTVIEW_WIDGET_HXX__
#define __PV_BAGCHARTVIEW_WIDGET_HXX__

#include "PVXYChartViewWidget.hxx"

class pqPipelineSource;

namespace OTGUI
{
class OTGUI_API PVBagChartViewWidget : public PVXYChartViewWidget
{
  Q_OBJECT

public:
  PVBagChartViewWidget(QWidget *parent, PVServerManagerInterface *smb, const Type type);
  PVBagChartViewWidget(QWidget *parent, PVServerManagerInterface *smb, const Type type, pqPipelineSource* filtersource);

  ~PVBagChartViewWidget();

  virtual void setData(const std::vector< std::vector<double> >& valuesByColumn, const std::vector<std::string>& columnNames);

  int getUserQuantile() const;
  void setUserQuantile(const int quantile);

  pqPipelineSource * getFilterSource();

protected:
  void createRepresentation();

private:
  pqPipelineSource* filtersource_;
};
}
#endif
