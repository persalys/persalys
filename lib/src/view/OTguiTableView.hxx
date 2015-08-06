// OTguiTableView.hxx

#ifndef OTGUI_OTGUITABLEVIEW_H
#define OTGUI_OTGUITABLEVIEW_H

#include "OTguiTableModel.hxx"
#include <QTableView>

namespace OTGUI {
class OTguiTableView : public QTableView
{
  Q_OBJECT

public:
  OTguiTableView(OT::NumericalSample & sample);
  virtual ~OTguiTableView();

public slots:
  void contextMenu(const QPoint & pos);
  void exportData();

private:
  OTguiTableModel * model_;
};
}
#endif