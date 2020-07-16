#include "persalys/PVBagChartViewWidget.hxx"

#include "persalys/PVServerManagerInterface.hxx"

#include <pqApplicationCore.h>
#include <pqObjectBuilder.h>
#include <vtkSMSourceProxy.h>
#include <vtkPVTrivialProducer.h>
#include <vtkContextScene.h>
#include <vtkSMPropertyHelper.h>

using namespace OT;

namespace PERSALYS
{

PVBagChartViewWidget::PVBagChartViewWidget(QWidget *parent, PVServerManagerInterface *smb, pqPipelineSource *filtersource)
  : PVXYChartViewWidget(parent, smb, filtersource == 0 ? BagChart : FunctionalBagChart)
  , filtersource_(filtersource)
{
  if (filtersource)
    createRepresentation();
}


PVBagChartViewWidget::~PVBagChartViewWidget()
{
}


pqPipelineSource * PVBagChartViewWidget::getFilterSource()
{
  return filtersource_;
}


void PVBagChartViewWidget::setData(const std::vector< std::vector<double> >& valuesByColumn, const std::vector<std::string>& columnNames)
{
  buildTableFrom(valuesByColumn, columnNames);

  pqObjectBuilder * builder(pqApplicationCore::instance()->getObjectBuilder());
  pqServer * serv(smb_->fetchServer());
  // create a new source
  pqActiveObjects::instance().setActiveView(getView());
  pqPipelineSource * mySourceProducer(builder->createSource("sources", "PVTrivialProducer", serv));
  vtkSMSourceProxy * producer(vtkSMSourceProxy::SafeDownCast(mySourceProducer->getProxy()));

  vtkObjectBase * clientSideObject(producer->GetClientSideObject());
  vtkPVTrivialProducer * realProducer(vtkPVTrivialProducer::SafeDownCast(clientSideObject));
  realProducer->SetOutput(tables_[tables_.size() - 1]);
  mySourceProducer->updatePipeline();

  // create a filter
  QMap<QString, QList<pqOutputPort*> > namedInputs;
  QList<pqOutputPort*> inputs;
  inputs.push_back(mySourceProducer->getOutputPort(0));
  namedInputs["Input"] = inputs;
  filtersource_ = builder->createFilter("filters", "ExtractBagPlots", namedInputs, serv);

  createRepresentation();
}


void PVBagChartViewWidget::createRepresentation()
{
  producerBases_.append(filtersource_->getProxy());

  vtkSMSourceProxy::SafeDownCast(filtersource_->getProxy())->UpdatePipeline();

  // create a new representation
  pqObjectBuilder * builder(pqApplicationCore::instance()->getObjectBuilder());
  pqDataRepresentation * newRepr(builder->createDataRepresentation(filtersource_->getOutputPort(0), getView(), getRepresentationName()));
  newRepr->setVisible(1);

  // update view
  getView()->resetDisplay();
  getView()->render();

  // ChartTitleBold : yes
  vtkSMProperty * idvpTitleBold(getView()->getProxy()->GetProperty("ChartTitleBold"));
  vtkSMPropertyHelper(idvpTitleBold).Set(true);
  getView()->getProxy()->UpdateProperty("ChartTitleBold");
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
