#ifndef __PV_MATRIXPLOTVIEW_WIDGET_HXX__
#define __PV_MATRIXPLOTVIEW_WIDGET_HXX__

#include "PVViewWidget.hxx"
#include "persalys/PersalysPrivate.hxx"

#include <vtkSmartPointer.h>

class vtkCommand;

namespace PERSALYS
{
class PERSALYS_PLOTPV_API PVMatrixPlotViewWidget : public PVViewWidget
{
  Q_OBJECT
public:
  PVMatrixPlotViewWidget(QWidget *parent, PVServerManagerInterface *smb);
  ~PVMatrixPlotViewWidget();
  const char *getRepresentationName() const override
  {
    return PV_REPRESENTATION_TYPE;
  }
  void updateTable(const OT::Sample& sample, const OT::UnsignedInteger repr_ind = 0) override;
public:
  static const char PV_VIEW_TYPE[];
  static const char PV_REPRESENTATION_TYPE[];

private:
  vtkSmartPointer<vtkCommand> linkObserver_;
};
}
#endif
