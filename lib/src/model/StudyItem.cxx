//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a study
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#ifdef PERSALYS_HAVE_YACS
#include "persalys/YACSPhysicalModel.hxx"
#endif
#include "persalys/StudyItem.hxx"

#include "persalys/FileTools.hxx"
#include "persalys/PhysicalModelAnalysis.hxx"
#include "persalys/DesignOfExperimentAnalysis.hxx"
#include "persalys/SymbolicPhysicalModel.hxx"
#include "persalys/SymbolicFieldModel.hxx"
#include "persalys/PythonPhysicalModel.hxx"
#include "persalys/PythonFieldModel.hxx"

#ifdef PERSALYS_HAVE_OTFMI
#include "persalys/FMIPhysicalModel.hxx"
#endif

#include <QDebug>
#include <QFileInfo>
#include <QApplication>

using namespace OT;

namespace PERSALYS
{

StudyItem::StudyItem(const Study& study)
  : Item(QString::fromUtf8(study.getName().c_str()), "Study")
  , Observer("Study")
  , study_(study)
{
  parentStudyItem_ = this;
  study_.addObserver(this);

  buildActions();
  connect(this, SIGNAL(statusChanged()), this, SLOT(updateIcon()));
}


void StudyItem::buildActions()
{
  // new model actions
  newSymbolicModel_ = new QAction(tr("Symbolic model"), this);
  connect(newSymbolicModel_, SIGNAL(triggered()), this, SLOT(createSymbolicModel()));

  newPythonModel_ = new QAction(tr("Python model"), this);
  connect(newPythonModel_, SIGNAL(triggered()), this, SLOT(createPythonModel()));

#ifdef PERSALYS_HAVE_YACS
  newYACSModel_ = new QAction(tr("YACS model"), this);
  connect(newYACSModel_, SIGNAL(triggered()), this, SLOT(createYACSModel()));
#endif
#ifdef PERSALYS_HAVE_OTFMI
  newFMIModel_ = new QAction(tr("FMI model"), this);
  connect(newFMIModel_, SIGNAL(triggered()), this, SLOT(createFMIModel()));
#endif
  // new model actions
  newSymbolicFieldModel_ = new QAction(tr("Symbolic Field model"), this);
  connect(newSymbolicFieldModel_, SIGNAL(triggered()), this, SLOT(createSymbolicFieldModel()));

  newPythonFieldModel_ = new QAction(tr("Python Field model"), this);
  connect(newPythonFieldModel_, SIGNAL(triggered()), this, SLOT(createPythonFieldModel()));


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
#ifdef PERSALYS_HAVE_YACS
  appendAction(newYACSModel_);
#endif
#ifdef PERSALYS_HAVE_OTFMI
  appendAction(newFMIModel_);
#endif
  appendAction(newSymbolicFieldModel_);
  appendAction(newPythonFieldModel_);
  appendAction(newDataModel_);
  appendSeparator();
  appendAction(exportAction_);
  appendAction(saveAction_);
  appendSeparator();
  appendAction(closeAction_);
}


void StudyItem::update(Observable * source, const String & message)
{
  if (message == "addDataModel")
  {
    DesignOfExperiment addedDataModel = study_.getDataModels()[study_.getDataModels().getSize() - 1];
    appendItem(addedDataModel);
  }
  else if (message == "addPhysicalModel")
  {
    PhysicalModel addedPhysicalModel = study_.getPhysicalModels()[study_.getPhysicalModels().getSize() - 1];
    appendItem(addedPhysicalModel);
  }
  else if (message == "addLimitState")
  {
    LimitState addedLimitState = study_.getLimitStates()[study_.getLimitStates().getSize() - 1];
    appendItem(addedLimitState);
  }
  else if (message == "addAnalysis")
  {
    Analysis addedAnalysis = study_.getAnalyses()[study_.getAnalyses().getSize() - 1];
    appendItem(addedAnalysis);
  }
  else if (message == "studyRemoved")
  {
    requestRemove();
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
    qDebug() << "In StudyItem::update: not recognized message: " << message.data() << "\n";
  }
}


void StudyItem::updateIcon()
{
  emitDataChanged();
}


void StudyItem::createSymbolicModel()
{
  SymbolicPhysicalModel * newModel = new SymbolicPhysicalModel(study_.getAvailablePhysicalModelName(tr("SymbolicModel_").toStdString()));
  study_.add(newModel);
}


void StudyItem::createSymbolicFieldModel()
{
  SymbolicFieldModel * newModel = new SymbolicFieldModel(study_.getAvailablePhysicalModelName(tr("SymbolicModel_").toStdString()));
  study_.add(newModel);
}


void StudyItem::createPythonModel()
{
  PythonPhysicalModel * newModel = new PythonPhysicalModel(study_.getAvailablePhysicalModelName(tr("PythonModel_").toStdString()));
  study_.add(newModel);
}


void StudyItem::createPythonFieldModel()
{
  PythonFieldModel * newModel = new PythonFieldModel(study_.getAvailablePhysicalModelName(tr("PythonModel_").toStdString()));
  study_.add(newModel);
}


#ifdef PERSALYS_HAVE_YACS
void StudyItem::createYACSModel()
{
  YACSPhysicalModel * newModel = new YACSPhysicalModel(study_.getAvailablePhysicalModelName(tr("YACSModel_").toStdString()));
  study_.add(newModel);
}
#endif


#ifdef PERSALYS_HAVE_OTFMI
void StudyItem::createFMIModel()
{
  FMIPhysicalModel newModel(study_.getAvailablePhysicalModelName(tr("FMIModel_").toStdString()));
  study_.add(newModel);
}
#endif


void StudyItem::createDataModel()
{
  DesignOfExperiment newDataModel(DataModel(study_.getAvailableDataModelName(tr("DataModel_").toStdString())));
  study_.add(newDataModel);
}


void StudyItem::exportPythonScript(const QString& filename)
{
  QString fileName = filename;

  if (fileName.isEmpty())
  {
    qDebug() << "StudyItem::exportStudy : file name empty\n";
    return;
  }

  if (!fileName.endsWith(".py"))
    fileName += ".py";

  QFile file(fileName);

  // check
  if (!file.open(QFile::WriteOnly))
  {
    qDebug() << "StudyItem::exportStudy : cannot open the file " << file.fileName() << "\n";
    emit showErrorMessageRequested(tr("Cannot read file %1:\n%2").arg(fileName).arg(file.errorString()));
    return;
  }

  // write file
  QTextStream out(&file);
  out.setCodec("UTF-8");
  out << QString::fromUtf8(study_.getPythonScript().c_str());
  file.setPermissions(QFile::ReadUser | QFile::WriteUser | QFile::ExeUser | QFile::ReadGroup | QFile::ExeGroup | QFile::ReadOther | QFile::ExeOther);
  file.close();

  // update QSettings
  FileTools::SetCurrentDir(fileName);
}


void StudyItem::emitSave()
{
  emit saveRequested(this);
}


void StudyItem::emitSaveAs()
{
  emit saveAsRequested(this);
}


bool StudyItem::save(const QString& filename)
{
  QString fileName = filename;

  if (fileName.isEmpty())
  {
    qDebug() << "StudyItem::saveStudy : file name empty\n";
    return false;
  }

  if (!fileName.endsWith(".xml"))
    fileName += ".xml";

  QFile file(fileName);

  // check
  if (!file.open(QFile::WriteOnly))
  {
    qDebug() << "StudyItem::saveStudy : cannot open the file " << file.fileName() << "\n";
    emit showErrorMessageRequested(tr("Cannot save file %1:\n%2").arg(fileName).arg(file.errorString()));
    return false;
  }

  // write file
  QApplication::setOverrideCursor(Qt::WaitCursor);
  study_.save(fileName.toUtf8().constData());
  QApplication::restoreOverrideCursor();

  // update QSettings
  FileTools::SetCurrentDir(fileName);

  return true;
}


void StudyItem::emitClose()
{
  emit closeRequested(this);
}


void StudyItem::appendItem(DesignOfExperiment & dataModel)
{
  if (!dataModel.hasPhysicalModel())
  {
    Item * item = getTitleItemNamed(tr("Data models"), "DataModelsTitle");

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
  else // Observations
  {
    // search PhysicalModelDiagram observer
    if (PhysicalModelDiagramItem * pmItem = dynamic_cast<PhysicalModelDiagramItem*>(dataModel.getPhysicalModel().getImplementation().get()->getObserver("PhysicalModelDiagram")))
    {
      // append item for limitState
      pmItem->appendItem(dataModel);
    }
  }
}


void StudyItem::appendItem(PhysicalModel & physicalModel)
{
  const bool isMetaModelTitle = physicalModel.getImplementation()->getClassName() == "MetaModel";
  const QString title = isMetaModelTitle ? tr("Metamodels") : tr("Physical models");
  const QString titleType = isMetaModelTitle ? "MetaModelsTitle" : "PhysicalModelsTitle";
  Item * item = getTitleItemNamed(title, titleType);

  // context menu actions
  if (!item->getActions().size() && !isMetaModelTitle)
  {
    item->appendAction(newSymbolicModel_);
    item->appendAction(newPythonModel_);
#ifdef PERSALYS_HAVE_YACS
    item->appendAction(newYACSModel_);
#endif
#ifdef PERSALYS_HAVE_OTFMI
    item->appendAction(newFMIModel_);
#endif
  }

  // new Physical model item
  PhysicalModelDiagramItem * newModelItem = new PhysicalModelDiagramItem(physicalModel);
  item->appendRow(newModelItem);

  // signal for StudyTreeView to create the window
  if (!physicalModel.hasMesh())
    emit physicalModelItemCreated(newModelItem);
  else
    emit fieldModelItemCreated(newModelItem);

  // Add sub items
  newModelItem->fill();
}


void StudyItem::appendItem(LimitState & limitState)
{
  // search PhysicalModelDiagram observer
  if (PhysicalModelDiagramItem * pmItem = dynamic_cast<PhysicalModelDiagramItem*>(limitState.getPhysicalModel().getImplementation().get()->getObserver("PhysicalModelDiagram")))
  {
    // append item for limitState
    pmItem->appendItem(limitState);
  }
  else
  {
    qDebug() << "In StudyItem::appendItem: No item added for the limit state named " << limitState.getName().data() << "\n"
             << "No physical model matches the name " << limitState.getPhysicalModel().getName().data() << "\n";
  }
}


void StudyItem::appendItem(Analysis & analysis)
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
      qDebug() << "In StudyItem::appendItem: No item added for the analysis named " << analysis.getName().data() << ". Physical model item not found.\n";
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
    qDebug() << "In StudyItem::appendItem: No item added for the analysis named " << analysis.getName().data() << "\n";
}


void StudyItem::appendMetaModelItem(PhysicalModel metaModel)
{
  const String availableName = study_.getAvailablePhysicalModelName(metaModel.getName());
  metaModel.setName(availableName);
  study_.add(metaModel);
}


QVariant StudyItem::data(int role) const
{
  // set icon
  if (role == Qt::DecorationRole)
  {
    if (study_.getImplementation()->hasBeenModified())
      return QIcon(":/images/document-save.png");
    else
      return QIcon();
  }
  // tooltip
  if (role == Qt::ToolTipRole && !study_.getFileName().empty())
    return QFileInfo(study_.getFileName().c_str()).absoluteFilePath();

  return Item::data(role);
}


void StudyItem::setData(const QVariant & value, int role)
{
  // do NOT use study_.setName otherwise study_ is duplicated!!
  // when copying an Observable, the list of observers is not duplicated...
  // when an observable has no observer, persalys is lost
  // for example is not possible to remove the items...
  if (role == Qt::EditRole)
    study_.getImplementation()->setName(value.toString().toUtf8().data());

  QStandardItem::setData(value, role);
}


Study StudyItem::getStudy() const
{
  return study_;
}


void StudyItem::requestRemove()
{
  emit removeWindowRequested();
  if (model())
    model()->invisibleRootItem()->removeRow(row());
}
}
