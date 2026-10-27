#include "persalys/PVMatrixPlotViewWidget.hxx"

#include <pqContextView.h>
#include <vtkSMContextViewProxy.h>
#include <vtkScatterPlotMatrix.h>
#include <vtkAnnotationLink.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkSmartPointer.h>
#include <vtkCommand.h>
#include <vtkAbstractContextItem.h>
#include <vtkInformation.h>
#include <vtkVector.h>

namespace
{
// The cells of a vtkScatterPlotMatrix register their plots under block index 0
// (see vtkScatterPlotMatrix::UpdateLayout), while selections delivered through
// ParaView carry the composite index of the block they were made on. Such
// block-scoped selections are silently dropped by the cells, so linked
// selections (e.g. from the scatter plot view) never show up in the matrix.
// The matrix always displays whole tables, hence block scoping is meaningless
// here: drop the composite index so row selections apply to every cell.
// The rewrite is a no-op when no node carries a composite index, which also
// prevents feedback loops.
class PVMatrixSelectionCommand : public vtkCommand
{
public:
  static PVMatrixSelectionCommand* New() { return new PVMatrixSelectionCommand; }
  void Execute(vtkObject* caller, unsigned long, void*) override
  {
    vtkAnnotationLink* link = vtkAnnotationLink::SafeDownCast(caller);
    if (!link)
      return;
    vtkSelection* sel = link->GetCurrentSelection();
    if (!sel)
      return;
    bool hasComposite = false;
    for (unsigned int i = 0; i < sel->GetNumberOfNodes(); ++i)
    {
      vtkSelectionNode* node = sel->GetNode(i);
      if (node && node->GetProperties() &&
        node->GetProperties()->Has(vtkSelectionNode::COMPOSITE_INDEX()))
      {
        hasComposite = true;
        break;
      }
    }
    if (!hasComposite)
      return;
    vtkSmartPointer<vtkSelection> cleaned = vtkSmartPointer<vtkSelection>::New();
    for (unsigned int i = 0; i < sel->GetNumberOfNodes(); ++i)
    {
      vtkSelectionNode* node = sel->GetNode(i);
      if (!node)
        continue;
      vtkSmartPointer<vtkSelectionNode> newNode = vtkSmartPointer<vtkSelectionNode>::New();
      newNode->SetFieldType(node->GetFieldType());
      newNode->SetContentType(node->GetContentType());
      newNode->SetSelectionList(node->GetSelectionList());
      if (node->GetProperties())
      {
        newNode->GetProperties()->Copy(node->GetProperties());
        newNode->GetProperties()->Remove(vtkSelectionNode::COMPOSITE_INDEX());
      }
      cleaned->AddNode(newNode);
    }
    link->SetCurrentSelection(cleaned);
  }
};
}

namespace PERSALYS
{
const char PVMatrixPlotViewWidget::PV_VIEW_TYPE[] = "PlotMatrixView";

const char PVMatrixPlotViewWidget::PV_REPRESENTATION_TYPE[] = "PlotMatrixRepresentation";

PVMatrixPlotViewWidget::PVMatrixPlotViewWidget(QWidget *parent, PVServerManagerInterface *smb)
  : PVViewWidget(parent, smb, PV_VIEW_TYPE)
  , linkObserver_(nullptr)
{
  // observe the shared annotation link to un-scope block selections (see above)
  if (const pqContextView * viewC = dynamic_cast<const pqContextView *>(getView()))
  {
    if (vtkScatterPlotMatrix * matrix = vtkScatterPlotMatrix::SafeDownCast(
          viewC->getContextViewProxy()->GetContextItem()))
    {
      if (matrix->GetAnnotationLink())
      {
        linkObserver_ = vtkSmartPointer<PVMatrixSelectionCommand>::New();
        matrix->GetAnnotationLink()->AddObserver(
          vtkCommand::AnnotationChangedEvent, linkObserver_);
      }
    }
  }
}


PVMatrixPlotViewWidget::~PVMatrixPlotViewWidget()
{
  if (linkObserver_)
  {
    if (const pqContextView * viewC = dynamic_cast<const pqContextView *>(getView()))
    {
      if (vtkScatterPlotMatrix* matrix = vtkScatterPlotMatrix::SafeDownCast(
            viewC->getContextViewProxy()->GetContextItem()))
      {
        if (matrix->GetAnnotationLink())
          matrix->GetAnnotationLink()->RemoveObserver(linkObserver_);
      }
    }
  }
}


void PVMatrixPlotViewWidget::updateTable(const OT::Sample& sample, const OT::UnsignedInteger repr_ind)
{
  // Refreshing the table content (e.g. switching between values and ranks)
  // makes the representation push the table to the matrix again on the next
  // render, which resets the cell selected by the user (active plot) to the
  // default first cell. Flush that render now and restore the selected pair
  // afterwards so the selection survives the refresh.
  vtkVector2i activePlot(0, 0);
  bool hasActivePlot = false;
  if (const pqContextView * viewC = dynamic_cast<const pqContextView *>(getView()))
  {
    if (vtkScatterPlotMatrix * matrix = vtkScatterPlotMatrix::SafeDownCast(
          viewC->getContextViewProxy()->GetContextItem()))
    {
      activePlot = matrix->GetActivePlot();
      hasActivePlot = true;
    }
  }

  PVViewWidget::updateTable(sample, repr_ind);

  // updateTable only schedules a render: force it now so the matrix layout
  // rebuild (and the active plot reset) happens synchronously here
  getView()->forceRender();

  if (hasActivePlot)
  {
    if (const pqContextView * viewC = dynamic_cast<const pqContextView *>(getView()))
    {
      if (vtkScatterPlotMatrix * matrix = vtkScatterPlotMatrix::SafeDownCast(
            viewC->getContextViewProxy()->GetContextItem()))
      {
        const vtkVector2i currentPlot = matrix->GetActivePlot();
        if (currentPlot.GetX() != activePlot.GetX() || currentPlot.GetY() != activePlot.GetY())
        {
          matrix->SetActivePlot(activePlot);
          // repaint with the restored active plot
          getView()->forceRender();
        }
      }
    }
  }
}
}
