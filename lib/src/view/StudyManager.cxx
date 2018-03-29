//                                               -*- C++ -*-
/**
 *  @brief QObject managing otStudy objects
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
#include "otgui/StudyManager.hxx"

#include "otgui/WindowFactory.hxx"

#include "otgui/OTStudyWindow.hxx"
#include "otgui/DataModelDiagramWindow.hxx"
#include "otgui/PhysicalModelDiagramWindow.hxx"
#include "otgui/ProbabilisticModelWindow.hxx"
#include "otgui/DesignOfExperimentInputWindow.hxx"
#include "otgui/LimitStateWindow.hxx"
#include "otgui/DataModelWindow.hxx"
#include "otgui/AnalysisWindow.hxx"
#include "otgui/FileTools.hxx"

#include "otgui/DesignOfExperimentEvaluationWizard.hxx"

#include <QFileDialog>
#include <QApplication>
#include <QMessageBox>

#include <iostream>

using namespace OT;

namespace OTGUI
{

StudyManager::StudyManager(MainWidget * mainWidget, QObject * parent)
  : QObject(parent)
  , mainWidget_(mainWidget)
  , analysisInProgress_(false)
{
  connect(mainWidget_->getStudyTree(), SIGNAL(studyCreated(OTStudyItem*)), this, SLOT(createOTStudyWindow(OTStudyItem*)));

  connect(mainWidget_->getActions()->newAction(), SIGNAL(triggered()), this, SLOT(createOTStudy()));
  connect(mainWidget_->getActions()->openAction(), SIGNAL(triggered()), this, SLOT(open()));
  connect(mainWidget_->getActions()->saveAction(), SIGNAL(triggered()), this, SLOT(saveCurrent()));
  connect(mainWidget_->getActions()->saveAsAction(), SIGNAL(triggered()), this, SLOT(saveAsCurrent()));
  connect(mainWidget_->getActions()->closeAction(), SIGNAL(triggered()), this, SLOT(closeCurrent()));
  connect(mainWidget_->getActions()->importPyAction(), SIGNAL(triggered()), this, SLOT(importPythonScript()));

  connect(this, SIGNAL(analysisInProgressStatusChanged(bool)), mainWidget_->getActions(), SLOT(updateActionsAvailability(bool)));
}


void StudyManager::setAnalysisInProgress(bool analysisInProgress)
{
  analysisInProgress_ = analysisInProgress;

  // emit signal to MenuBar/ToolBar to disable/enable import Python script actions
  //             to AnalysisWindows to disable/enable the run buttons
  emit analysisInProgressStatusChanged(analysisInProgress);
}


void StudyManager::showErrorMessage(QString message)
{
  QMessageBox::critical(mainWidget_, tr("Error"), message);
}


void StudyManager::updateView(OTguiSubWindow * window)
{
  mainWidget_->getMdiArea()->addSubWindow(window);
  mainWidget_->getStudyTree()->setCurrentIndex(window->getItem()->index());
  mainWidget_->getStudyTree()->setExpanded(window->getItem()->index(), true);
}


void StudyManager::createOTStudy()
{
  // create a new study with an available name
  OTStudy otstudy(OTStudy::GetAvailableName());
  OTStudy::Add(otstudy);
}


void StudyManager::openDesignOfExperimentEvaluationWizard(const Analysis& analysis, const bool isGeneralWizard)
{
  DesignOfExperimentEvaluationWizard * wizard = new DesignOfExperimentEvaluationWizard(analysis, isGeneralWizard, mainWidget_);

  if (wizard && wizard->exec())
  {
    wizard->getDesignOfExperimentDefinitionItem()->appendEvaluationItem();
    delete wizard;
  }
}


void StudyManager::openAnalysisWizard(OTguiItem* item, const Analysis& analysis, const bool isGeneralWizard)
{
  if (!item || !item->getParentOTStudyItem())
    return;

  AnalysisWizard * wizard = WindowFactory::GetAnalysisWizard(analysis, isGeneralWizard, mainWidget_);

  if (wizard && wizard->exec())
  {
    item->getParentOTStudyItem()->getOTStudy().add(wizard->getAnalysis());
    delete wizard;
  }
}


void StudyManager::createOTStudyWindow(OTStudyItem* item)
{
  if (!item)
    return;

  // connections
  connect(item, SIGNAL(showErrorMessageRequested(QString)), this, SLOT(showErrorMessage(QString)));
  connect(item, SIGNAL(dataModelItemCreated(DataModelDiagramItem*)), this, SLOT(createDataModelDiagramWindow(DataModelDiagramItem*)));
  connect(item, SIGNAL(physicalModelItemCreated(PhysicalModelDiagramItem*)), this, SLOT(createPhysicalModelDiagramWindow(PhysicalModelDiagramItem*)));
  connect(item, SIGNAL(exportRequested()), this, SLOT(exportPythonScript()));
  connect(item, SIGNAL(saveRequested(OTStudyItem*)), this, SLOT(save(OTStudyItem*)));
  connect(item, SIGNAL(saveAsRequested(OTStudyItem*)), this, SLOT(saveAs(OTStudyItem*)));
  connect(item, SIGNAL(closeRequested(OTStudyItem*)), this, SLOT(close(OTStudyItem*)));

  // window
  OTStudyWindow * window = new OTStudyWindow(item, mainWidget_);

  updateView(window);
}


void StudyManager::createDataModelDiagramWindow(DataModelDiagramItem* item)
{
  if (!item)
    return;

  // connections
  connect(item, SIGNAL(showErrorMessageRequested(QString)), this, SLOT(showErrorMessage(QString)));
  connect(item, SIGNAL(changeCurrentItemRequested(QModelIndex)), mainWidget_->getStudyTree(), SLOT(setCurrentIndex(QModelIndex)));
  connect(item, SIGNAL(modelDefinitionWindowRequested(DataModelDefinitionItem*)), this, SLOT(createDataModelWindow(DataModelDefinitionItem*)));
  connect(item, SIGNAL(analysisItemCreated(AnalysisItem*)), this, SLOT(createAnalysisWindow(AnalysisItem*)));
  connect(item, SIGNAL(analysisRequested(OTguiItem*, Analysis, bool)), this, SLOT(openAnalysisWizard(OTguiItem*, Analysis, bool)));

  // window
  DataModelDiagramWindow * window = new DataModelDiagramWindow(item, mainWidget_);

  updateView(window);
}


void StudyManager::createDataModelWindow(DataModelDefinitionItem* item)
{
  if (!item)
    return;

  // connections
  connect(item, SIGNAL(showErrorMessageRequested(QString)), this, SLOT(showErrorMessage(QString)));
  connect(item, SIGNAL(analysisRequested(OTguiItem*, Analysis)), this, SLOT(openAnalysisWizard(OTguiItem*, Analysis)));

  // window
  DataModelWindow * window = new DataModelWindow(item, mainWidget_);

  updateView(window);
}


void StudyManager::createPhysicalModelDiagramWindow(PhysicalModelDiagramItem* item)
{
  if (!item)
    return;

  // connections
  connect(item, SIGNAL(showErrorMessageRequested(QString)), this, SLOT(showErrorMessage(QString)));
  connect(item, SIGNAL(changeCurrentItemRequested(QModelIndex)), mainWidget_->getStudyTree(), SLOT(setCurrentIndex(QModelIndex)));
  connect(item, SIGNAL(modelDefinitionWindowRequested(PhysicalModelDefinitionItem*)), this, SLOT(createPhysicalModelWindow(PhysicalModelDefinitionItem*)));
  connect(item, SIGNAL(probabilisticModelItemCreated(ProbabilisticModelItem*)), this, SLOT(createProbabilisticModelWindow(ProbabilisticModelItem*)));
  connect(item, SIGNAL(analysisItemCreated(AnalysisItem*)), this, SLOT(createAnalysisWindow(AnalysisItem*)));
  connect(item, SIGNAL(limitStateCreated(LimitStateItem*)), this, SLOT(createLimitStateWindow(LimitStateItem*)));
  connect(item, SIGNAL(designOfExperimentEvaluationRequested(Analysis, bool)), this, SLOT(openDesignOfExperimentEvaluationWizard(Analysis, bool)));
  connect(item, SIGNAL(doeAnalysisItemCreated(DesignOfExperimentDefinitionItem*)), this, SLOT(createDesignOfExperimentWindow(DesignOfExperimentDefinitionItem*)));
  connect(item, SIGNAL(analysisRequested(OTguiItem*, Analysis, bool)), this, SLOT(openAnalysisWizard(OTguiItem*, Analysis, bool)));

  // window
  PhysicalModelDiagramWindow * window = new PhysicalModelDiagramWindow(item, mainWidget_);

  updateView(window);
}


void StudyManager::createPhysicalModelWindow(PhysicalModelDefinitionItem* item)
{
  if (!item)
    return;

  // connections
  connect(item, SIGNAL(showErrorMessageRequested(QString)), this, SLOT(showErrorMessage(QString)));
  connect(item, SIGNAL(analysisRequested(OTguiItem*, const Analysis&)), this, SLOT(openAnalysisWizard(OTguiItem*, const Analysis&)));

  // window
  OTguiSubWindow * window = WindowFactory::GetPhysicalModelWindow(item, mainWidget_);

  updateView(window);
}


void StudyManager::createProbabilisticModelWindow(ProbabilisticModelItem* item)
{
  if (!item || !item->getParentOTStudyItem())
    return;

  // connections
  connect(item, SIGNAL(showErrorMessageRequested(QString)), this, SLOT(showErrorMessage(QString)));
  connect(item, SIGNAL(analysisRequested(OTguiItem*, const Analysis&)), this, SLOT(openAnalysisWizard(OTguiItem*, const Analysis&)));

  // window
  ProbabilisticModelWindow * window = new ProbabilisticModelWindow(item, mainWidget_);

  updateView(window);
}


void StudyManager::createLimitStateWindow(LimitStateItem* item)
{
  if (!item)
    return;

  // connections
  connect(item, SIGNAL(analysisRequested(OTguiItem*, const Analysis&)), this, SLOT(openAnalysisWizard(OTguiItem*, const Analysis&)));
  connect(item, SIGNAL(showErrorMessageRequested(QString)), this, SLOT(showErrorMessage(QString)));
  connect(item, SIGNAL(analysisItemCreated(AnalysisItem*)), this, SLOT(createAnalysisWindow(AnalysisItem*)));

  // window
  LimitStateWindow * window = new LimitStateWindow(item, mainWidget_);

  updateView(window);
}


void StudyManager::createDesignOfExperimentWindow(DesignOfExperimentDefinitionItem* item, const bool createConnections)
{
  if (!item)
    return;

  item->emitRemoveWindowRequested();

  // connections
  if (createConnections)
  {
    connect(item, SIGNAL(showErrorMessageRequested(QString)), this, SLOT(showErrorMessage(QString)));
    connect(item, SIGNAL(modifyAnalysisRequested(DesignOfExperimentDefinitionItem*)), this, SLOT(modifyDesignOfExperiment(DesignOfExperimentDefinitionItem*)));
    connect(item, SIGNAL(analysisItemCreated(AnalysisItem*)), this, SLOT(createAnalysisWindow(AnalysisItem*)));
    connect(item, SIGNAL(analysisRequested(OTguiItem*, Analysis)), this, SLOT(openAnalysisWizard(OTguiItem*, Analysis)));
    connect(item, SIGNAL(DOEEvaluationRequested(Analysis)), this, SLOT(openDesignOfExperimentEvaluationWizard(Analysis)));
    connect(item, SIGNAL(updateEvaluationWindowRequested(AnalysisItem*, bool)), this, SLOT(createAnalysisWindow(AnalysisItem*, bool)));
  }

  // window
  DesignOfExperimentInputWindow * window = new DesignOfExperimentInputWindow(item, mainWidget_);

  updateView(window);
}


void StudyManager::createAnalysisWindow(AnalysisItem* item, const bool createConnections)
{
  if (!item)
    return;

  if (createConnections)
  {
    // connections
    connect(item, SIGNAL(showErrorMessageRequested(QString)), this, SLOT(showErrorMessage(QString)));
    connect(item, SIGNAL(analysisInProgressStatusChanged(bool)), this, SLOT(setAnalysisInProgress(bool)));
    connect(item, SIGNAL(analysisFinished(AnalysisItem*, bool)), this, SLOT(createAnalysisWindow(AnalysisItem*, bool)));
    connect(item, SIGNAL(modifyAnalysisRequested(AnalysisItem*)), this, SLOT(modifyAnalysis(AnalysisItem*)));
    connect(item, SIGNAL(modifyDesignOfExperimentEvaluation(Analysis, bool)), this, SLOT(openDesignOfExperimentEvaluationWizard(Analysis, bool)));
  }

  // do removeSubWindow if the analysis run method has been launched from a Python script
  item->emitRemoveWindowRequested();

  OTguiSubWindow * window = 0;
  QString message = "";

  // if analysis has valid result
  if (item->getAnalysis().hasValidResult())
  {
    try
    {
      window = WindowFactory::GetAnalysisWindow(item, mainWidget_);
      updateView(window);
    }
    catch (std::exception& ex)
    {
      qDebug() << "Error when building the analysis window : " << ex.what();
      message = tr("Impossible to create a result window");
    }
  }

  // if analysis type not recognized
  // if error when building window
  // if analysis does not have valid result
  if (!window)
  {
    AnalysisWindow * analysisWindow = new AnalysisWindow(item, analysisInProgress_, mainWidget_);
    if (!message.isEmpty())
      analysisWindow->setErrorMessage(message);
    connect(this, SIGNAL(analysisInProgressStatusChanged(bool)), analysisWindow, SLOT(updateRunButtonAvailability(bool)));
    updateView(analysisWindow);
  }
}


void StudyManager::modifyDesignOfExperiment(DesignOfExperimentDefinitionItem* item)
{
  if (!item)
    return;

  AnalysisWizard * wizard = WindowFactory::GetAnalysisWizard(item->getAnalysis(), false, mainWidget_);

  if (wizard && wizard->exec())
  {
    item->updateAnalysis(wizard->getAnalysis());
    createDesignOfExperimentWindow(item, false);
    delete wizard;
  }
}


void StudyManager::modifyAnalysis(AnalysisItem* item)
{
  if (!item)
    return;

  AnalysisWizard * wizard = WindowFactory::GetAnalysisWizard(item->getAnalysis(), false, mainWidget_);

  if (wizard && wizard->exec())
  {
    item->updateAnalysis(wizard->getAnalysis());
    createAnalysisWindow(item, false);
    delete wizard;
  }
}


void StudyManager::importPythonScript()
{
  if (mainWidget_->getStudyTree()->model()->rowCount())
  {
    int ret = QMessageBox::warning(mainWidget_,
                                   tr("Warning"),
                                   tr("Cannot import a Python script when other studies are opened.\nDo you want to continue and close the other studies?"),
                                   QMessageBox::Cancel | QMessageBox::Ok,
                                   QMessageBox::Ok);
    if (ret == QMessageBox::Ok)
    {
      if (!closeAll())
        return;
    }
    else
      return;
  }

  const QString fileName = QFileDialog::getOpenFileName(mainWidget_,
                           tr("Import Python..."),
                           FileTools::GetCurrentDir(),
                           tr("Python source files (*.py)"));

  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    FileTools::SetCurrentDir(fileName);

    // check
    if (!file.open(QFile::ReadOnly))
    {
      QMessageBox::warning(mainWidget_,
                           tr("Warning"),
                           tr("Cannot read file %1:\n%2").arg(fileName).arg(file.errorString()));
    }
    else
    {
      const QString command("exec(open(u\"" + fileName.toUtf8() + "\", encoding=\"utf-8\").read())");
      emit commandExecutionRequested(command);
    }
  }
}


void StudyManager::exportPythonScript()
{
  OTStudyItem * item = qobject_cast<OTStudyItem *>(sender());

  if (!item)
  {
    qDebug() << "StudyManager::exportPythonScript : item NULL\n";
    showErrorMessage(tr("Can not export the current study"));
    return;
  }

  const QString fileName = QFileDialog::getSaveFileName(mainWidget_,
                           tr("Export Python..."),
                           FileTools::GetCurrentDir() + QDir::separator() + item->data(Qt::DisplayRole).toString(),
                           tr("Python source files (*.py)"));

  if (!fileName.isEmpty())
    item->exportPythonScript(fileName);
}


void StudyManager::saveCurrent()
{
  OTguiItem * item = mainWidget_->getStudyTree()->getCurrentItem();
  if (!item)
  {
    qDebug() << "StudyManager::saveCurrent : item NULL\n";
    return;
  }

  save(item->getParentOTStudyItem());
}


void StudyManager::saveAsCurrent()
{
  OTguiItem * item = mainWidget_->getStudyTree()->getCurrentItem();
  if (!item)
  {
    qDebug() << "StudyManager::saveCurrent : item NULL\n";
    return;
  }

  saveAs(item->getParentOTStudyItem());
}


bool StudyManager::save(OTStudyItem* studyItem)
{
  if (!studyItem)
  {
    qDebug() << "StudyManager::save : item NULL\n";
    showErrorMessage(tr("Can not save the current study"));
    return false;
  }
  const QFileInfo file(QString::fromUtf8(studyItem->getOTStudy().getFileName().c_str()));
  if (file.exists())
  {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    studyItem->getOTStudy().save(file.absoluteFilePath().toUtf8().data());
    QApplication::restoreOverrideCursor();
    return true;
  }
  else
  {
    return saveAs(studyItem);
  }
}


bool StudyManager::saveAs(OTStudyItem* studyItem)
{
  if (!studyItem)
  {
    qDebug() << "StudyManager::saveAs : item NULL\n";
    showErrorMessage(tr("Can not save the current study as"));
    return false;
  }

  const QString fileName = QFileDialog::getSaveFileName(mainWidget_,
                           tr("Save OTStudy..."),
                           FileTools::GetCurrentDir() + QDir::separator() + studyItem->data(Qt::DisplayRole).toString(),
                           tr("XML files (*.xml)"));

  const bool ret = studyItem->save(fileName);
  if (ret)
    emit recentFilesListChanged(fileName);
  return ret;
}


void StudyManager::open(const QString& recentFileName)
{
  QString fileName = recentFileName;

  // if signal from a button (and not open a recent file)
  if (fileName.isEmpty())
  {
    fileName = QFileDialog::getOpenFileName(mainWidget_,
                                            tr("Open an existing OTStudy"),
                                            FileTools::GetCurrentDir(),
                                            tr("XML files (*.xml)"));
    if (fileName.isEmpty())
      return;
  }

  const QFileInfo file(fileName);

  // check file
  if (!file.exists())
  {
    showErrorMessage(tr("The file '%1' does not exist.").arg(fileName));
    return;
  }
  else if (OTStudy::GetFileNames().contains(file.absoluteFilePath().toUtf8().constData()))
  {
    showErrorMessage(tr("The file '%1' is already opened.").arg(fileName));
    return;
  }

  // open study
  QApplication::setOverrideCursor(Qt::WaitCursor);
  try
  {
    OTStudy newStudy(OTStudy::Open(fileName.toUtf8().constData()));
    OTStudy::Add(newStudy);
    emit recentFilesListChanged(fileName);
  }
  catch (std::exception & ex)
  {
    showErrorMessage(tr("An error has occurred when reading the file '%1'. \nMaybe objects are not opened.\n").arg(fileName) + ex.what());
  }
  QApplication::restoreOverrideCursor();

  // update QSettings
  FileTools::SetCurrentDir(fileName);
}


bool StudyManager::close(OTStudyItem* studyItem)
{
  if (!studyItem)
  {
    qDebug() << "StudyManager::close : item NULL\n";
    showErrorMessage(tr("Can not close the current study"));
    return false;
  }

  // check if an analysis is running
  if (studyItem->analysisInProgress())
  {
    showErrorMessage(tr("Can not remove a study when one of its analyses is running."));
    return false;
  }

  // if study has been modified: ask to save it before closing it
  if (studyItem->getOTStudy().getImplementation().get()->hasBeenModified())
  {
    const int ret = QMessageBox::warning(mainWidget_,
                                         tr("Warning"),
                                         tr("Do you want to save the OTStudy '%1' [%2]?").arg(studyItem->getOTStudy().getName().c_str())
                                         .arg(studyItem->getOTStudy().getFileName().c_str()),
                                         QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                         QMessageBox::Save);
    if (ret == QMessageBox::Cancel)
    {
      return false;
    }
    else if (ret == QMessageBox::Save)
    {
      if (!save(studyItem))
      {
        return false;
      }
    }
  }
  // remove instance
  OTStudy::Remove(studyItem->getOTStudy());

  return true;
}


bool StudyManager::closeCurrent()
{
  OTguiItem * item = mainWidget_->getStudyTree()->getCurrentItem();
  if (!item)
  {
    qDebug() << "StudyManager::closeCurrent : item NULL\n";
    return false;
  }

  return close(item->getParentOTStudyItem());
}


bool StudyManager::closeAll()
{
  while (mainWidget_->getStudyTree()->model()->hasChildren())
  {
    mainWidget_->getStudyTree()->setCurrentIndex(mainWidget_->getStudyTree()->model()->index(0, 0));

    if (!closeCurrent())
      return false;
  }
  return true;
}
}
