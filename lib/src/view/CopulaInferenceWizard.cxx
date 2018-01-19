//                                               -*- C++ -*-
/**
 *  @brief QWizard for copula inference
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
#include "otgui/CopulaInferenceWizard.hxx"

#include "otgui/TranslationManager.hxx"
#include "otgui/DistributionsForInferenceWidget.hxx"
#include "otgui/QtTools.hxx"
#include "otgui/DistributionDictionary.hxx"
#include "otgui/CheckableHeaderView.hxx"

#include <openturns/NormalCopulaFactory.hxx>

#include <QVBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QHeaderView>

using namespace OT;

namespace OTGUI
{

CopulaInferenceWizard::CopulaInferenceWizard(const Analysis &analysis, QWidget *parent)
  : AnalysisWizard(analysis, parent)
{
  CopulaInferenceAnalysis * analysis_ptr = dynamic_cast<CopulaInferenceAnalysis*>(analysis_.getImplementation().get());
  Q_ASSERT(analysis_ptr);

  const Collection<Description> groups(analysis_ptr->getVariablesGroups());
  for (UnsignedInteger i = 0; i < groups.getSize(); ++i)
    distForVars_[groups[i]] = analysis_ptr->getDistributionsFactories(groups[i]);

  const Description doeVariables(analysis_ptr->getDesignOfExperiment().getSample().getDescription());
  Interval::BoolCollection selectedVars(doeVariables.getSize());
  for (UnsignedInteger i = 0; i < 2; ++i)
    selectedVars[i] = true;
  varTableModel_ = new VariablesSelectionTableModel(doeVariables, selectedVars, this);

  buildInterface();
}


void CopulaInferenceWizard::buildInterface()
{
  setWindowTitle(tr("Copula inference"));

  QWizardPage * page = new QWizardPage(this);
  QVBoxLayout * pageLayout = new QVBoxLayout(page);

  // info label at the top of the wizard
  QLabel * label = new QLabel;
  label->setWordWrap(true);
  label->setText(tr("Firstly, define groups of variables. Then for each group, list copulas to infer from the sample."));
  pageLayout->addWidget(label);

  QHBoxLayout * hSplitter = new QHBoxLayout;

  // DOE variables table
  QWidget * leftWidget = new QWidget;
  QGridLayout * leftWidgetLayout = new QGridLayout(leftWidget);

  // - table view
  QTableView * varTableView = new QTableView;
  varTableView->setSelectionMode(QAbstractItemView::NoSelection);
  varTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  varTableView->verticalHeader()->hide();

  // - table model
  varTableView->setModel(varTableModel_);

  // - horizontal header
  CheckableHeaderView * headerView = new CheckableHeaderView;
  headerView->setStretchLastSection(true);
  varTableView->setHorizontalHeader(headerView);

  leftWidgetLayout->addWidget(varTableView, 0, 0, 2, 1);
  hSplitter->addWidget(leftWidget);

  // - remove button
  QToolButton * arrowButton = new QToolButton;
  arrowButton->setArrowType(Qt::LeftArrow);
  arrowButton->setToolTip(tr("Remove the selected group of variables"));
  leftWidgetLayout->addWidget(arrowButton, 0, 1, Qt::AlignBottom);
  connect(arrowButton, SIGNAL(pressed()), this, SLOT(removeGroup()));

  // - add button
  arrowButton = new QToolButton;
  arrowButton->setArrowType(Qt::RightArrow);
  arrowButton->setToolTip(tr("Add a group of variables"));
  leftWidgetLayout->addWidget(arrowButton, 1, 1, Qt::AlignTop);
  connect(arrowButton, SIGNAL(pressed()), this, SLOT(defineGroup()));

  // left side
  QWidget * leftSideWidget = new QWidget;
  QVBoxLayout * leftSideLayout = new QVBoxLayout(leftSideWidget);

  // table view
  tableView_ = new QTableView;
  tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
  tableView_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  tableView_->horizontalHeader()->setStretchLastSection(true);
  tableView_->verticalHeader()->hide();
  tableView_->setShowGrid(false);
  leftSideLayout->addWidget(tableView_);

  tableModel_ = new QStandardItemModel(tableView_);
  tableModel_->setHorizontalHeaderLabels(QStringList() << tr("Groups"));
  tableView_->setModel(tableModel_);
  connect(tableView_->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), this, SLOT(selectedItemChanged(QModelIndex, QModelIndex)));

  hSplitter->addWidget(leftSideWidget);

  // list dist
  stackedWidget_ = new ResizableStackedWidget;
  std::map<Description, CopulaInferenceAnalysis::DistributionFactoryCollection>::iterator it;
  for (it = distForVars_.begin(); it != distForVars_.end(); ++it)
  {
    Description vars(it->first);
    QStandardItem * newItem = new QStandardItem(vars.__str__().c_str());
    newItem->setData(qVariantFromValue(vars), Qt::UserRole + 1);
    tableModel_->appendRow(newItem);

    CopulaInferenceAnalysis::DistributionFactoryCollection factories(it->second);
    QStringList dist;
    for (UnsignedInteger j = 0; j < factories.getSize(); ++j)
    {
      String str = factories[j].getImplementation()->getClassName();
      dist << TranslationManager::GetTranslatedCopulaName(str.substr(0, str.find("Copula")));
    }

    DistributionsForInferenceWidget * distWidget = new DistributionsForInferenceWidget(dist, vars, this);
    connect(distWidget, SIGNAL(distributionsListChanged(OT::Description, QStringList)), this, SLOT(updateDistForVars(OT::Description, QStringList)));

    stackedWidget_->addWidget(distWidget);
  }
  hSplitter->addWidget(stackedWidget_);

  pageLayout->addLayout(hSplitter);

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  errorMessageLabel_->setWordWrap(true);
  pageLayout->addWidget(errorMessageLabel_);
  connect(varTableModel_, SIGNAL(selectionChanged(OT::Description, OT::String)), errorMessageLabel_, SLOT(clear()));

  tableView_->selectRow(0);

  addPage(page);
}


void CopulaInferenceWizard::selectedItemChanged(const QModelIndex &current, const QModelIndex &old)
{
  if (current.isValid())
    stackedWidget_->setCurrentIndex(current.row());
}


void CopulaInferenceWizard::updateDistForVars(const Description& vars, const QStringList& dist)
{
  errorMessageLabel_->setText("");
  CopulaInferenceAnalysis::DistributionFactoryCollection factories;
  for (int i = 0; i < dist.size(); ++i)
  {
    const String distName = TranslationManager::GetCopulaName(dist[i]);
    factories.add(DistributionDictionary::BuildCopulaFactory(distName));
  }
  distForVars_[vars] = factories;
}


void CopulaInferenceWizard::removeGroup()
{
  if (tableView_->selectionModel()->hasSelection())
  {
    QModelIndex index = tableView_->selectionModel()->currentIndex();
    int previousRow = index.row() - 1;

    stackedWidget_->removeWidget(stackedWidget_->widget(index.row()));
    const QVariant variant = tableModel_->data(index, Qt::UserRole + 1);
    Q_ASSERT(variant.canConvert<Description>());
    Description vars = variant.value<Description>();
    distForVars_.erase(vars);
    tableModel_->removeRow(index.row());
    tableView_->selectRow(previousRow < 0 ? 0 : previousRow);
    stackedWidget_->setCurrentIndex(previousRow < 0 ? 0 : previousRow);
  }
}


void CopulaInferenceWizard::defineGroup()
{
  Description selectedVars(varTableModel_->getSelectedVariables());

  // check if at least two variables
  QString errorMessage;
  if (selectedVars.getSize() < 2)
  {
    errorMessage = tr("Define at least a group of two variables");
  }
  selectedVars.sort();
  // check if group already exists
  Collection<Description> collections;
  std::map<Description, CopulaInferenceAnalysis::DistributionFactoryCollection>::iterator it;
  for (it = distForVars_.begin(); it != distForVars_.end(); ++it)
  {
    collections.add(it->first);
  }
  if (collections.contains(selectedVars))
  {
    errorMessage = tr("This group of variables already exists");
  }
  if (!errorMessage.isEmpty())
  {
    errorMessageLabel_->setTemporaryErrorMessage(errorMessage);
    return;
  }

  // add group
  CopulaInferenceAnalysis::DistributionFactoryCollection collectionCopula;
  collectionCopula.add(NormalCopulaFactory());
  distForVars_[selectedVars] = collectionCopula;

  QStandardItem * newItem = new QStandardItem(selectedVars.__str__().c_str());
  newItem->setData(qVariantFromValue(selectedVars), Qt::UserRole + 1);
  tableModel_->appendRow(newItem);

  QStringList dist(TranslationManager::GetTranslatedCopulaName("Normal"));

  DistributionsForInferenceWidget * distWidget = new DistributionsForInferenceWidget(dist, selectedVars, this);
  connect(distWidget, SIGNAL(distributionsListChanged(OT::Description, QStringList)), this, SLOT(updateDistForVars(OT::Description, QStringList)));

  stackedWidget_->addWidget(distWidget);
  tableView_->selectRow(tableModel_->rowCount() - 1);
}


Analysis CopulaInferenceWizard::getAnalysis() const
{
  // get the doe
  DesignOfExperiment doe = dynamic_cast<const DesignOfExperimentAnalysis*>(analysis_.getImplementation().get())->getDesignOfExperiment();
  CopulaInferenceAnalysis newAnalysis(analysis_.getName(), doe, Collection<Description>());

  std::map<Description, CopulaInferenceAnalysis::DistributionFactoryCollection>::const_iterator it;
  for (it = distForVars_.begin(); it != distForVars_.end(); ++it)
  {
    newAnalysis.setDistributionsFactories(it->first, it->second);
  }
  return newAnalysis;
}


bool CopulaInferenceWizard::validateCurrentPage()
{
  // check at least a group of variables
  if (!tableModel_->rowCount())
  {
    const QString errorMessage = tr("Define at least a group of variables associated with a list of copulas");
    errorMessageLabel_->setText(QString("<font color=red>%1</font>").arg(errorMessage));
    return false;
  }
  // check there are at least a copula to test for each group of variables
  std::map<Description, CopulaInferenceAnalysis::DistributionFactoryCollection>::iterator it;
  for (it = distForVars_.begin(); it != distForVars_.end(); ++it)
  {
    if (!it->second.getSize())
    {
      const QString errorMessage = tr("At least one copula must be tested for the selected group of variables '%1'").arg(it->first.__str__().c_str());
      errorMessageLabel_->setText(QString("<font color=red>%1</font>").arg(errorMessage));
      return false;
    }
  }

  return QWizard::validateCurrentPage();
}
}
