//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a study
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#include "persalys/YACSCouplingPhysicalModel.hxx"
#endif
#include "persalys/StudyItem.hxx"

#include "persalys/FileTools.hxx"
#include "persalys/PhysicalModelAnalysis.hxx"
#include "persalys/DesignOfExperimentAnalysis.hxx"
#include "persalys/SymbolicPhysicalModel.hxx"
#include "persalys/SymbolicFieldModel.hxx"
#include "persalys/PythonPhysicalModel.hxx"
#include "persalys/PythonFieldModel.hxx"
#include "persalys/ReliabilityAnalysis.hxx"
#include "persalys/CalibrationAnalysis.hxx"
#include "persalys/CouplingPhysicalModel.hxx"

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
  , Observer("StudyItem")
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
  connect(newSymbolicModel_, &QAction::triggered, [=](){ study_.add(new SymbolicPhysicalModel(getModelName(tr("SymbolicModel_")))); });

  newPythonModel_ = new QAction(tr("Python model"), this);
  connect(newPythonModel_, &QAction::triggered, [=](){ study_.add(new PythonPhysicalModel(getModelName(tr("PythonModel_")))); });

  newCouplingModel_ = new QAction(tr("Coupling model"), this);
#ifdef PERSALYS_HAVE_YACS
  newYACSModel_ = new QAction(tr("YACS model"), this);
  connect(newYACSModel_, &QAction::triggered, [=](){ study_.add(new YACSPhysicalModel(getModelName(tr("YACSModel_")))); });

  connect(newCouplingModel_, &QAction::triggered, [=](){ study_.add(new YACSCouplingPhysicalModel(getModelName(tr("CouplingModel_")))); });
#else
  connect(newCouplingModel_, &QAction::triggered, [=](){ study_.add(new CouplingPhysicalModel(getModelName(tr("CouplingModel_")))); });
#endif
#ifdef PERSALYS_HAVE_OTFMI
  newFMIModel_ = new QAction(tr("FMI model"), this);
  connect(newFMIModel_, &QAction::triggered, [=](){ study_.add(new FMIPhysicalModel(getModelName(tr("FMIModel_")))); });
#endif

  // new model actions
  newSymbolicFieldModel_ = new QAction(tr("Symbolic Field model"), this);
  connect(newSymbolicFieldModel_, &QAction::triggered, [=](){ study_.add(new SymbolicFieldModel(getModelName(tr("SymbolicModel_")))); });

  newPythonFieldModel_ = new QAction(tr("Python Field model"), this);
  connect(newPythonFieldModel_, &QAction::triggered, [=](){ study_.add(new PythonFieldModel(getModelName(tr("PythonModel_")))); });

  newDataModel_ = new QAction(tr("Data model"), this);
  connect(newDataModel_, &QAction::triggered, [=](){ study_.add(new DataModel(study_.getAvailableDataModelName(tr("DataModel_").toStdString()))); });

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
  appendAction(newCouplingModel_);
  appendAction(newDataModel_);
  appendSeparator();
  appendAction(exportAction_);
  appendAction(saveAction_);
  appendSeparator();
  appendAction(closeAction_);
}


String StudyItem::getModelName(const QString &baseName) const
{
  return study_.getAvailablePhysicalModelName(baseName.toStdString());
}


void StudyItem::update(Observable * /*source*/, const String & message)
{
  if (message == "analysisLaunched")
  {
    analysisInProgress_ = true;
  }
  else if (message == "analysisFinished" || message == "analysisBadlyFinished")
  {
    analysisInProgress_ = false;
  }
  else if (message == "statusChanged")
  {
    // emit signal to change the updateIcon
    // do NOT call directly updateIcon because is not 'thread safe'
    // (some notifications are emitted when the analyses are running)
    emit statusChanged();
  }
  else if (message == "objectRemoved")
  {
    requestRemove();
  }
}


void StudyItem::updateIcon()
{
  emitDataChanged();
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


void StudyItem::appendItem(const DesignOfExperiment &dataModel)
{
  if (!dataModel.hasPhysicalModel())
  {
    // new Data model item
    DataModelDiagramItem * newItem = new DataModelDiagramItem(dataModel);
    Item * titleItem = getTitleItemNamed("DataModel");
    titleItem->appendRow(newItem);

    // signal for StudyTreeView to create the window
    emit windowRequested(newItem);

    // Add sub items
    newItem->fill();
  }
  else if (Observer * observer = dataModel.getPhysicalModel().getImplementation()->getObserver("PhysicalModelDiagramItem"))
  {
    observer->appendItem(dataModel);
  }
  else
  {
    qDebug() << "In StudyItem::appendItem: No item added for the design of experiment\n";
  }
}


void StudyItem::appendItem(const PhysicalModel &physicalModel)
{
  // new Physical model item
  PhysicalModelDiagramItem * newModelItem = new PhysicalModelDiagramItem(physicalModel);
  Item * titleItem = getTitleItemNamed(physicalModel.getImplementation()->getClassName().c_str());
  titleItem->appendRow(newModelItem);

  // signal for StudyTreeView to create the window
  emit windowRequested(newModelItem);

  // Add sub items
  newModelItem->fill();
}


void StudyItem::appendItem(const LimitState &limitState)
{
  // search PhysicalModelDiagram observer
  if (Observer * observer = limitState.getPhysicalModel().getImplementation()->getObserver("PhysicalModelDiagramItem"))
  {
    observer->appendItem(limitState);
  }
  else
  {
    qDebug() << "In StudyItem::appendItem: No item added for the limit state named " << limitState.getName().data() << "\n"
             << "No physical model matches the name " << limitState.getPhysicalModel().getName().data() << "\n";
  }
}


void StudyItem::appendItem(const Analysis &analysis)
{
  if (Observer * observer = analysis.getParentObserver())
    observer->appendItem(analysis);
  else
    qDebug() << "In StudyItem::appendItem: No item added for the analysis named " << analysis.getName().data() << ". No found observer\n";
}


void StudyItem::appendMetaModelItem(PhysicalModel metaModel)
{
  const String availableName = metaModel.getName();
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
