//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of an otStudy
 *
 *  Copyright 2015-2018 EDF-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/OTStudyItem.hxx"

#include "otgui/FileTools.hxx"
#include "otgui/PhysicalModelAnalysis.hxx"
#include "otgui/DesignOfExperimentAnalysis.hxx"
#include "otgui/SymbolicPhysicalModel.hxx"
#include "otgui/PythonPhysicalModel.hxx"
#ifdef OTGUI_HAVE_YACS
#include "otgui/YACSPhysicalModel.hxx"
#endif
#ifdef OTGUI_HAVE_OTFMI
#include "otgui/FMIPhysicalModel.hxx"
#endif

#include <QDebug>
#include <QFileInfo>
#include <QApplication>

using namespace OT;

namespace OTGUI
{

OTStudyItem::OTStudyItem(const OTStudy& otStudy)
  : OTguiItem(QString::fromUtf8(otStudy.getName().c_str()), "OTStudy")
  , Observer("OTStudy")
  , otStudy_(otStudy)
{
  parentOTStudyItem_ = this;
  otStudy_.addObserver(this);
  update(0, "fileNameChanged");

  buildActions();
  connect(this, SIGNAL(statusChanged()), this, SLOT(updateIcon()));
}


void OTStudyItem::buildActions()
{
  // new model actions
  newSymbolicModel_ = new QAction(tr("Symbolic model"), this);
  connect(newSymbolicModel_, SIGNAL(triggered()), this, SLOT(createSymbolicModel()));

  newPythonModel_ = new QAction(tr("Python model"), this);
  connect(newPythonModel_, SIGNAL(triggered()), this, SLOT(createPythonModel()));

#ifdef OTGUI_HAVE_YACS
  newYACSModel_ = new QAction(tr("YACS model"), this);
  connect(newYACSModel_, SIGNAL(triggered()), this, SLOT(createYACSModel()));
#endif
#ifdef OTGUI_HAVE_OTFMI
  newFMIModel_ = new QAction(tr("FMI model"), this);
  connect(newFMIModel_, SIGNAL(triggered()), this, SLOT(createFMIModel()));
#endif

  newDataModel_ = new QAction(tr("Data model"), this);
  connect(newDataModel_, SIGNAL(triggered()), this, SLOT(createDataModel()));

  // export action
  exportAction_ = new QAction(QIcon(":/images/document-export.png"), tr("Export Python"), this);
  connect(exportAction_, SIGNAL(triggered()), this, SIGNAL(exportRequested()));

  // save  action
  saveAction_ = new QAction(QIcon(":/images/document-save.png"), tr("Save"), this);
  connect(saveAction_, SIGNAL(triggered()), this, SLOT(emitSave()));

  // save as  action
  saveAsAction_ = new QAction(QIcon(":/images/document-save-as.png"), tr("Save As..."), this);
  connect(saveAsAction_, SIGNAL(triggered()), this, SLOT(emitSaveAs()));

  // close action
  closeAction_ = new QAction(QIcon(":/images/window-close.png"), tr("Close"), this);
  connect(closeAction_, SIGNAL(triggered()), this, SLOT(emitClose()));

  // add actions
  appendSeparator(tr("Model"));
  appendAction(newSymbolicModel_);
  appendAction(newPythonModel_);
#ifdef OTGUI_HAVE_YACS
  appendAction(newYACSModel_);
#endif
#ifdef OTGUI_HAVE_OTFMI
  appendAction(newFMIModel_);
#endif
  appendAction(newDataModel_);
  appendSeparator();
  appendAction(exportAction_);
  appendAction(saveAction_);
  appendSeparator();
  appendAction(closeAction_);
}


void OTStudyItem::update(Observable * source, const String & message)
{
  if (message == "addDataModel")
  {
    DesignOfExperiment addedDataModel = otStudy_.getDataModels()[otStudy_.getDataModels().getSize() - 1];
    appendItem(addedDataModel);
  }
  else if (message == "addPhysicalModel")
  {
    PhysicalModel addedPhysicalModel = otStudy_.getPhysicalModels()[otStudy_.getPhysicalModels().getSize() - 1];
    appendItem(addedPhysicalModel);
  }
  else if (message == "addLimitState")
  {
    LimitState addedLimitState = otStudy_.getLimitStates()[otStudy_.getLimitStates().getSize() - 1];
    appendItem(addedLimitState);
  }
  else if (message == "addAnalysis")
  {
    Analysis addedAnalysis = otStudy_.getAnalyses()[otStudy_.getAnalyses().getSize() - 1];
    appendItem(addedAnalysis);
  }
  else if (message == "otStudyRemoved")
  {
    requestRemove();
  }
  else if (message == "fileNameChanged")
  {
    if (!otStudy_.getFileName().empty())
      setToolTip(QString::fromUtf8(otStudy_.getFileName().c_str()));
  }
  else if (message == "statusChanged")
  {
    // emit signal to change the updateIcon
    // do NOT call directly updateIcon because is not 'thread safe'
    // (some notifications are emitted when the analyses are running)
    emit statusChanged();
  }
  else
  {
    qDebug() << "In OTStudyItem::update: not recognized message: " << message.data() << "\n";
  }
}


void OTStudyItem::updateIcon()
{
  emitDataChanged();
}


void OTStudyItem::createSymbolicModel()
{
  SymbolicPhysicalModel * newModel = new SymbolicPhysicalModel(otStudy_.getAvailablePhysicalModelName());
  otStudy_.add(newModel);
}


void OTStudyItem::createPythonModel()
{
  PythonPhysicalModel * newModel = new PythonPhysicalModel(otStudy_.getAvailablePhysicalModelName());
  otStudy_.add(newModel);
}


#ifdef OTGUI_HAVE_YACS
void OTStudyItem::createYACSModel()
{
  YACSPhysicalModel * newModel = new YACSPhysicalModel(otStudy_.getAvailablePhysicalModelName());
  otStudy_.add(newModel);
}
#endif


#ifdef OTGUI_HAVE_OTFMI
void OTStudyItem::createFMIModel()
{
  FMIPhysicalModel newModel(otStudy_.getAvailablePhysicalModelName());
  otStudy_.add(newModel);
}
#endif


void OTStudyItem::createDataModel()
{
  DesignOfExperiment newDataModel(DataModel(otStudy_.getAvailableDataModelName()));
  otStudy_.add(newDataModel);
}


void OTStudyItem::exportPythonScript(const QString& filename)
{
  QString fileName = filename;

  if (fileName.isEmpty())
  {
    qDebug() << "OTStudyItem::exportStudy : file name empty\n";
    return;
  }

  if (!fileName.endsWith(".py"))
    fileName += ".py";

  QFile file(fileName);

  // check
  if (!file.open(QFile::WriteOnly))
  {
    qDebug() << "OTStudyItem::exportStudy : cannot open the file " << file.fileName() << "\n";
    emit showErrorMessageRequested(tr("Cannot read file %1:\n%2").arg(fileName).arg(file.errorString()));
    return;
  }

  // write file
  QTextStream out(&file);
  out.setCodec("UTF-8");
  out << QString::fromUtf8(otStudy_.getPythonScript().c_str());
  file.setPermissions(QFile::ReadUser | QFile::WriteUser | QFile::ExeUser | QFile::ReadGroup | QFile::ExeGroup | QFile::ReadOther | QFile::ExeOther);
  file.close();

  // update QSettings
  FileTools::SetCurrentDir(fileName);
}


void OTStudyItem::emitSave()
{
  emit saveRequested(this);
}


void OTStudyItem::emitSaveAs()
{
  emit saveAsRequested(this);
}


bool OTStudyItem::save(const QString& filename)
{
  QString fileName = filename;

  if (fileName.isEmpty())
  {
    qDebug() << "OTStudyItem::saveOTStudy : file name empty\n";
    return false;
  }

  if (!fileName.endsWith(".xml"))
    fileName += ".xml";

  QFile file(fileName);

  // check
  if (!file.open(QFile::WriteOnly))
  {
    qDebug() << "OTStudyItem::saveOTStudy : cannot open the file " << file.fileName() << "\n";
    emit showErrorMessageRequested(tr("Cannot save file %1:\n%2").arg(fileName).arg(file.errorString()));
    return false;
  }

  // write file
  QApplication::setOverrideCursor(Qt::WaitCursor);
  otStudy_.save(fileName.toUtf8().constData());
  QApplication::restoreOverrideCursor();

  // update QSettings
  FileTools::SetCurrentDir(fileName);

  return true;
}


void OTStudyItem::emitClose()
{
  emit closeRequested(this);
}


void OTStudyItem::appendItem(DesignOfExperiment & dataModel)
{
  OTguiItem * item = getTitleItemNamed(tr("Data models"), "DataModelsTitle");

  // context menu actions
  if (!item->getActions().size())
  {
    item->appendAction(newDataModel_);
  }

  // new Data model item
  DataModelDiagramItem * newItem = new DataModelDiagramItem(dataModel);
  item->appendRow(newItem);

  // signal for StudyTreeView to create the window
  emit dataModelItemCreated(newItem);

  // Add sub items
  newItem->fill();
}


void OTStudyItem::appendItem(PhysicalModel & physicalModel)
{
  const QString title = (physicalModel.getImplementation()->getClassName() == "MetaModel") ? tr("Metamodels") : tr("Physical models");
  const QString titleType = (physicalModel.getImplementation()->getClassName() == "MetaModel") ? "MetaModelsTitle" : "PhysicalModelsTitle";
  OTguiItem * item = getTitleItemNamed(title, titleType);

  // context menu actions
  if (!item->getActions().size())
  {
    item->appendAction(newSymbolicModel_);
    item->appendAction(newPythonModel_);
#ifdef OTGUI_HAVE_YACS
    item->appendAction(newYACSModel_);
#endif
#ifdef OTGUI_HAVE_OTFMI
    item->appendAction(newFMIModel_);
#endif
  }

  // new Physical model item
  PhysicalModelDiagramItem * newModelItem = new PhysicalModelDiagramItem(physicalModel);
  item->appendRow(newModelItem);

  // signal for StudyTreeView to create the window
  emit physicalModelItemCreated(newModelItem);

  // Add sub items
  newModelItem->fill();
}


void OTStudyItem::appendItem(LimitState & limitState)
{
  // search PhysicalModelDiagram observer
  if (PhysicalModelDiagramItem * pmItem = dynamic_cast<PhysicalModelDiagramItem*>(limitState.getPhysicalModel().getImplementation().get()->getObserver("PhysicalModelDiagram")))
  {
    // append item for limitState
    pmItem->appendItem(limitState);
  }
  else
  {
    qDebug() << "In OTStudyItem::appendItem: No item added for the limit state named " << limitState.getName().data() << "\n"
             << "No physical model matches the name " << limitState.getPhysicalModel().getName().data() << "\n";
  }
}


void OTStudyItem::appendItem(Analysis & analysis)
{
  if (PhysicalModelAnalysis * pmAnalysis = dynamic_cast<PhysicalModelAnalysis*>(analysis.getImplementation().get()))
  {
    PhysicalModel model(pmAnalysis->getPhysicalModel());
    // search PhysicalModelDiagram observer
    if (PhysicalModelDiagramItem * pmItem = dynamic_cast<PhysicalModelDiagramItem*>(model.getImplementation().get()->getObserver("PhysicalModelDiagram")))
    {
      // append item for analysis
      pmItem->appendItem(analysis);
    }
    else
    {
      qDebug() << "In OTStudyItem::appendItem: No item added for the analysis named " << analysis.getName().data() << ". Physical model item not found.\n";
    }
  }
  else if (DesignOfExperimentAnalysis * dmAnalysis = dynamic_cast<DesignOfExperimentAnalysis*>(analysis.getImplementation().get()))
  {
    DesignOfExperiment design(dmAnalysis->getDesignOfExperiment());
    // search DesignOfExperiment observer
    if (design.hasPhysicalModel())
    {
      DesignOfExperimentDefinitionItem * doeItem = dynamic_cast<DesignOfExperimentDefinitionItem*>(design.getImplementation().get()->getObserver("DesignOfExperimentDefinition"));
      Q_ASSERT(doeItem);
      // append item for analysis
      doeItem->appendItem(analysis);
    }
    else
    {
      DataModelDiagramItem * doeItem = dynamic_cast<DataModelDiagramItem*>(design.getImplementation().get()->getObserver("DataModelDiagram"));
      Q_ASSERT(doeItem);
      // append item for analysis
      doeItem->appendItem(analysis);
    }
  }
  else
    qDebug() << "In OTStudyItem::appendItem: No item added for the analysis named " << analysis.getName().data() << "\n";
}


void OTStudyItem::appendMetaModelItem(PhysicalModel metaModel)
{
  const String availableName = otStudy_.getAvailablePhysicalModelName(metaModel.getName());
  metaModel.setName(availableName);
  otStudy_.add(metaModel);
}


QVariant OTStudyItem::data(int role) const
{
  // set icon
  if (role == Qt::DecorationRole)
  {
    if (otStudy_.getImplementation()->hasBeenModified())
      return QIcon(":/images/document-save.png");
    else
      return QIcon();
  }
  return OTguiItem::data(role);
}


void OTStudyItem::setData(const QVariant & value, int role)
{
  // do NOT use otStudy_.setName otherwise otStudy_ is duplicated!!
  // when copying an Observable, the list of observers is not duplicated...
  // when an observable has no observer, otgui is lost
  // for example is not possible to remove the items...
  if (role == Qt::EditRole)
    otStudy_.getImplementation()->setName(value.toString().toUtf8().data());

  QStandardItem::setData(value, role);
}


OTStudy OTStudyItem::getOTStudy() const
{
  return otStudy_;
}


void OTStudyItem::requestRemove()
{
  emit removeWindowRequested();
  if (model())
    model()->invisibleRootItem()->removeRow(row());
}
}
