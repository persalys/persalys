#ifndef __PV_VIEW_WIDGET_HXX__
#define __PV_VIEW_WIDGET_HXX__

#include "otgui/OTGuiprivate.hxx"

#include <vtkSmartPointer.h>
#include <vtkTable.h>

#include <QMainWindow>
#include <QAction>

#include <openturns/Sample.hxx>

#include <vector>
#include <string>

class pqView;
class pqOutputPort;
class pqDataRepresentation;
class vtkSMProxy;
class vtkAbstractContextItem;

namespace OTGUI {
class PVServerManagerInterface;
class OTGUI_API PVViewWidget : public QWidget
{
  Q_OBJECT
public:
  PVViewWidget(QWidget *parent, PVServerManagerInterface *smb, const char *viewType);
  ~PVViewWidget();

  virtual void setData(const std::vector< std::vector<double> >& valuesByColumn, const std::vector<std::string>& columnNames);
  void setData(const OT::Sample& sample);
  vtkTable *getTable() const { return table_; }
  vtkSMProxy *getProxy(const int i=0) const { if (i >= producerBases_.size())return 0; return producerBases_[i]; }
  void setAxisToShow(const std::vector<std::string>& axis);
  void setAxisToShow(const OT::Description& sampleDescription);
  virtual const char *getRepresentationName() const = 0;

public slots:
  void setAxisToShow(const QStringList& variablesNames);
  void exportPlot();
signals:
  void plotChanged();

protected:
  QMainWindow *findMWInHierachy();
  bool eventFilter(QObject *obj, QEvent *event);
  pqView *getView() const { return view_; }

private:
  void buildTableFrom(const std::vector< std::vector<double> >& valuesByColumn, const std::vector<std::string>& columnNames);

private:
  PVServerManagerInterface * smb_;
  pqView * view_;
  vtkAbstractContextItem * contextItem_;
  vtkSmartPointer<vtkTable> table_;
  vtkSMProxy * producerBase_;
  QList< vtkSMProxy * > producerBases_;
public:
  static const char SERIES_VISIBILITY_PROP[];
};
}
#endif
