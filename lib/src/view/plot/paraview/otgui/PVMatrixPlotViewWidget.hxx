#ifndef __PV_MATRIXPLOTVIEW_WIDGET_HXX__
#define __PV_MATRIXPLOTVIEW_WIDGET_HXX__

#include "PVViewWidget.hxx"
#include "otgui/OTGuiprivate.hxx"

class OTGUI_API PVMatrixPlotViewWidget : public PVViewWidget
{
  Q_OBJECT
public:
  PVMatrixPlotViewWidget(QWidget *parent, PVServerManagerInterface *smb);
  ~PVMatrixPlotViewWidget();
  virtual const char *getRepresentationName() const { return PV_REPRESENTATION_TYPE; }
public:
  static const char PV_VIEW_TYPE[];
  static const char PV_REPRESENTATION_TYPE[];
};

#endif
