#include "otgui/PVMatrixPlotViewWidget.hxx"

namespace OTGUI {
const char PVMatrixPlotViewWidget::PV_VIEW_TYPE[] = "PlotMatrixView";

const char PVMatrixPlotViewWidget::PV_REPRESENTATION_TYPE[] = "PlotMatrixRepresentation";

PVMatrixPlotViewWidget::PVMatrixPlotViewWidget(QWidget *parent, PVServerManagerInterface *smb)
  : PVViewWidget(parent, smb, PV_VIEW_TYPE)
{
}


PVMatrixPlotViewWidget::~PVMatrixPlotViewWidget()
{
}
}
