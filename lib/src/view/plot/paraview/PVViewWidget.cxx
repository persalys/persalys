#include "otgui/PVViewWidget.hxx"

#include "otgui/OTguiException.hxx"
#include "otgui/PVServerManagerInterface.hxx"

#include <pqApplicationCore.h>
#include <pqObjectBuilder.h>
#include <pqPipelineSource.h>
#include <pqActiveObjects.h>
#include <pqView.h>
#include <pqSMAdaptor.h>
#include <pqSaveScreenshotReaction.h>

#include <vtkSMSourceProxy.h>
#include <vtkPVTrivialProducer.h>
#include <vtkSMSaveScreenshotProxy.h>
#include <vtkDoubleArray.h>
#include <vtkSMPropertyHelper.h>
#include <vtkSMProperty.h>

#include <pqContextView.h>
#include <vtkSMContextViewProxy.h>
#include <vtkAbstractContextItem.h>
#include <vtkContextScene.h>
#include <vtkCommand.h>

#include <QVBoxLayout>
#include <QMenu>
#include <QDebug>
#include <QEvent>

using namespace OT;

namespace OTGUI {

const char PVViewWidget::SERIES_VISIBILITY_PROP[] = "SeriesVisibility";

PVViewWidget::PVViewWidget(QWidget *parent, PVServerManagerInterface *smb, const char *viewType)
  : QWidget(parent)
  , smb_(smb)
  , view_(0)
  , contextItem_(0)
  , tables_()
  , producerBases_()
{
  QMainWindow * mw(findMWInHierachy());
  if(!mw)
    throw InvalidArgumentException(HERE) << "Internal error : No main window found !";
  if (!smb_)
    throw InvalidArgumentException(HERE) << "PVViewWidget: No pvserver factory found !";

  // create view
  pqObjectBuilder * builder(pqApplicationCore::instance()->getObjectBuilder());
  pqServer * serv(smb_->fetchServer());
  view_ = builder->createView(QString(viewType), serv, true);

  this->installEventFilter(this);
  QWidget * pvWid(view_->widget());
  pvWid->installEventFilter(this);
  QVBoxLayout * lay = new QVBoxLayout(this);
  lay->addWidget(pvWid);

  pqContextView * viewC(dynamic_cast<pqContextView *>(view_));
  if (viewC)
    contextItem_ = viewC->getContextViewProxy()->GetContextItem();
}


PVViewWidget::~PVViewWidget()
{
}


vtkTable * PVViewWidget::getTable(const UnsignedInteger repr_ind) const
{
  Q_ASSERT(repr_ind <= tables_.size());
  return tables_[repr_ind];
}


vtkSMProxy * PVViewWidget::getProxy(const UnsignedInteger repr_ind) const
{
  Q_ASSERT(repr_ind <= producerBases_.size());
  return producerBases_[repr_ind];
}


QMainWindow * PVViewWidget::findMWInHierachy()
{
  QObject * curPar(parent());
  while (curPar)
  {
    QMainWindow * isMW(qobject_cast<QMainWindow *>(curPar));
    if(isMW)
      return isMW;
    curPar = curPar->parent();
  }
  return 0;
}


bool PVViewWidget::eventFilter(QObject *obj, QEvent *event)
{
  if (obj == getView()->widget() && event->type() == QEvent::Paint)
  {
    emit plotChanged();

    // set active view : ugly to do this here...
    // avoid the problem of synchronization of the selection between the bound widgets
    QWidget * wdg = qobject_cast<QWidget*>(obj);
    if (wdg && this->isAncestorOf(wdg))
    {
      if (pqActiveObjects::instance().activeView() != view_)
      {
        pqActiveObjects::instance().setActiveView(view_);
      }
    }
  }
  return QObject::eventFilter(obj, event);
}


void PVViewWidget::exportPlot()
{
  pqActiveObjects::instance().setActiveView(view_);
  pqSaveScreenshotReaction::saveScreenshot();
}


void PVViewWidget::setData(const std::vector< std::vector<double> >& valuesByColumn, const std::vector<std::string>& columnNames)
{
  pqObjectBuilder * builder(pqApplicationCore::instance()->getObjectBuilder());
  pqServer * serv(smb_->fetchServer());
  // create a new table
  buildTableFrom(valuesByColumn, columnNames);
  // create a new source
  pqActiveObjects::instance().setActiveView(view_);
  pqPipelineSource * mySourceProducer(builder->createSource("sources", "PVTrivialProducer", serv));
  producerBases_.append(mySourceProducer->getProxy());
  vtkSMSourceProxy * producer(vtkSMSourceProxy::SafeDownCast(mySourceProducer->getProxy()));
  vtkObjectBase * clientSideObject(producer->GetClientSideObject());
  vtkPVTrivialProducer * realProducer(vtkPVTrivialProducer::SafeDownCast(clientSideObject));
  realProducer->SetOutput(tables_[tables_.size() - 1]);
  mySourceProducer->updatePipeline();
  // create a new representation
  pqDataRepresentation * newRepr(builder->createDataRepresentation(mySourceProducer->getOutputPort(0), view_, getRepresentationName()));
  newRepr->setVisible(1);
  // update view
  view_->resetDisplay();
  view_->render();
}


void PVViewWidget::setData(const Sample& sample)
{
  // values of each variable
  std::vector< std::vector<double> > varData(sample.getDimension(), std::vector<double>(sample.getSize()));
  for (UnsignedInteger i = 0; i < sample.getSize(); ++i)
    for (UnsignedInteger j = 0; j < sample.getDimension(); ++j)
      varData[j][i] = sample[i][j];

  // name of each variable
  Description desc(sample.getDescription());
  std::vector<std::string> varNames(sample.getDimension());
  std::copy(desc.begin(), desc.end(), varNames.begin());

  // build table
  setData(varData, varNames);
}


void PVViewWidget::buildTableFrom(const std::vector< std::vector<double> >& valuesByColumn,
                                        const std::vector<std::string>& columnNames)
{
  vtkSmartPointer<vtkTable> table;
  table.TakeReference(vtkTable::New());
  tables_.append(table);

  const std::size_t sz(valuesByColumn.size());

  // check size
  if (sz != columnNames.size())
    throw InvalidArgumentException(HERE) << "PVViewWidget::buildTableFrom: Sizes of vectors mismatches !";

  if (sz == 0)
    return;

  // set table_
  const std::size_t nbSamples(valuesByColumn[0].size());
  for (int i = 0; i < sz; i++)
  {
    vtkSmartPointer<vtkDoubleArray> arr(vtkSmartPointer<vtkDoubleArray>::New());
    arr->SetName(columnNames[i].c_str());

    if (nbSamples != valuesByColumn[i].size())
    {
      OSS oss;
      oss << "PVViewWidget::buildTableFrom: Sizes of vectors " << i << " mismatches with size of others "  << nbSamples << " !";
      throw InvalidArgumentException(HERE) << oss.str();
    }
    arr->SetNumberOfTuples(nbSamples);
    arr->SetNumberOfComponents(1);
    double * pt(arr->GetPointer(0));
    std::copy(valuesByColumn[i].begin(), valuesByColumn[i].end(), pt);
    table->AddColumn(arr);
  }
}


void PVViewWidget::setAxisToShow(const std::vector<std::string>& axis)
{
  // Mark the scene as dirty
  if (contextItem_)
    contextItem_->GetScene()->SetDirty(true);

  for (int repr_ind = 0; repr_ind < view_->getNumberOfRepresentations(); repr_ind++)
  {
    // get property
    vtkSMProperty* idvp(view_->getRepresentation(repr_ind)->getProxy()->GetProperty(SERIES_VISIBILITY_PROP));
    QList<QVariant> value = pqSMAdaptor::getMultipleElementProperty(idvp);

    // get columns names
    std::vector<std::string> sts;
    for (int cc = 0; cc < value.size(); cc++)
    {
      if ((cc % 2) == 0)
      {
        QString columnValues(value[cc].toString());
        sts.push_back(columnValues.toStdString());
      }
    }

    // re order/ keep some columns
    vtkSMPropertyHelper * smph(new vtkSMPropertyHelper(idvp));
    std::size_t ii(0);
    for (std::vector<std::string>::const_iterator it = axis.begin(); it != axis.end(); it++, ii++)
    {
      std::vector<std::string>::iterator it2(std::find(sts.begin(), sts.end(), *it));
      if (it2 == sts.end())
      {
        std::cerr << "Oooops " << *it << " not in ";
        std::copy(sts.begin(), sts.end(), std::ostream_iterator<std::string>(std::cerr, " "));
        std::cerr << std::endl;
        delete smph;
        return ;
      }
      smph->Set(2 * ii, (*it).c_str());
      smph->Set(2 * ii + 1, 1);
    }
    for (std::vector<std::string>::const_iterator it = sts.begin(); it != sts.end() && ii < sts.size(); it++)
    {
      std::vector<std::string>::const_iterator it2(std::find(axis.begin(), axis.end(), *it));
      if (it2 != axis.end())
        continue;
      smph->Set(2 * ii, (*it).c_str());
      smph->Set(2 * ii + 1, 0);
      ii++;
    }

    // update property
    view_->getRepresentation(repr_ind)->getProxy()->UpdateProperty("SeriesVisibility");
    delete smph;
  }
}


void PVViewWidget::setAxisToShow(const Description& sampleDescription)
{
  std::vector<std::string> varNames(sampleDescription.getSize());
  std::copy(sampleDescription.begin(), sampleDescription.end(), varNames.begin());

  setAxisToShow(varNames);
}


void PVViewWidget::setAxisToShow(const QStringList& variablesNames)
{
  std::vector<std::string> varNames(variablesNames.size());
  for (int i = 0; i < variablesNames.size(); ++i)
  {
    varNames[i] = variablesNames[i].toStdString();
  }

  setAxisToShow(varNames);
}


void PVViewWidget::updateTable(const Sample& sample, const UnsignedInteger repr_ind)
{
  // values of each variable
  std::vector< std::vector<double> > varData(sample.getDimension(), std::vector<double>(sample.getSize()));
  for (UnsignedInteger i = 0; i < sample.getSize(); ++i)
    for (UnsignedInteger j = 0; j < sample.getDimension(); ++j)
      varData[j][i] = sample[i][j];

  vtkIdType nbCols(getTable(repr_ind)->GetNumberOfColumns());
  Q_ASSERT(sample.getDimension() == nbCols);

  for (vtkIdType i = 0; i < nbCols; i++)
  {
    vtkAbstractArray * col(getTable(repr_ind)->GetColumn(i));
    vtkDoubleArray * cold(vtkDoubleArray::SafeDownCast(col));
    double * pt(cold->GetPointer(0));
    std::copy(varData[i].begin(), varData[i].end(), pt);
    cold->SetTuple(2, pt);
  }

  // update view
  getProxy(repr_ind)->MarkModified(NULL);
  view_->resetDisplay();
}
}
