#include "otgui/PVViewWidget.hxx"
#include "otgui/PVServerManagerInterface.hxx"
#include "otgui/MyApplicationException.hxx"

#include "pqApplicationCore.h"
#include "pqObjectBuilder.h"
#include "pqPipelineSource.h"
#include "pqActiveObjects.h"
#include "pqView.h"
#include "pqSMAdaptor.h"

#include "vtkSMSourceProxy.h"
#include "vtkPVTrivialProducer.h"

#include "vtkDoubleArray.h"

#include <QMainWindow>
#include <QVBoxLayout>

#include <sstream>

PVViewWidget::PVViewWidget(QWidget *parent, PVServerManagerInterface *smb, const char *viewType):QWidget(parent),_main_win(0),_smb(smb),_view(0)
{
  QMainWindow *mw(findMWInHierachy());
  if(!mw)
    throw MyApplicationException("Internal error : No main window found !");
  if(!_smb)
    throw MyApplicationException("No pvserver factory found !");
  _main_win=mw;
  pqObjectBuilder *builder(pqApplicationCore::instance()->getObjectBuilder());
  pqServer *serv(_smb->fetchServer());
  _view=builder->createView(QString(viewType),serv,true);// PlotMatrixView ParallelCoordinatesChartView
  connect(_view,SIGNAL(selected(pqOutputPort *)),this,SLOT(mySlot(pqOutputPort *)));
  QWidget *pvWid(_view->widget());
  QVBoxLayout *lay(new QVBoxLayout(this));
  lay->addWidget(pvWid);
}

void PVViewWidget::mySlot(pqOutputPort *op)
{
  vtkSMSourceProxy *obj(op->getSelectionInput());
  std::cerr << "Selection occurs : " << std::endl;
  vtkSMProperty* idvp( obj->GetProperty("IDs"));
  QList<QVariant> value = pqSMAdaptor::getMultipleElementProperty(idvp);
  for (int cc = 0; cc < value.size(); cc++)
    {
      if ((cc % 2) == 1)
        {
          QString columnValues(value[cc].toString());
          std::cerr << columnValues.toStdString()  << " ";
        }
    }
  std::cerr << std::endl;
}

PVViewWidget::~PVViewWidget()
{
}

void PVViewWidget::setData(const std::vector< std::vector<double> >& valuesByColumn, const std::vector<std::string>& columnNames)
{
  pqObjectBuilder *builder(pqApplicationCore::instance()->getObjectBuilder());
  pqServer *serv(_smb->fetchServer());
  //
  buildTableFrom(valuesByColumn,columnNames);
  //
  pqActiveObjects::instance().setActiveView(_view);
  pqPipelineSource *mySourceProducer(builder->createSource("sources","PVTrivialProducer",serv));
  _producerBase=mySourceProducer->getProxy();
  vtkSMSourceProxy *producer(vtkSMSourceProxy::SafeDownCast(_producerBase));
  vtkObjectBase *clientSideObject(producer->GetClientSideObject());
  vtkPVTrivialProducer *realProducer(vtkPVTrivialProducer::SafeDownCast(clientSideObject));
  realProducer->SetOutput(_table);
  //
  pqDataRepresentation* myRepr(builder->createDataRepresentation(mySourceProducer->getOutputPort(0),_view,getRepresentationName()));
  myRepr->setVisible(1);
  _view->resetDisplay();
  _view->render();
}

QMainWindow *PVViewWidget::findMWInHierachy()
{
  QObject *curPar(parent());
  while(curPar)
    {
      QMainWindow *isMW(qobject_cast<QMainWindow *>(curPar));
      if(isMW)
        return isMW;
      curPar=curPar->parent();
    }
  return 0;
}

void PVViewWidget::buildTableFrom(const std::vector< std::vector<double> >& valuesByColumn, const std::vector<std::string>& columnNames)
{
  _table.TakeReference(vtkTable::New());
  std::size_t sz(valuesByColumn.size());
  if(sz!=columnNames.size())
    throw MyApplicationException("Sizes of vectors mismatches !");
  if(sz==0)
    return;
  std::size_t nbSamples(valuesByColumn[0].size());
  for(int i=0;i<sz;i++)
    {
      vtkSmartPointer<vtkDoubleArray> arr(vtkSmartPointer<vtkDoubleArray>::New());
      arr->SetName(columnNames[i].c_str());
      if(nbSamples!=valuesByColumn[i].size())
        {
          std::ostringstream oss;
          oss << "Sizes of vectors " << i << " mismatches with size of others "  << nbSamples << " !";
          throw MyApplicationException(oss.str());
        }
      arr->SetNumberOfTuples(nbSamples);
      arr->SetNumberOfComponents(1);
      double *pt(arr->GetPointer(0));
      std::copy(valuesByColumn[i].begin(),valuesByColumn[i].end(),pt);
      _table->AddColumn(arr);
    }
}
