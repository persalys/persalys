#ifndef __PV_VIEW_WIDGET_HXX__
#define __PV_VIEW_WIDGET_HXX__

#include "vtkSmartPointer.h"
#include "vtkTable.h"

#include <QWidget>

#include "otgui/OTGuiprivate.hxx"

#include <vector>
#include <string>

class QMainWindow;
class PVServerManagerInterface;
class pqView;
class pqOutputPort;
class vtkSMProxy;

class OTGUI_API PVViewWidget : public QWidget
{
  Q_OBJECT
public:
  PVViewWidget(QWidget *parent, PVServerManagerInterface *smb, const char *viewType);
  ~PVViewWidget();
  void setData(const std::vector< std::vector<double> >& valuesByColumn, const std::vector<std::string>& columnNames);
  vtkTable *getTable() const { return _table; }
  vtkSMProxy *getProxy() const { return _producerBase; }
  virtual const char *getRepresentationName() const = 0;
public slots:
  void mySlot(pqOutputPort *op);
protected:
  pqView *getView() const { return _view; }
private:
  QMainWindow *findMWInHierachy();
  void buildTableFrom(const std::vector< std::vector<double> >& valuesByColumn, const std::vector<std::string>& columnNames);
private:
  QMainWindow *_main_win;
  PVServerManagerInterface *_smb;
private:
  pqView *_view;
  vtkSmartPointer<vtkTable> _table;
  vtkSMProxy *_producerBase;
};

#endif
