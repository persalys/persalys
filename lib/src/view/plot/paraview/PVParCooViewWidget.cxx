#include "otgui/PVParCooViewWidget.hxx"

#include <vtkChart.h>
#include <pqContextView.h>
#include <vtkSMContextViewProxy.h>
#include <vtkContextMouseEvent.h>
#include <vtkContextScene.h>

#include <vtkSMPropertyHelper.h>
namespace OTGUI {
const char PVParCooViewWidget::PV_VIEW_TYPE[] = "ParallelCoordinatesChartView";

const char PVParCooViewWidget::PV_REPRESENTATION_TYPE[] = "ParallelCoordinatesRepresentation";

PVParCooViewWidget::PVParCooViewWidget(QWidget *parent, PVServerManagerInterface *smb)
  : PVViewWidget(parent, smb , PV_VIEW_TYPE)
{
  pqContextView * viewC(dynamic_cast<pqContextView *>(getView()));
  if(!viewC)
    return ;

  vtkAbstractContextItem * contextItem(viewC->getContextViewProxy()->GetContextItem());
  vtkChart * chart(vtkChart::SafeDownCast(contextItem));

  int selectionType(vtkChart::SELECT_RECTANGLE);
  int selectionModifier(vtkContextScene::SELECTION_DEFAULT);
  chart->SetActionToButton(selectionType, vtkContextMouseEvent::LEFT_BUTTON);
  chart->SetSelectionMode(selectionModifier);
}


PVParCooViewWidget::~PVParCooViewWidget()
{
}
}
