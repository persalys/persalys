#include "otgui/PVParCooViewWidget.hxx"

#include "vtkChart.h"
#include "pqContextView.h"
#include "vtkSMContextViewProxy.h"
#include "vtkContextMouseEvent.h"
#include "vtkContextScene.h"

const char PVParCooViewWidget::PV_VIEW_TYPE[]="ParallelCoordinatesChartView";

const char PVParCooViewWidget::PV_REPRESENTATION_TYPE[]="ParallelCoordinatesRepresentation";

PVParCooViewWidget::PVParCooViewWidget(QWidget *parent, PVServerManagerInterface *smb):PVViewWidget(parent,smb,PV_VIEW_TYPE)
{
  int selectionType(vtkChart::SELECT_RECTANGLE);
  int selectionModifier(vtkContextScene::SELECTION_DEFAULT);
  pqContextView *viewC(dynamic_cast<pqContextView *>(getView()));
  if(!viewC)
    return ;
  vtkAbstractContextItem* contextItem(viewC->getContextViewProxy()->GetContextItem());
  vtkChart* chart(vtkChart::SafeDownCast(contextItem));
  chart->SetActionToButton(selectionType, vtkContextMouseEvent::LEFT_BUTTON);
  chart->SetSelectionMode(selectionModifier);
}

PVParCooViewWidget::~PVParCooViewWidget()
{
}
