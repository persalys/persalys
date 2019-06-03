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

PVBagChartViewWidget::PVBagChartViewWidget(QWidget *parent, PVServerManagerInterface *smb, const Type type)
  : PVXYChartViewWidget(parent, smb, type)
  , filtersource_(0)
{
}


PVBagChartViewWidget::PVBagChartViewWidget(QWidget *parent, PVServerManagerInterface *smb, const Type type, pqPipelineSource *filtersource)
  : PVXYChartViewWidget(parent, smb, type)
  , filtersource_(filtersource)
{
  if (!filtersource)
    throw InvalidArgumentException(HERE) << "Internal error in PVBagChartViewWidget constructor: filter source null pointer !";

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
  vtkSMPropertyHelper * smphTitleBold(new vtkSMPropertyHelper(idvpTitleBold));
  smphTitleBold->Set(true);
  getView()->getProxy()->UpdateProperty("ChartTitleBold");
  delete smphTitleBold;
}


int PVBagChartViewWidget::getUserQuantile() const
{
  vtkSMProperty * idvp(getProxy()->GetProperty("UserQuantile"));
  vtkSMPropertyHelper * smph(new vtkSMPropertyHelper(idvp));
  int quantile = smph->GetAsInt();
  delete smph;
  return quantile;
}


void PVBagChartViewWidget::setUserQuantile(const int quantile)
{
  // set internal parameter to be able to update automatically the plot
  chartXY_->GetScene()->SetDirty(true);

  vtkSMProperty * idvp(getProxy()->GetProperty("UserQuantile"));
  vtkSMPropertyHelper * smph(new vtkSMPropertyHelper(idvp));
  smph->Set(quantile);
  filtersource_->getProxy()->UpdateProperty("UserQuantile");
  delete smph;
}
}
