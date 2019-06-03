#ifndef __PV_VIEW_WIDGET_HXX__
#define __PV_VIEW_WIDGET_HXX__

#include "persalys/PersalysPrivate.hxx"

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

namespace PERSALYS
{
class PVServerManagerInterface;
class PERSALYS_API PVViewWidget : public QWidget
{
  Q_OBJECT
public:
  PVViewWidget(QWidget *parent, PVServerManagerInterface *smb, const char *viewType);
  ~PVViewWidget();

  virtual void setData(const std::vector< std::vector<double> >& valuesByColumn, const std::vector<std::string>& columnNames);
  void setData(const OT::Sample& sample);
  vtkTable * getTable(const OT::UnsignedInteger repr_ind = 0) const;
  vtkSMProxy * getProxy(const OT::UnsignedInteger repr_ind = 0) const;
  void setAxisToShow(const std::vector<std::string>& axis);
  void setAxisToShow(const OT::Description& sampleDescription);
  virtual const char *getRepresentationName() const = 0;
  void updateTable(const OT::Sample& sample, const OT::UnsignedInteger repr_ind = 0);
  pqOutputPort *getPort(const OT::UnsignedInteger ind = 0) const { return ports_[ind]; };

public slots:
  void setAxisToShow(const QStringList& variablesNames);
  void exportPlot();
signals:
  void plotChanged();

protected:
  QMainWindow *findMWInHierachy();
  bool eventFilter(QObject *obj, QEvent *event);
  pqView *getView() const;

  void buildTableFrom(const std::vector< std::vector<double> >& valuesByColumn, const std::vector<std::string>& columnNames);

protected:
  PVServerManagerInterface * smb_;
  OT::Pointer<pqView> view_;
  QList< pqOutputPort * > ports_;
  QList< vtkSmartPointer<vtkTable> > tables_;
  QList< vtkSmartPointer<vtkSMProxy> > producerBases_;
public:
  static const char SERIES_VISIBILITY_PROP[];
};
}
#endif
