#ifndef __PV_SPREADSHEETVIEW_WIDGET_HXX__
#define __PV_SPREADSHEETVIEW_WIDGET_HXX__

#include "PVViewWidget.hxx"

namespace PERSALYS
{
class PERSALYS_API PVSpreadSheetViewWidget : public PVViewWidget
{
  Q_OBJECT
public:
  PVSpreadSheetViewWidget(QWidget *parent, PVServerManagerInterface *smb);
  ~PVSpreadSheetViewWidget();
  virtual const char *getRepresentationName() const
  {
    return PV_REPRESENTATION_TYPE;
  }
public slots:
  void contextMenu(const QPoint & pos);
  void exportData();
public:
  static const char PV_VIEW_TYPE[];
  static const char PV_REPRESENTATION_TYPE[];
};
}
#endif
