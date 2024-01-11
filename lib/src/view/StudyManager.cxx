//                                               -*- C++ -*-
/**
 *  @brief QObject managing study objects
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#include "persalys/StudyManager.hxx"

#include "persalys/WindowFactory.hxx"

#include "persalys/AnalysisWindow.hxx"
#include "persalys/FileTools.hxx"
#include "persalys/TranslationManager.hxx"

#include "persalys/DesignOfExperimentEvaluationWizard.hxx"
#include "persalys/ExtractDataFieldWizard.hxx"
#include "persalys/ObservationsWizard.hxx"
#include "persalys/MetaModelExportWizard.hxx"
#include "persalys/PolynomialRegressionAnalysis.hxx"
#include "persalys/KrigingAnalysis.hxx"
#include "persalys/FunctionalChaosAnalysis.hxx"

#include <QFileDialog>
#include <QApplication>
#include <QMessageBox>
#include <QDebug>

using namespace OT;

namespace PERSALYS
{

StudyManager::StudyManager(MainWidget * mainWidget, QObject * parent)
  : QObject(parent)
  , mainWidget_(mainWidget)
{
  connect(mainWidget_->getStudyTree(), SIGNAL(windowRequested(Item*)), this, SLOT(createWindow(Item*)));

  connect(mainWidget_->getActions()->newAction(), SIGNAL(triggered()), this, SLOT(createStudy()));
  connect(mainWidget_->getActions()->openAction(), SIGNAL(triggered()), this, SLOT(open()));
  connect(mainWidget_->getActions()->saveAction(), SIGNAL(triggered()), this, SLOT(saveCurrent()));
  connect(mainWidget_->getActions()->saveAsAction(), SIGNAL(triggered()), this, SLOT(saveAsCurrent()));
  connect(mainWidget_->getActions()->closeAction(), SIGNAL(triggered()), this, SLOT(closeCurrent()));
  connect(mainWidget_->getActions()->importPyAction(), SIGNAL(triggered()), this, SLOT(importPythonScript()));
}


void StudyManager::showErrorMessage(const QString &message)
{
  QMessageBox::critical(mainWidget_, tr("Error"), message);
}


void StudyManager::updateView(SubWindow * window)
{
  if (!window || !window->getItem())
  {
    qDebug() << "Error in StudyManager::updateView : empty pointer";
    return;
  }
  mainWidget_->getSubWindowsStackedWidget()->addSubWindow(window);
  mainWidget_->getStudyTree()->setCurrentIndex(window->getItem()->index());
  mainWidget_->getStudyTree()->setExpanded(window->getItem()->index(), true);
}


void StudyManager::createStudy()
{
  // create a new study with an available name
  Study study(Study::GetAvailableName(tr("Study_").toStdString()));
  Study::Add(study);
}


// --------------- WIZARD CREATION ---------------


void StudyManager::openAnalysisWizard(StudyItem *item, const Analysis &analysis, const bool isGeneralWizard)
{
  Q_ASSERT(item);

  String errorMessage = "";
  if (!analysis.canBeLaunched(errorMessage))
  {
    showErrorMessage(TranslationManager::GetTranslatedErrorMessage(errorMessage));
    return;
  }

  AnalysisWizard * wizard = WindowFactory::GetAnalysisWizard(analysis, isGeneralWizard, mainWidget_);

  if (wizard)
  {
    if (wizard->exec())
      item->getStudy().add(wizard->getAnalysis());
    delete wizard;
  }
}


void StudyManager::modifyAnalysis(AnalysisItem* item)
{
  Q_ASSERT(item);
  AnalysisWizard * wizard = WindowFactory::GetAnalysisWizard(item->getAnalysis(), false, mainWidget_);

  if (wizard)
  {
    if (wizard->exec())
      item->updateAnalysis(wizard->getAnalysis());
    delete wizard;
  }
}


void StudyManager::openDesignOfExperimentEvaluationWizard(const Analysis& analysis, const bool isGeneralWizard)
{
  DesignOfExperimentEvaluationWizard * wizard = new DesignOfExperimentEvaluationWizard(analysis, isGeneralWizard, mainWidget_);

  if (wizard)
  {
    if (wizard->exec())
      wizard->getAnalysis().getImplementation().get()->notify("EvaluationItemRequested");
    delete wizard;
  }
}


void StudyManager::openObservationsWizard(StudyItem *item, const DesignOfExperiment &designOfExp)
{
  Q_ASSERT(item);
  ObservationsWizard * wizard = new ObservationsWizard(designOfExp, mainWidget_);

  if (wizard)
  {
    if (wizard->exec())
      item->getStudy().add(wizard->getDesignOfExperiment());
    delete wizard;
  }
}


void StudyManager::openExtractDataFieldWizard(StudyItem *item, const Analysis &analysis)
{
  Q_ASSERT(item);
  ExtractDataFieldWizard * wizard = new ExtractDataFieldWizard(analysis, mainWidget_);

  if (wizard)
  {
    if (wizard->exec())
      item->getStudy().add(wizard->getDataModel());
    delete wizard;
  }
}

void StudyManager::openMetamodelExportWizard(StudyItem *item, const Analysis& analysis, const bool isGeneralWizard)
{
  MetaModelExportWizard * wizard = new MetaModelExportWizard(analysis, isGeneralWizard, mainWidget_);

  if (wizard) {
    if (wizard->exec()) {
      FunctionalChaosAnalysis * chaos = dynamic_cast<FunctionalChaosAnalysis*>(wizard->getAnalysis().getImplementation().get());
      if (chaos)
        item->appendMetaModelItem(chaos->getResult().getMetaModel());
      KrigingAnalysis * kriging = dynamic_cast<KrigingAnalysis*>(wizard->getAnalysis().getImplementation().get());
      if(kriging)
        item->appendMetaModelItem(kriging->getResult().getMetaModel());
      PolynomialRegressionAnalysis * linear = dynamic_cast<PolynomialRegressionAnalysis*>(wizard->getAnalysis().getImplementation().get());
      if (linear)
	item->appendMetaModelItem(linear->getResult().getMetaModel());
    }
    delete wizard;
  }
}


// --------------- WINDOW CREATION ---------------


void StudyManager::createWindow(Item *item)
{
  if (!item)
    return;

  // connections
  connect(item, SIGNAL(showErrorMessageRequested(QString)), this, SLOT(showErrorMessage(QString)));
  connect(item, SIGNAL(changeCurrentItemRequested(QModelIndex)), mainWidget_->getStudyTree(), SLOT(setCurrentIndex(QModelIndex)));

  connect(item, SIGNAL(wizardRequested(StudyItem*, Analysis, bool)), this, SLOT(openAnalysisWizard(StudyItem*, Analysis, bool)));
  connect(item, SIGNAL(doeEvaluationWizardRequested(Analysis, bool)), this, SLOT(openDesignOfExperimentEvaluationWizard(Analysis, bool)));
  connect(item, SIGNAL(mmExportWizardRequested(StudyItem*, Analysis, bool)), this, SLOT(openMetamodelExportWizard(StudyItem*, Analysis, bool)));
  connect(item, SIGNAL(wizardRequested(StudyItem*, DesignOfExperiment)), this, SLOT(openObservationsWizard(StudyItem*, DesignOfExperiment)));

  connect(item, SIGNAL(windowRequested(Item*)), this, SLOT(createWindow(Item*)));
  connect(item, SIGNAL(windowRequested(AnalysisItem*)), this, SLOT(createAnalysisWindow(AnalysisItem*)));

  connect(item, SIGNAL(exportRequested()), this, SLOT(exportPythonScript()));
  connect(item, SIGNAL(saveRequested(StudyItem*)), this, SLOT(save(StudyItem*)));
  connect(item, SIGNAL(saveAsRequested(StudyItem*)), this, SLOT(saveAs(StudyItem*)));
  connect(item, SIGNAL(closeRequested(StudyItem*)), this, SLOT(close(StudyItem*)));

  // window
  SubWindow * window = WindowFactory::GetWindow(item, mainWidget_);
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
    connect(item, SIGNAL(modifyAnalysisRequested(AnalysisItem*)), this, SLOT(modifyAnalysis(AnalysisItem*)));
    connect(item, SIGNAL(dataExtractionWizardRequested(StudyItem*, Analysis)), this, SLOT(openExtractDataFieldWizard(StudyItem*, Analysis)));
    connect(item, SIGNAL(windowRequested(AnalysisItem*, bool)), this, SLOT(createAnalysisWindow(AnalysisItem*, bool)));
    connect(item, SIGNAL(wizardRequested(StudyItem*, Analysis)), this, SLOT(openAnalysisWizard(StudyItem*, Analysis)));
    connect(item, SIGNAL(doeEvaluationWizardRequested(Analysis)), this, SLOT(openDesignOfExperimentEvaluationWizard(Analysis)));
    connect(item, SIGNAL(pythonMetamodelExportRequested(PhysicalModel)), this, SLOT(exportMetamodelPython(PhysicalModel)));
  }

  // do removeSubWindow if the analysis run method has been launched from a Python script
  item->emitRemoveWindowRequested();

  SubWindow * window = 0;
  QString message = "";

  try
  {
    window = WindowFactory::GetAnalysisWindow(item, mainWidget_);
    if (window)
      updateView(window);
  }
  catch (std::exception& ex)
  {
    qDebug() << "Error when building the analysis window : " << ex.what();
    message = tr("Impossible to create a result window");
  }

  // if analysis type not recognized
  // if error when building window
  // if analysis does not have valid result
  if (!window)
  {
    AnalysisWindow * analysisWindow = new AnalysisWindow(item, this, mainWidget_);
    if (!message.isEmpty())
      analysisWindow->setErrorMessage(message);
    updateView(analysisWindow);
  }
}


// --------------- IMPORT/EXPORT FILE ---------------

bool StudyManager::analysisInProgress() const
{
  for (int i = 0; i < mainWidget_->getStudyTree()->model()->rowCount(); ++i)
    if (mainWidget_->getStudyTree()->getItem(i)->analysisInProgress())
      return true;
  return false;
}


void StudyManager::importPythonScript(const QString &fileToImport)
{
  // check if an analysis is running
  if (analysisInProgress())
  {
    showErrorMessage("Cannot import a Python script when an analysis is running.");
    return;
  }
  // if there are studies in the tree view : propose to close them
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

  QString fileName = fileToImport;
  if (fileName.isEmpty())
    fileName = QFileDialog::getOpenFileName(mainWidget_,
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
      const QString command("__file__ = \"" + fileName.toUtf8() + "\"; exec(open(u\"" + fileName.toUtf8() + "\", encoding=\"utf-8\").read())");
      emit commandExecutionRequested(command);
    }
  }
}


void StudyManager::exportPythonScript()
{
  StudyItem * item = qobject_cast<StudyItem *>(sender());

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


void StudyManager::exportMetamodelPython(const PhysicalModel & metamodel)
{
  AnalysisItem * item = qobject_cast<AnalysisItem *>(sender());

  if (!item)
  {
    qDebug() << "StudyManager::exportMetamodelPython : item NULL\n";
    showErrorMessage(tr("Can not export the current study"));
    return;
  }

  const QString fileName = QFileDialog::getSaveFileName(mainWidget_,
                           tr("Export Python..."),
                           FileTools::GetCurrentDir() + QDir::separator() + item->data(Qt::DisplayRole).toString() + QString(".py"),
                           tr("Python source files (*.py)"));

  if (!fileName.isEmpty())
    metamodel.exportStandalonePythonScript(fileName.toStdString());
}

// --------------- SAVE FILE ---------------

void StudyManager::saveCurrent()
{
  Item * item = mainWidget_->getStudyTree()->getCurrentItem();
  if (!item)
  {
    qDebug() << "StudyManager::saveCurrent : item NULL\n";
    return;
  }

  save(item->getParentStudyItem());
}


void StudyManager::saveAsCurrent()
{
  Item * item = mainWidget_->getStudyTree()->getCurrentItem();
  if (!item)
  {
    qDebug() << "StudyManager::saveCurrent : item NULL\n";
    return;
  }

  saveAs(item->getParentStudyItem());
}


bool StudyManager::save(StudyItem* studyItem)
{
  if (!studyItem)
  {
    qDebug() << "StudyManager::save : item NULL\n";
    showErrorMessage(tr("Can not save the current study"));
    return false;
  }
  const QFileInfo file(QString::fromUtf8(studyItem->getStudy().getFileName().c_str()));
  if (file.exists())
  {
    studyItem->save(QDir::toNativeSeparators(file.absoluteFilePath()).toUtf8().data());
    emit recentFilesListChanged(file.absoluteFilePath().toUtf8().data());
    return true;
  }
  else
  {
    return saveAs(studyItem);
  }
}


bool StudyManager::saveAs(StudyItem* studyItem)
{
  if (!studyItem)
  {
    qDebug() << "StudyManager::saveAs : item NULL\n";
    showErrorMessage(tr("Can not save the current study as"));
    return false;
  }

  const QString fileName = QFileDialog::getSaveFileName(mainWidget_,
                           tr("Save study..."),
                           FileTools::GetCurrentDir() + QDir::separator() + studyItem->data(Qt::DisplayRole).toString(),
                           tr("XML files (*.xml)"));

  const bool ret = studyItem->save(fileName);
  if (ret)
    emit recentFilesListChanged(fileName);
  return ret;
}


// --------------- OPEN FILE ---------------

void StudyManager::open(const QString& recentFileName)
{
  QString fileName = recentFileName;

  // if signal from a button (and not open a recent file)
  if (fileName.isEmpty())
  {
    fileName = QFileDialog::getOpenFileName(mainWidget_,
                                            tr("Open an existing study"),
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
  else if (Study::GetFileNames().contains(file.absoluteFilePath().toUtf8().constData()))
  {
    showErrorMessage(tr("The file '%1' is already opened.").arg(fileName));
    return;
  }

  // open study
  QApplication::setOverrideCursor(Qt::WaitCursor);
  try
  {
    Study newStudy(Study::Open(fileName.toUtf8().constData()));
    Study::Add(newStudy);
    emit recentFilesListChanged(fileName);
    QApplication::restoreOverrideCursor();
  }
  catch (std::exception & ex)
  {
    QApplication::restoreOverrideCursor();
    showErrorMessage(tr("An error has occurred when reading the file '%1'. \nMaybe objects are not opened.\n").arg(fileName) + ex.what());
  }

  // update QSettings
  FileTools::SetCurrentDir(fileName);
}


// --------------- CLOSE FILE ---------------

bool StudyManager::close(StudyItem* studyItem)
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
  if (studyItem->getStudy().getImplementation().get()->hasBeenModified())
  {
    const int ret = QMessageBox::warning(mainWidget_,
                                         tr("Warning"),
                                         tr("Do you want to save the study '%1' [%2]?").arg(studyItem->getStudy().getName().c_str())
                                         .arg(studyItem->getStudy().getFileName().c_str()),
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
  Study::Remove(studyItem->getStudy());

  return true;
}


bool StudyManager::closeCurrent()
{
  Item * item = mainWidget_->getStudyTree()->getCurrentItem();
  if (!item)
  {
    qDebug() << "StudyManager::closeCurrent : item NULL\n";
    return false;
  }

  return close(item->getParentStudyItem());
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
