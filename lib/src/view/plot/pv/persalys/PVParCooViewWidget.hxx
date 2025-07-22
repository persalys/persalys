#ifndef __PV_PARCOOVIEW_WIDGET_HXX__
#define __PV_PARCOOVIEW_WIDGET_HXX__

#include "PVViewWidget.hxx"
#include "persalys/PersalysPrivate.hxx"

namespace PERSALYS
{
class PERSALYS_PLOTPV_API PVParCooViewWidget : public PVViewWidget
{
  Q_OBJECT
public:
  PVParCooViewWidget(QWidget *parent, PVServerManagerInterface *smb);
  ~PVParCooViewWidget();
  const char *getRepresentationName() const override
  {
    return PV_REPRESENTATION_TYPE;
  }
public:
  static const char PV_VIEW_TYPE[];
  static const char PV_REPRESENTATION_TYPE[];
};
}
#endif
