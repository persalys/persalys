//                                               -*- C++ -*-
/**
 *  @brief QTreeView to list studies
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
#include "otgui/StudyTreeView.hxx"

#include "otgui/OTStudyWindow.hxx"
#include "otgui/DataAnalysisResultWindow.hxx"
#include "otgui/DataModelDiagramWindow.hxx"
#include "otgui/DesignOfExperimentInputWindow.hxx"
#include "otgui/DesignOfExperimentEvaluationWizard.hxx"
#include "otgui/PhysicalModelDiagramWindow.hxx"
#ifdef OTGUI_HAVE_YACS
#include "otgui/YACSPhysicalModelWindow.hxx"
#endif
#ifdef OTGUI_HAVE_OTFMI
#include "otgui/FMIPhysicalModel.hxx"
#include "otgui/FMIPhysicalModelWindow.hxx"
#endif
#include "otgui/SymbolicPhysicalModelWindow.hxx"
#include "otgui/PythonPhysicalModelWindow.hxx"
#include "otgui/MetaModelWindow.hxx"
#include "otgui/ProbabilisticModelWindow.hxx"
#include "otgui/LimitStateWindow.hxx"
#include "otgui/DataModelWindow.hxx"
#include "otgui/DesignOfExperimentWindow.hxx"
#include "otgui/AnalysisWindow.hxx"
#include "otgui/ModelEvaluationResultWindow.hxx"
#include "otgui/MonteCarloResultWindow.hxx"
#include "otgui/TaylorExpansionMomentsResultWindow.hxx"
#include "otgui/SobolResultWindow.hxx"
#include "otgui/SRCResultWindow.hxx"
#include "otgui/SimulationReliabilityResultWindow.hxx"
#include "otgui/ApproximationResultWindow.hxx"
#include "otgui/FunctionalChaosResultWindow.hxx"
#include "otgui/KrigingResultWindow.hxx"
#include "otgui/InferenceResultWindow.hxx"
#include "otgui/CopulaInferenceResultWindow.hxx"

#include "otgui/InferenceWizard.hxx"
#include "otgui/DesignOfExperimentWizard.hxx"
#include "otgui/ModelEvaluationWizard.hxx"
#include "otgui/CentralTendencyWizard.hxx"
#include "otgui/SensitivityAnalysisWizard.hxx"
#include "otgui/ReliabilityAnalysisWizard.hxx"
#include "otgui/MetaModelAnalysisWizard.hxx"

#include "otgui/LineEditWithQValidatorDelegate.hxx"

#include <QMenu>
#include <QFileDialog>
#include <QApplication>
#include <QMessageBox>
#include <QSettings>
#include <QHeaderView>

#include <iostream>

using namespace OT;

namespace OTGUI {

// define QStyledItemDelegate
class TreeItemDelegate : public LineEditWithQValidatorDelegate
{
public:
  TreeItemDelegate(QObject* parent)
    : LineEditWithQValidatorDelegate(QString("[a-zA-Z_][a-zA-Z_0-9]*"), parent)
  {
  }


  void paint(QPainter* painter,
             const QStyleOptionViewItem& option,
             const QModelIndex& index) const
  {
    painter->save();

    QStyleOptionViewItem optionButton = option;
    initStyleOption(&optionButton, index);

#if QT_VERSION >= 0x050000
    // use optionButton.widget to keep information from style sheet
    if (optionButton.widget)
      optionButton.widget->style()->drawControl(QStyle::CE_ItemViewItem, &optionButton, painter, optionButton.widget);
    else
      QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &optionButton, painter);
#else
      QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &optionButton, painter);
#endif

    // draw a line at the bottom of the OTStudyItem
    if (index.data(Qt::UserRole).toString().contains("OTStudy"))
    {
      QLineF aLine(optionButton.rect.bottomLeft(), optionButton.rect.bottomRight());
      QPen pen("#0a5205");
      pen.setWidth(1);
      painter->setPen(pen);
      painter->drawLine(aLine);
    }

    painter->restore();
  }
};


StudyTreeView::StudyTreeView(QWidget * parent)
  : QTreeView(parent)
  , treeViewModel_(new StudyTreeViewModel(this))
  , analysisInProgress_(false)
{
  // set model
  OTStudy::SetInstanceObserver(treeViewModel_);
  setModel(treeViewModel_);
  connect(treeViewModel_, SIGNAL(newOTStudyCreated(OTStudyItem*)), this, SLOT(createNewOTStudyWindow(OTStudyItem*)));
  connect(treeViewModel_, SIGNAL(otStudySubItemsAdded(OTStudyItem*)), this, SLOT(modifyStudySubItemsExpansion(OTStudyItem*)));

  // forbid the user to define not valid item's name
#if QT_VERSION >= 0x050000
  // draw a line at the bottom of the OTStudyItem
  setItemDelegate(new TreeItemDelegate(this));
#else
  setItemDelegate(new LineEditWithQValidatorDelegate(QString("[a-zA-Z_][a-zA-Z_0-9]*")));
#endif

  // context menu
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));

  // connections
  connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(setCurrentIndex(QModelIndex)));
  connect(this->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(selectedItemChanged(QModelIndex, QModelIndex)));

  setExpandsOnDoubleClick(false);

  // style sheet
  const QString styleSheet = " QTreeView::item:selected { background-color: #a5d3a1;\
                                                          color: doubledarkgray;\
                               }\
                               QTreeView::branch:selected { background-color: #a5d3a1;\
                               }\
                               QTreeView::branch:has-children:!has-siblings:closed,\
                               QTreeView::branch:closed:has-children:has-siblings { border-image: none;\
                                                                                    image: url(:/images/branch-closed.png);\
                               }\
                               QTreeView::branch:open:has-children:!has-siblings,\
                               QTreeView::branch:open:has-children:has-siblings { border-image: none;\
                                                                                  image: url(:/images/branch-opened.png);\
                               }\
                               QTreeView::branch:has-siblings:adjoins-item { border-image: url(:/images/branch-more.png) 0;\
                               }\
                               QTreeView::branch:!has-children:!has-siblings:adjoins-item { border-image: url(:/images/branch-end.png) 0;\
                               }\
                             ";

  setStyleSheet(styleSheet);

  // style sheet for header
  const QString headerStyleSheet = " QHeaderView::section { border: 2px solid #0a5205;\
                                                            border-radius: 1px;\
                                                            background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                                                                              stop: 0 #0a5205, stop: 0.4 #4ea248,\
                                                                                              stop: 0.5 #4ea248, stop: 1.0 #0a5205);\
                                                            color: white;\
                                                            font: bold;\
                                                            padding: 5px 1px;\
                                                            outline: none;\
                                                          }\
                                   ";
  header()->setStyleSheet(headerStyleSheet);
}


void StudyTreeView::mousePressEvent(QMouseEvent* event)
{
  // get the clicked item
  const QModelIndex eventIndex = indexAt(event->pos());
  const QStandardItem * currentItem = treeViewModel_->itemFromIndex(eventIndex);

  if (currentItem)
  {
    // if it is a title: no selection possible
    if (currentItem->data(Qt::UserRole).toString().contains("Title"))
    {
      if (event->button() == Qt::LeftButton)
        setExpanded(eventIndex, !isExpanded(eventIndex));
      return;
    }
  }

  QTreeView::mousePressEvent(event);
}


void StudyTreeView::setAnalysisInProgress(bool analysisInProgress)
{
  analysisInProgress_ = analysisInProgress;

  // emit signal to MenuBar/ToolBar to disable/enable import Python script actions
  //             to AnalysisWindows to disable/enable the run buttons
  emit analysisInProgressStatusChanged(analysisInProgress);
}


void StudyTreeView::showErrorMessage(QString message)
{
  QMessageBox::critical(this, tr("Error"), message);
}


void StudyTreeView::onCustomContextMenu(const QPoint &point)
{
  const QModelIndex index = indexAt(point);

  if (!index.isValid())
    return;

  QStandardItem * currentItem = treeViewModel_->itemFromIndex(index);

  QList<QAction*> actions;

  // get actions defined in the current item
  OTguiItem * otguiItem = dynamic_cast<OTguiItem*>(currentItem);
  if (otguiItem)
    actions = otguiItem->getActions();

  if (!actions.size())
    return;

  // build the context menu
  QMenu * contextMenu = new QMenu(this);
  contextMenu->addActions(actions);
  contextMenu->exec(viewport()->mapToGlobal(point));
}


void StudyTreeView::selectedItemChanged(const QModelIndex& currentIndex, const QModelIndex& previousIndex)
{
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(currentIndex);

  if (!selectedItem)
    return;

  if (selectedItem->data(Qt::UserRole).toString().contains("Title") && selectedItem->hasChildren())
  {
    OTguiItem * otguiItem = dynamic_cast<OTguiItem*>(selectedItem);
    if (otguiItem)
    {
      setCurrentIndex(otguiItem->getParentOTStudyItem()->index());
      return;
    }
  }

  emit itemSelected(selectedItem);
}


void StudyTreeView::createNewOTStudy()
{
  OTStudy otstudy(OTStudy::GetAvailableName());
  OTStudy::Add(otstudy);
}


void StudyTreeView::createNewDesignOfExperimentEvaluation(const Analysis& analysis, const bool isGeneralWizard)
{
  DesignOfExperimentEvaluationWizard * wizard = new DesignOfExperimentEvaluationWizard(analysis, isGeneralWizard, this);

  if (wizard)
  {
    if (wizard->exec())
    {
      wizard->getDesignOfExperimentDefinitionItem()->appendEvaluationItem();
    }
  }
}


void StudyTreeView::createNewAnalysis(OTguiItem* item, const Analysis& analysis, const bool isGeneralWizard)
{
  if (!item)
    return;

  AnalysisWizard * wizard = getWizard(item, analysis, isGeneralWizard);

  if (wizard)
  {
    if (wizard->exec())
    {
      item->getParentOTStudyItem()->getOTStudy().add(wizard->getAnalysis());
      delete wizard;
      wizard = 0;
    }
  }
}


void StudyTreeView::createNewOTStudyWindow(OTStudyItem* item)
{
  if (!item)
    return;

  // connections
  connect(item, SIGNAL(emitErrorMessageRequested(QString)), this, SLOT(showErrorMessage(QString)));
  connect(item, SIGNAL(newDataModelItemCreated(DataModelDiagramItem*)), this, SLOT(createNewDataModelDiagramWindow(DataModelDiagramItem*)));
  connect(item, SIGNAL(newPhysicalModelItemCreated(PhysicalModelDiagramItem*)), this, SLOT(createNewPhysicalModelDiagramWindow(PhysicalModelDiagramItem*)));
  connect(item, SIGNAL(otStudyExportRequested()), this, SLOT(exportOTStudy()));
  connect(item, SIGNAL(otStudySaveAsRequested()), this, SLOT(saveAsOTStudy()));
  connect(item, SIGNAL(otStudySaveAsRequested(OTStudyItem*, bool*)), this, SLOT(saveAsOTStudy(OTStudyItem*, bool*)));
  connect(item, SIGNAL(otStudyCloseRequested(OTStudyItem*,bool*)), this, SLOT(closeNotSavedOTStudyRequest(OTStudyItem*,bool*)));
  connect(item, SIGNAL(recentFilesListChanged(QString)), this, SIGNAL(recentFilesListChanged(QString)));

  // window
  OTStudyWindow * window = new OTStudyWindow(item, this);

  emit showWindow(window);
  setCurrentIndex(item->index());
}


void StudyTreeView::createNewDataModelDiagramWindow(DataModelDiagramItem* item)
{
  if (!item)
    return;

  // connections
  connect(item, SIGNAL(emitErrorMessageRequested(QString)), this, SLOT(showErrorMessage(QString)));
  connect(item, SIGNAL(changeCurrentItemRequested(QModelIndex)), this, SLOT(setCurrentIndex(QModelIndex)));
  connect(item, SIGNAL(modelDefinitionWindowRequested(DataModelDefinitionItem*)), this, SLOT(createNewDataModelWindow(DataModelDefinitionItem*)));
  connect(item, SIGNAL(newAnalysisItemCreated(AnalysisItem*)), this, SLOT(createNewAnalysisWindow(AnalysisItem*)));
  connect(item, SIGNAL(analysisRequested(OTguiItem*,Analysis, bool)), this, SLOT(createNewAnalysis(OTguiItem*,Analysis, bool)));

  // window
  DataModelDiagramWindow * window = new DataModelDiagramWindow(item, this);

  if (!window)
  {
    qDebug() << "Error: In createNewDataModelDiagramWindow: impossible to create DataModelDiagramWindow\n";
    return;
  }

  emit showWindow(window);
  setCurrentIndex(item->index());
  setExpanded(item->index(), true);
}


void StudyTreeView::createNewDataModelWindow(DataModelDefinitionItem* item, const bool createConnections)
{
  if (!item)
    return;

  // connections
  if (createConnections)
  {
    connect(item, SIGNAL(emitErrorMessageRequested(QString)), this, SLOT(showErrorMessage(QString)));
    connect(item, SIGNAL(analysisRequested(OTguiItem*, Analysis)), this, SLOT(createNewAnalysis(OTguiItem*, Analysis)));
  }

  // window
  DataModelWindow * window = new DataModelWindow(item, this);

  if (!window)
  {
    qDebug() << "Error: In createNewDataModelWindow: impossible to create DataModelWindow\n";
    return;
  }

  emit showWindow(window);
  setCurrentIndex(item->index());
  setExpanded(item->index().parent(), true);
}


void StudyTreeView::createNewPhysicalModelDiagramWindow(PhysicalModelDiagramItem* item)
{
  if (!item)
    return;

  // connections
  connect(item, SIGNAL(emitErrorMessageRequested(QString)), this, SLOT(showErrorMessage(QString)));
  connect(item, SIGNAL(changeCurrentItemRequested(QModelIndex)), this, SLOT(setCurrentIndex(QModelIndex)));
  connect(item, SIGNAL(modelDefinitionWindowRequested(PhysicalModelDefinitionItem*)), this, SLOT(createNewPhysicalModelWindow(PhysicalModelDefinitionItem*)));
  connect(item, SIGNAL(newProbabilisticModelItemCreated(ProbabilisticModelItem*)), this, SLOT(createNewProbabilisticModelWindow(ProbabilisticModelItem*)));
  connect(item, SIGNAL(newAnalysisItemCreated(AnalysisItem*)), this, SLOT(createNewAnalysisWindow(AnalysisItem*)));
  connect(item, SIGNAL(newLimitStateCreated(LimitStateItem*)), this, SLOT(createNewLimitStateWindow(LimitStateItem*)));
  connect(item, SIGNAL(designOfExperimentEvaluationRequested(Analysis, bool)), this, SLOT(createNewDesignOfExperimentEvaluation(Analysis, bool)));
  connect(item, SIGNAL(newDOEAnalysisItemCreated(DesignOfExperimentDefinitionItem*)), this, SLOT(createNewDesignOfExperimentWindow(DesignOfExperimentDefinitionItem*)));
  connect(item, SIGNAL(analysisRequested(OTguiItem*, Analysis, bool)), this, SLOT(createNewAnalysis(OTguiItem*, Analysis, bool)));

  // window
  PhysicalModelDiagramWindow * window = new PhysicalModelDiagramWindow(item, this);

  if (!window)
  {
    qDebug() << "Error: In createNewPhysicalModelDiagramWindow: impossible to create PhysicalModelDiagramWindow\n";
    return;
  }

  emit showWindow(window);
  setCurrentIndex(item->index());
  setExpanded(item->index(), true);
}


void StudyTreeView::createNewPhysicalModelWindow(PhysicalModelDefinitionItem* item)
{
  if (!item)
    return;

  // connections
  connect(item, SIGNAL(emitErrorMessageRequested(QString)), this, SLOT(showErrorMessage(QString)));
  connect(item, SIGNAL(analysisRequested(OTguiItem*, const Analysis&)), this, SLOT(createNewAnalysis(OTguiItem*, const Analysis&)));

  // window
  OTguiSubWindow * window = 0;
  String physicalModelType = item->getPhysicalModel().getImplementation()->getClassName();

  if (physicalModelType == "SymbolicPhysicalModel")
    window = new SymbolicPhysicalModelWindow(item, this);
  else if (physicalModelType == "PythonPhysicalModel")
    window = new PythonPhysicalModelWindow(item, this);
  else if (physicalModelType == "MetaModel")
    window = new MetaModelWindow(item, this);
#ifdef OTGUI_HAVE_YACS
  else if (physicalModelType == "YACSPhysicalModel")
    window = new YACSPhysicalModelWindow(item, this);
#endif
#ifdef OTGUI_HAVE_OTFMI
  else if (physicalModelType == "FMIPhysicalModel")
    window = new FMIPhysicalModelWindow(item, this);
#endif
  if (!window)
  {
    qDebug() << "Error: In : impossible to create PhysicalModelWindow\n";
    return;
  }

  emit showWindow(window);
  setCurrentIndex(item->index());
  setExpanded(item->index(), true);
}


void StudyTreeView::createNewProbabilisticModelWindow(ProbabilisticModelItem* item)
{
  if (!item)
    return;

  // connections
  connect(item, SIGNAL(emitErrorMessageRequested(QString)), this, SLOT(showErrorMessage(QString)));
  connect(item, SIGNAL(analysisRequested(OTguiItem*, const Analysis&)), this, SLOT(createNewAnalysis(OTguiItem*, const Analysis&)));

  OTStudyItem * otStudyItem = item->getParentOTStudyItem();
  if (!otStudyItem)
    return;

  // window
  ProbabilisticModelWindow * window = new ProbabilisticModelWindow(otStudyItem->getOTStudy(), item, this);

  if (!window)
  {
    qDebug() << "Error: In createNewProbabilisticModelWindow: impossible to create ProbabilisticModelWindow\n";
    return;
  }

  emit showWindow(window);
  setCurrentIndex(item->index());
}


void StudyTreeView::createNewLimitStateWindow(LimitStateItem* item)
{
  if (!item)
    return;

  // connections
  connect(item, SIGNAL(analysisRequested(OTguiItem*, const Analysis&)), this, SLOT(createNewAnalysis(OTguiItem*, const Analysis&)));
  connect(item, SIGNAL(emitErrorMessageRequested(QString)), this, SLOT(showErrorMessage(QString)));
  connect(item, SIGNAL(newAnalysisItemCreated(AnalysisItem*)), this, SLOT(createNewAnalysisWindow(AnalysisItem*)));

  // window
  LimitStateWindow * window = new LimitStateWindow(item, this);

  if (!window)
  {
    qDebug() << "Error: In createNewLimitStateWindow: impossible to create LimitStateWindow\n";
    return;
  }

  emit showWindow(window);
  setCurrentIndex(item->index());
}


void StudyTreeView::createNewDesignOfExperimentWindow(DesignOfExperimentDefinitionItem* item, const bool createConnections)
{
  if (!item)
    return;

  // connections
  if (createConnections)
  {
    connect(item, SIGNAL(emitErrorMessageRequested(QString)), this, SLOT(showErrorMessage(QString)));
    connect(item, SIGNAL(modifyAnalysisRequested(DesignOfExperimentDefinitionItem*)), this, SLOT(modifyDesignOfExperiment(DesignOfExperimentDefinitionItem*)));
    connect(item, SIGNAL(newAnalysisItemCreated(AnalysisItem*)), this, SLOT(createNewAnalysisWindow(AnalysisItem*)));
    connect(item, SIGNAL(analysisRequested(OTguiItem*,Analysis)), this, SLOT(createNewAnalysis(OTguiItem*,Analysis)));
    connect(item, SIGNAL(DOEEvaluationRequested(Analysis)), this, SLOT(createNewDesignOfExperimentEvaluation(Analysis)));
    connect(item, SIGNAL(updateEvaluationWindowRequested(AnalysisItem*)), this, SLOT(modifyDesignOfExperimentEvaluationWindow(AnalysisItem*)));
  }

  // window
  DesignOfExperimentInputWindow * window = new DesignOfExperimentInputWindow(item, this);

  if (!window)
  {
    qDebug() << "Error: In createNewDesignOfExperimentWindow: impossible to create DesignOfExperimentInputWindow\n";
    return;
  }

  emit showWindow(window);
  setCurrentIndex(item->index());
  setExpanded(item->index().parent(), true);
}


void StudyTreeView::createNewAnalysisWindow(AnalysisItem * item)
{
  if (!item)
    return;

  // connections
  connect(item, SIGNAL(emitErrorMessageRequested(QString)), this, SLOT(showErrorMessage(QString)));
  connect(item, SIGNAL(analysisInProgressStatusChanged(bool)), this, SLOT(setAnalysisInProgress(bool)));
  connect(item, SIGNAL(analysisFinished(AnalysisItem *)), this, SLOT(createAnalysisResultWindow(AnalysisItem*)));
  connect(item, SIGNAL(analysisBadlyFinished(AnalysisItem*)), this, SLOT(createAnalysisWindow(AnalysisItem*)));
  connect(item, SIGNAL(modifyAnalysisRequested(AnalysisItem*)), this, SLOT(modifyAnalysis(AnalysisItem*)));
  connect(item, SIGNAL(modifyDesignOfExperimentEvaluation(Analysis, bool)), this, SLOT(createNewDesignOfExperimentEvaluation(Analysis, bool)));

  // window
  if (item->getAnalysis().analysisLaunched())
    createAnalysisResultWindow(item);
  else
    createAnalysisWindow(item);
}


void StudyTreeView::createAnalysisResultWindow(AnalysisItem* item)
{
  if (!item)
    return;

  // do removeSubWindow if the analysis run method has been launched from a Python script
  emit removeSubWindow(item);

  OTguiSubWindow * resultWindow = 0;
  const QString analysisType = item->data(Qt::UserRole).toString();

  if (analysisType == "ModelEvaluation")
    resultWindow = new ModelEvaluationResultWindow(item, this);
  else if (analysisType == "MonteCarloAnalysis")
    resultWindow = new MonteCarloResultWindow(item, this);
  else if (analysisType == "TaylorExpansionMomentsAnalysis")
    resultWindow = new TaylorExpansionMomentsResultWindow(item);
  else if (analysisType == "SobolAnalysis")
    resultWindow = new SobolResultWindow(item, this);
  else if (analysisType == "SRCAnalysis")
    resultWindow = new SRCResultWindow(item, this);
  else if (analysisType == "MonteCarloReliabilityAnalysis" ||
           analysisType == "FORMImportanceSamplingAnalysis" ||
           analysisType == "ImportanceSamplingAnalysis")
    resultWindow = new SimulationReliabilityResultWindow(item, this);
  else if (analysisType == "FORMAnalysis")
    resultWindow = new ApproximationResultWindow(item, this);
  else if (analysisType == "DataAnalysis")
    resultWindow = new DataAnalysisResultWindow(item, this);
  else if (analysisType == "FunctionalChaosAnalysis")
    resultWindow = new FunctionalChaosResultWindow(item, this);
  else if (analysisType == "KrigingAnalysis")
    resultWindow = new KrigingResultWindow(item, this);
  else if (analysisType == "InferenceAnalysis")
    resultWindow = new InferenceResultWindow(item, this);
  else if (analysisType == "CopulaInferenceAnalysis")
    resultWindow = new CopulaInferenceResultWindow(item, this);
  else if (analysisType.contains("DesignOfExperiment"))
    // if DesignOfExperimentEvaluation GridDesignOfExperiment ImportedDesignOfExperiment ProbabilisticDesignOfExperiment
    resultWindow = new DesignOfExperimentWindow(item, this);

  if (!resultWindow)
  {
    qDebug() << "Error: In createAnalysisResultWindow: analysisType " << analysisType << " not recognized.\n";
    return;
  }

  emit showWindow(resultWindow);
  setCurrentIndex(item->index());
}


void StudyTreeView::createAnalysisWindow(AnalysisItem* item)
{
  if (!item)
    return;

  // do removeSubWindow if the analysis run method has been launched from a Python script
  emit removeSubWindow(item); // need?

  AnalysisWindow * window = new AnalysisWindow(item, analysisInProgress_, this);

  if (!window)
  {
    qDebug() << "Error: In createAnalysisWindow: impossible to create AnalysisWindow\n";
    return;
  }

  connect(this, SIGNAL(analysisInProgressStatusChanged(bool)), window, SLOT(updateRunButtonAvailability(bool)));

  emit showWindow(window);
  setCurrentIndex(item->index());
}


void StudyTreeView::modifyDesignOfExperiment(DesignOfExperimentDefinitionItem* item)
{
  if (!item)
    return;

  DesignOfExperimentWizard * wizard = new DesignOfExperimentWizard(item->getAnalysis().getImplementation(), this);

  if (wizard)
  {
    if (wizard->exec())
    {
      emit removeSubWindow(item);
      item->updateAnalysis(wizard->getAnalysis());
      createNewDesignOfExperimentWindow(item, false);
      delete wizard;
      wizard = 0;
    }
  }
}


void StudyTreeView::modifyDesignOfExperimentEvaluationWindow(AnalysisItem* item)
{
  if (!item)
    return;

  // update window
  emit removeSubWindow(item);
  createAnalysisWindow(item);

  emit itemSelected(item);
}


void StudyTreeView::modifyAnalysis(AnalysisItem* item)
{
  if (!item)
    return;

  AnalysisWizard * wizard = getWizard(item, item->getAnalysis());

  if (wizard)
  {
    if (wizard->exec())
    {
      emit removeSubWindow(item);
      item->updateAnalysis(wizard->getAnalysis());
      createAnalysisWindow(item);
      delete wizard;
      wizard = 0;
    }
  }
}


void StudyTreeView::modifyStudySubItemsExpansion(OTStudyItem* item)
{
  setCurrentIndex(item->index());
  setExpanded(item->index(), true);
  for (int i = 0; i < item->rowCount(); ++i)
  {
    setExpanded(item->child(i)->index(), true);
    for (int j = 0; j < item->child(i)->rowCount(); ++j)
    {
      // collapse models items
      setExpanded(item->child(i)->child(j)->index(), false);
    }
  }
}


void StudyTreeView::exportOTStudy()
{
  OTStudyItem * item = qobject_cast<OTStudyItem *>(sender());

  if (!item)
  {
    qDebug() << "StudyTreeView::exportOTStudy : item NULL\n";
    showErrorMessage(tr("Can not export the current study"));
    return;
  }

  QSettings settings;
  QString currentDir = settings.value("currentDir").toString();
  if (currentDir.isEmpty())
    currentDir = QDir::homePath();

  const QString fileName = QFileDialog::getSaveFileName(this,
                                                        tr("Export Python..."),
                                                        currentDir + QDir::separator() + item->data(Qt::DisplayRole).toString(),
                                                        tr("Python source files (*.py)"));

  if (!fileName.isEmpty())
    item->exportOTStudy(fileName);
}


void StudyTreeView::saveCurrentOTStudy()
{
  OTguiItem * item = dynamic_cast<OTguiItem*>(treeViewModel_->itemFromIndex(selectionModel()->currentIndex()));
  if (!item)
  {
    qDebug() << "StudyTreeView::saveCurrentOTStudy : item NULL\n";
    return;
  }

  OTStudyItem * studyItem = item->getParentOTStudyItem();
  if (!studyItem)
  {
    qDebug() << "StudyTreeView::saveCurrentOTStudy : studyItem NULL\n";
    return;
  }

  studyItem->saveOTStudy();
}


void StudyTreeView::saveAsCurrentOTStudy()
{
  OTguiItem * item = dynamic_cast<OTguiItem*>(treeViewModel_->itemFromIndex(selectionModel()->currentIndex()));
  if (!item)
  {
    qDebug() << "StudyTreeView::saveCurrentOTStudy : item NULL\n";
    return;
  }

  saveAsOTStudy(item->getParentOTStudyItem());
}


void StudyTreeView::saveAsOTStudy()
{
  OTStudyItem * item = qobject_cast<OTStudyItem*>(sender());

  if (!item)
  {
    qDebug() << "StudyTreeView::saveAsOTStudy : item NULL\n";
    showErrorMessage(tr("Can not save the current study as"));
    return;
  }

  saveAsOTStudy(item);
}


void StudyTreeView::saveAsOTStudy(OTStudyItem* item, bool* notcancel)
{
  if (!item)
    return;

  QSettings settings;
  QString currentDir = settings.value("currentDir").toString();
  if (currentDir.isEmpty())
    currentDir = QDir::homePath();

  const QString fileName = QFileDialog::getSaveFileName(this,
                                                        tr("Save OTStudy..."),
                                                        currentDir + QDir::separator() + item->data(Qt::DisplayRole).toString(),
                                                        tr("XML files (*.xml)"));

  if (fileName.isEmpty())
  {
    if (notcancel)
      *notcancel = false;
    return;
  }

  item->saveOTStudy(fileName);

  if (notcancel)
    *notcancel = true;
}


void StudyTreeView::openOTStudy(const QString& recentFileName)
{
  QSettings settings;

  QString fileName = recentFileName;

  // if signal from a button (and not open a recent file)
  if (fileName.isEmpty())
  {
    QString currentDir = settings.value("currentDir").toString();
    if (currentDir.isEmpty())
      currentDir = QDir::homePath();

    fileName = QFileDialog::getOpenFileName(this,
                                            tr("Open an existing OTStudy"),
                                            currentDir,
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
    OTStudy::Open(fileName.toUtf8().constData());
    QApplication::restoreOverrideCursor();
  }
  catch (std::exception & ex)
  {
    QApplication::restoreOverrideCursor();
    showErrorMessage(tr("An error has occured when reading the file '%1'. \nMaybe objects are not opened.\n").arg(fileName) + ex.what());
  }

  emit recentFilesListChanged(fileName);

  // update QSettings
  settings.setValue("currentDir", file.absolutePath());
}


void StudyTreeView::closeNotSavedOTStudyRequest(OTStudyItem* item, bool* canClose)
{
  const int ret = QMessageBox::warning(this,
                                       tr("Warning"),
                                       tr("Do you want to save the OTStudy '%1' [%2]?").arg(item->getOTStudy().getName().c_str())
                                                                                       .arg(item->getOTStudy().getFileName().c_str()),
                                       QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                       QMessageBox::Save);
  if (ret == QMessageBox::Save)
  {
    if (!item->saveOTStudy())
      return;
  }

  if (canClose)
    *canClose = (ret != QMessageBox::Cancel);
}


bool StudyTreeView::closeOTStudy()
{
  OTguiItem * item = dynamic_cast<OTguiItem*>(treeViewModel_->itemFromIndex(selectionModel()->currentIndex()));
  if (!item)
  {
    qDebug() << "StudyTreeView::closeOTStudy : item NULL\n";
    return false;
  }

  OTStudyItem * studyItem = item->getParentOTStudyItem();
  if (!studyItem)
  {
    qDebug() << "StudyTreeView::closeOTStudy : studyItem NULL\n";
    return false;
  }

  return studyItem->closeOTStudy();
}


bool StudyTreeView::closeAllOTStudies()
{
  // get root item
  QStandardItem * rootItem = treeViewModel_->invisibleRootItem();
  if (!rootItem->hasChildren())
    return true;

  while (rootItem->hasChildren())
  {
    setCurrentIndex(rootItem->child(0)->index());
    const int ret = closeOTStudy();
    if (!ret)
      return false;
  }
  return true;
}


AnalysisWizard* StudyTreeView::getWizard(OTguiItem* item, const Analysis& analysis, const bool isGeneralWizard)
{
  if (!item)
    return 0;

  AnalysisWizard * wizard = 0;

  const String analysisName = analysis.getImplementation()->getClassName();

  if (analysisName == "ModelEvaluation")
  {
    wizard = new ModelEvaluationWizard(analysis, this);
  }
  else if (analysisName == "FunctionalChaosAnalysis" ||
           analysisName == "KrigingAnalysis")
  {
    wizard = new MetaModelAnalysisWizard(analysis, isGeneralWizard, this);
  }
  else if (analysisName == "InferenceAnalysis")
  {
    wizard = new InferenceWizard(analysis, this);
  }
  else if (analysisName == "MonteCarloAnalysis" ||
           analysisName == "TaylorExpansionMomentsAnalysis")
  {
    wizard = new CentralTendencyWizard(analysis, this);
  }
  else if (analysisName == "SobolAnalysis" ||
           analysisName == "SRCAnalysis")
  {
    wizard = new SensitivityAnalysisWizard(analysis, this);
  }
  else if (analysisName == "MonteCarloReliabilityAnalysis" ||
           analysisName == "FORMImportanceSamplingAnalysis" ||
           analysisName == "FORMAnalysis")
  {
    wizard = new ReliabilityAnalysisWizard(item, analysis, isGeneralWizard, this);
  }
  else if (analysisName == "ImportedDesignOfExperiment" ||
           analysisName == "GridDesignOfExperiment" ||
           analysisName == "ProbabilisticDesignOfExperiment")
  {
    wizard = new DesignOfExperimentWizard(analysis, this);
  }
  else
  {
    qDebug() << "Error: In runAnalysis: analysisName " << analysisName.c_str() << " not recognized.\n";
  }

  return wizard;
}
}
