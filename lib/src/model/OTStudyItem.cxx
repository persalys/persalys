//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of an otStudy
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/OTStudyItem.hxx"

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
#include <QSettings>
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
  connect(this, SIGNAL(otStudyStatusChanged()), this, SLOT(updateIcon()));
}


void OTStudyItem::buildActions()
{
  // new model actions
  newSymbolicPhysicalModel_ = new QAction(tr("Symbolic model"), this);
  connect(newSymbolicPhysicalModel_, SIGNAL(triggered()), this, SLOT(createNewSymbolicPhysicalModel()));

  newPythonPhysicalModel_ = new QAction(tr("Python model"), this);
  connect(newPythonPhysicalModel_, SIGNAL(triggered()), this, SLOT(createNewPythonPhysicalModel()));

#ifdef OTGUI_HAVE_YACS
  newYACSPhysicalModel_ = new QAction(tr("YACS model"), this);
  connect(newYACSPhysicalModel_, SIGNAL(triggered()), this, SLOT(createNewYACSPhysicalModel()));
#endif
#ifdef OTGUI_HAVE_OTFMI
  newFMIPhysicalModel_ = new QAction(tr("FMI model"), this);
  connect(newFMIPhysicalModel_, SIGNAL(triggered()), this, SLOT(createNewFMIPhysicalModel()));
#endif

  newDataModel_ = new QAction(tr("Data model"), this);
  connect(newDataModel_, SIGNAL(triggered()), this, SLOT(createNewDataModel()));

  // export action
  exportOTStudy_ = new QAction(QIcon(":/images/document-export.png"), tr("Export Python"), this);
  connect(exportOTStudy_, SIGNAL(triggered()), this, SIGNAL(otStudyExportRequested()));

  // save  action
  saveOTStudy_ = new QAction(QIcon(":/images/document-save.png"), tr("Save"), this);
  connect(saveOTStudy_, SIGNAL(triggered()), this, SLOT(saveOTStudy()));

  // save as  action
  saveAsOTStudy_ = new QAction(QIcon(":/images/document-save-as.png"), tr("Save As..."), this);
  connect(saveAsOTStudy_, SIGNAL(triggered()), this, SIGNAL(otStudySaveAsRequested()));

  // close action
  closeOTStudy_ = new QAction(QIcon(":/images/window-close.png"), tr("Close"), this);
  connect(closeOTStudy_, SIGNAL(triggered()), this, SLOT(closeOTStudy()));

  // add actions
  appendAction(newSymbolicPhysicalModel_);
  appendAction(newPythonPhysicalModel_);
#ifdef OTGUI_HAVE_YACS
  appendAction(newYACSPhysicalModel_);
#endif
#ifdef OTGUI_HAVE_OTFMI
  appendAction(newFMIPhysicalModel_);
#endif
  appendAction(newDataModel_);
  appendAction(exportOTStudy_);
  appendAction(saveOTStudy_);
  appendAction(closeOTStudy_);
}


void OTStudyItem::update(Observable * source, const String & message)
{
  if (message == "addDataModel")
  {
    DesignOfExperiment addedDataModel = otStudy_.getDataModels()[otStudy_.getDataModels().getSize() - 1];
    addDataModelItem(addedDataModel);
  }
  else if (message == "addPhysicalModel")
  {
    PhysicalModel addedPhysicalModel = otStudy_.getPhysicalModels()[otStudy_.getPhysicalModels().getSize() - 1];
    addPhysicalModelItem(addedPhysicalModel);
  }
  else if (message == "addLimitState")
  {
    LimitState addedLimitState = otStudy_.getLimitStates()[otStudy_.getLimitStates().getSize() - 1];
    addLimitStateItem(addedLimitState);
  }
  else if (message == "addAnalysis")
  {
    Analysis addedAnalysis = otStudy_.getAnalyses()[otStudy_.getAnalyses().getSize() - 1];
    addAnalysisItem(addedAnalysis);
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
    emit otStudyStatusChanged();
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


void OTStudyItem::createNewSymbolicPhysicalModel()
{
  SymbolicPhysicalModel * newPhysicalModel = new SymbolicPhysicalModel(otStudy_.getAvailablePhysicalModelName());
  otStudy_.add(newPhysicalModel);
}


void OTStudyItem::createNewPythonPhysicalModel()
{
  PythonPhysicalModel * newPhysicalModel = new PythonPhysicalModel(otStudy_.getAvailablePhysicalModelName());
  otStudy_.add(newPhysicalModel);
}


#ifdef OTGUI_HAVE_YACS
void OTStudyItem::createNewYACSPhysicalModel()
{
  YACSPhysicalModel * newPhysicalModel = new YACSPhysicalModel(otStudy_.getAvailablePhysicalModelName());
  otStudy_.add(newPhysicalModel);
}
#endif


#ifdef OTGUI_HAVE_OTFMI
void OTStudyItem::createNewFMIPhysicalModel()
{
  FMIPhysicalModel newPhysicalModel(otStudy_.getAvailablePhysicalModelName());
  otStudy_.add(newPhysicalModel);
}
#endif


void OTStudyItem::createNewDataModel()
{
  DesignOfExperiment newDataModel(DataModel(otStudy_.getAvailableDataModelName()));
  otStudy_.add(newDataModel);
}


void OTStudyItem::exportOTStudy(QString fileName)
{
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
    emit emitErrorMessageRequested(tr("Cannot read file %1:\n%2").arg(fileName).arg(file.errorString()));
    return;
  }

  // write file
  QTextStream out(&file);
  out.setCodec("UTF-8");
  out << QString::fromUtf8(otStudy_.getPythonScript().c_str());
  file.setPermissions(QFile::ReadUser | QFile::WriteUser | QFile::ExeUser | QFile::ReadGroup | QFile::ExeGroup | QFile::ReadOther | QFile::ExeOther);
  file.close();

  // update QSettings
  QSettings settings;
  settings.setValue("currentDir", QFileInfo(fileName).absolutePath());
}


bool OTStudyItem::saveOTStudy()
{
  if (!QFileInfo(QString::fromUtf8(otStudy_.getFileName().c_str())).exists())
  {
    bool notcancel = true;
    emit otStudySaveAsRequested(this, &notcancel);

    if (!notcancel)
      return false;

    return true;
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);
  otStudy_.save(otStudy_.getFileName());
  QApplication::restoreOverrideCursor();
  return true;
}


bool OTStudyItem::saveOTStudy(QString fileName)
{
  if (fileName.isEmpty())
  {
    qDebug() << "OTStudyItem::saveOTStudy : file name empty\n";
    return true;
  }

  if (!fileName.endsWith(".xml"))
    fileName += ".xml";

  QFile file(fileName);

  // check
  if (!file.open(QFile::WriteOnly))
  {
    qDebug() << "OTStudyItem::saveOTStudy : cannot open the file " << file.fileName() << "\n";
    emit emitErrorMessageRequested(tr("Cannot save file %1:\n%2").arg(fileName).arg(file.errorString()));
    return false;
  }

  // write file
  QApplication::setOverrideCursor(Qt::WaitCursor);
  otStudy_.save(fileName.toUtf8().constData());
  QApplication::restoreOverrideCursor();

  emit recentFilesListChanged(fileName);

  // update QSettings
  QSettings settings;
  settings.setValue("currentDir", QFileInfo(fileName).absolutePath());

  return true;
}


bool OTStudyItem::closeOTStudy()
{
  // check if the analysis is running
  if (analysisInProgress_)
  {
    emit emitErrorMessageRequested(tr("Can not remove a study when an analysis is running."));
    return false;
  }

  // if there are modifications
  bool canClose = false;

  if (otStudy_.getImplementation().get()->hasBeenModified())
    emit otStudyCloseRequested(this, &canClose);
  else
    canClose = true;

  if (canClose)
    OTStudy::Remove(otStudy_);

  return canClose;
}


void OTStudyItem::addDataModelItem(DesignOfExperiment & dataModel)
{
  OTguiItem * item = getTitleItemNamed(tr("Data models"), "DataModelsTitle");

  // context menu actions
  if (!item->getActions().size())
  {
    item->appendAction(newDataModel_);
  }

  // new Data model item
  DataModelDiagramItem * newDataModelItem = new DataModelDiagramItem(dataModel);
  item->appendRow(newDataModelItem);

  // signal for StudyTreeView to create the window
  emit newDataModelItemCreated(newDataModelItem);

  // Add sub items
  newDataModelItem->fill();
}


void OTStudyItem::addPhysicalModelItem(PhysicalModel & physicalModel)
{
  const QString title = (physicalModel.getImplementation()->getClassName() == "MetaModel") ? tr("Metamodels") : tr("Physical models");
  const QString titleType = (physicalModel.getImplementation()->getClassName() == "MetaModel") ? "MetaModelsTitle" : "PhysicalModelsTitle";
  OTguiItem * item = getTitleItemNamed(title, titleType);

  // context menu actions
  if (!item->getActions().size())
  {
    item->appendAction(newSymbolicPhysicalModel_);
    item->appendAction(newPythonPhysicalModel_);
#ifdef OTGUI_HAVE_YACS
    item->appendAction(newYACSPhysicalModel_);
#endif
#ifdef OTGUI_HAVE_OTFMI
    item->appendAction(newFMIPhysicalModel_);
#endif
  }

  // new Physical model item
  PhysicalModelDiagramItem * newPhysicalModelItem = new PhysicalModelDiagramItem(physicalModel);
  item->appendRow(newPhysicalModelItem);

  // signal for StudyTreeView to create the window
  emit newPhysicalModelItemCreated(newPhysicalModelItem);

  // Add sub items
  newPhysicalModelItem->fill();
}


void OTStudyItem::addLimitStateItem(LimitState & limitState)
{
  // search PhysicalModelDiagram observer
  PhysicalModelDiagramItem * pmItem = dynamic_cast<PhysicalModelDiagramItem*>(limitState.getPhysicalModel().getImplementation().get()->getObserver("PhysicalModelDiagram"));
  if (!pmItem)
  {
    qDebug() << "In OTStudyItem::addLimitStateItem: No item added for the limit state named " << limitState.getName().data() << "\n"
             << "No physical model matches the name " << limitState.getPhysicalModel().getName().data() << "\n";
    return;
  }

  // append item for limitState
  pmItem->appendLimitStateItem(limitState);
}


void OTStudyItem::addAnalysisItem(Analysis & analysis)
{
  if (dynamic_cast<PhysicalModelAnalysis*>(analysis.getImplementation().get()))
  {
    PhysicalModel model(dynamic_cast<PhysicalModelAnalysis*>(analysis.getImplementation().get())->getPhysicalModel());
    // search PhysicalModelDiagram observer
    PhysicalModelDiagramItem * pmItem = dynamic_cast<PhysicalModelDiagramItem*>(model.getImplementation().get()->getObserver("PhysicalModelDiagram"));
    if (!pmItem)
    {
      qDebug() << "In OTStudyItem::addAnalysisItem: No item added for the analysis named " << analysis.getName().data() << ". Physical model item not found.\n";
      return;
    }
    // append item for analysis
    pmItem->appendAnalysisItem(analysis);
  }
  else if (dynamic_cast<DesignOfExperimentAnalysis*>(analysis.getImplementation().get()))
  {
    DesignOfExperiment design(dynamic_cast<DesignOfExperimentAnalysis*>(analysis.getImplementation().get())->getDesignOfExperiment());
    // search DesignOfExperiment observer
    if (design.hasPhysicalModel())
    {
      DesignOfExperimentDefinitionItem * doeItem = dynamic_cast<DesignOfExperimentDefinitionItem*>(design.getImplementation().get()->getObserver("DesignOfExperimentDefinition"));
      Q_ASSERT(doeItem);
      // append item for analysis
      doeItem->appendAnalysisItem(analysis);
    }
    else
    {
      DataModelDiagramItem * doeItem = dynamic_cast<DataModelDiagramItem*>(design.getImplementation().get()->getObserver("DataModelDiagram"));
      Q_ASSERT(doeItem);
      // append item for analysis
      doeItem->appendAnalysisItem(analysis);
    }
  }
  else
    qDebug() << "In OTStudyItem::addAnalysisItem: No item added for the analysis named " << analysis.getName().data() << "\n";
}


void OTStudyItem::addMetaModelItem(PhysicalModel metaModel)
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
  else
    return OTguiItem::data(role);
}


void OTStudyItem::setData(const QVariant & value, int role)
{
  // do NOT use otStudy_.setName otherwise otStudy_ is duplicated!!
  // when copying an Observable, the list of observers is not duplicated...
  // when an observable has no observer, otgui is lost
  // for example is not possible to remove the items...
  if (role == Qt::EditRole)
    otStudy_.getImplementation()->setName(value.toString().toLocal8Bit().data());

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
