#include "persalys/PVBagChartViewWidget.hxx"

#include "persalys/PVServerManagerInterface.hxx"

#include <pqApplicationCore.h>
#include <pqObjectBuilder.h>
#include <vtkSMSourceProxy.h>
#include <vtkPVTrivialProducer.h>
#include <vtkContextScene.h>
#include <vtkSMPropertyHelper.h>
#include <vtkDoubleArray.h>

using namespace OT;

namespace PERSALYS
{

PVBagChartViewWidget::PVBagChartViewWidget(QWidget *parent, PVServerManagerInterface *smb, pqPipelineSource *filtersource)
  : PVXYChartViewWidget(parent, smb, (!filtersource ? FunctionalBagChart : BagChart))
  , filtersource_(filtersource)
{
  if (filtersource) {
    producerBases_.append(filtersource_->getProxy());
    vtkSMSourceProxy::SafeDownCast(filtersource_->getProxy())->UpdatePipeline();

    // create a new representation
    pqObjectBuilder * builder(pqApplicationCore::instance()->getObjectBuilder());
    pqDataRepresentation * newRepr(builder->createDataRepresentation(filtersource_->getOutputPort(0), getView(), getRepresentationName()));
    newRepr->setVisible(1);

    // update view
    getView()->resetDisplay();
    getView()->render();
  }
}

PVBagChartViewWidget::~PVBagChartViewWidget()
{
}


pqPipelineSource * PVBagChartViewWidget::getFilterSource()
{
  return filtersource_;
}


void PVBagChartViewWidget::setData(const std::vector<std::vector<double> >& valuesByColumn,
                                   const std::vector<std::string>& columnNames)
{
  if (filtersource_) {
    return;
  }
  buildTableFrom(valuesByColumn, columnNames);

  pqObjectBuilder* builder(pqApplicationCore::instance()->getObjectBuilder());
  pqServer* serv(smb_->fetchServer());
  // create a new source
  pqActiveObjects::instance().setActiveView(getView());
  pqPipelineSource* mySourceProducer(builder->createSource("sources", "PVTrivialProducer", serv));

  vtkSMSourceProxy* producer(vtkSMSourceProxy::SafeDownCast(mySourceProducer->getProxy()));

  vtkObjectBase* clientSideObject(producer->GetClientSideObject());
  vtkPVTrivialProducer* realProducer(vtkPVTrivialProducer::SafeDownCast(clientSideObject));

  int nCols = tables_[tables_.size() - 1]->GetNumberOfColumns();
  realProducer->SetOutput(tables_[tables_.size() - 1]);
  mySourceProducer->updatePipeline();

  // create the ExtractBagPlots filter
  QMap<QString, QList<pqOutputPort*> > namedInputs;
  QList<pqOutputPort*> inputs;
  inputs.push_back(mySourceProducer->getOutputPort(0));
  namedInputs["Input"] = inputs;
  filtersource_ = builder->createFilter("filters", "ExtractBagPlots", namedInputs, serv);
  producerBases_.append(filtersource_->getProxy());
  vtkSMSourceProxy* sourceProxy = vtkSMSourceProxy::SafeDownCast(filtersource_->getProxy());

  // Set the column names to be processed - it excludes the last one (mesh)
  vtkSMPropertyHelper(sourceProxy->GetProperty("SelectArrays")).RemoveAllValues();
  vtkSMPropertyHelper(sourceProxy->GetProperty("SelectArrays"))
    .SetNumberOfElements(tables_[tables_.size() - 1]->GetNumberOfColumns()-1);
  for (int i = 0; i < nCols-1; i++) {
    vtkSMPropertyHelper(sourceProxy->GetProperty("SelectArrays"))
      .Set(i, tables_[tables_.size() - 1]->GetColumn(i)->GetName());
  }
  sourceProxy->UpdateProperty("SelectArrays");

  filtersource_->updatePipeline();

  // create a new representation
  pqDataRepresentation* newRepr(builder->createDataRepresentation(
    filtersource_->getOutputPort(0), getView(), getRepresentationName()));
  newRepr->setVisible(1);

  ////////////////////////////////////////
  // Use our user column for the X-axis of the functional bag plot representation
  auto* reprProxy = newRepr->getProxy();
  vtkSMPropertyHelper(reprProxy->GetProperty("XArrayName")).Set(axisLabels_[tables_[tables_.size() - 1]->GetColumn(nCols-1)->GetName()].toUtf8().constData());
  vtkSMPropertyHelper(reprProxy->GetProperty("UseIndexForXAxis")).Set(false);
  reprProxy->UpdateProperty("UseIndexForXAxis");
  reprProxy->UpdateProperty("XArrayName");

  // ChartTitleBold : yes
  vtkSMProperty * idvpTitleBold(reprProxy->GetProperty("ChartTitleBold"));
  vtkSMPropertyHelper(idvpTitleBold).Set(true);
  reprProxy->UpdateProperty("ChartTitleBold");

  // update view
  getView()->resetDisplay();
  getView()->render();
}

int PVBagChartViewWidget::getUserQuantile() const
{
  return vtkSMPropertyHelper(getProxy()->GetProperty("UserQuantile")).GetAsInt();
}


void PVBagChartViewWidget::setUserQuantile(const int quantile)
{
  // set internal parameter to be able to update automatically the plot
  chartXY_->GetScene()->SetDirty(true);

  vtkSMPropertyHelper(getProxy()->GetProperty("UserQuantile")).Set(quantile);
  getProxy()->UpdateProperty("UserQuantile");
}
}
