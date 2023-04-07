#ifndef __PV_SPREADSHEETVIEW_WIDGET_HXX__
#define __PV_SPREADSHEETVIEW_WIDGET_HXX__

#include "PVViewWidget.hxx"
#include "persalys/Item.hxx"

namespace PERSALYS
{
class PERSALYS_PLOTPV_API PVSpreadSheetViewWidget : public PVViewWidget
{
  Q_OBJECT
public:
  PVSpreadSheetViewWidget(QWidget *parent, PVServerManagerInterface *smb);
  ~PVSpreadSheetViewWidget();
  virtual const char *getRepresentationName() const
  {
    return PV_REPRESENTATION_TYPE;
  }
  static QWidget * GetSpreadSheetViewWidget(PVSpreadSheetViewWidget *pvWidget, const OT::Sample &sample, Item *item = 0);
signals:
  void exportDataRequested();
public slots:
  void contextMenu(const QPoint & pos);
public:
  static const char PV_VIEW_TYPE[];
  static const char PV_REPRESENTATION_TYPE[];
};
}
#endif
