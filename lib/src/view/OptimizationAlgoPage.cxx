//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define the method of sensitivity analysis
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
#include "persalys/OptimizationAlgoPage.hxx"

#include "persalys/OptimizationAnalysis.hxx"
#include "persalys/RadioButtonDelegate.hxx"
#include "persalys/LinkableItemDelegate.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/DocumentationToolButton.hxx"

#include <openturns/NLopt.hxx>

#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QScrollArea>
#include <QHeaderView>

using namespace OT;

namespace PERSALYS
{

OptimizationAlgoPage::OptimizationAlgoPage(QWidget* parent)
  : QWizardPage(parent)
  , solverNames_()
  , algoTableView_(0)
  , algoTableModel_(0)
  , outputsSelectionGroupBox_(0)
  , errorMessageLabel_(0)
{
}

void OptimizationAlgoPage::buildInterface()
{
  setTitle(tr("Optimization methods"));
  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  // output selection
  outputsSelectionGroupBox_ = new OutputsSelectionGroupBox(false, this);
  pageLayout->addWidget(outputsSelectionGroupBox_);

  QGroupBox * groupBox = new QGroupBox(tr("Available algorithms"));
  QGridLayout * groupBoxLayout = new QGridLayout(groupBox);

  QStringList localityFields;
  localityFields << tr("Local") << tr("Global") << tr("Any");
  QLabel * localLabel = new QLabel(tr("Locality"));
  groupBoxLayout->addWidget(localLabel, 0, 0);
  localityCombobox_ = new QComboBox;
  localityCombobox_->addItems(localityFields);
  localityCombobox_->setCurrentIndex(2);
  groupBoxLayout->addWidget(localityCombobox_, 0, 1);

  QStringList derivativeFields;
  derivativeFields << tr("None") << tr("First") << tr("Any");
  QLabel * derivLabel = new QLabel(tr("Derivative"));
  groupBoxLayout->addWidget(derivLabel, 0, 2);
  derivativeCombobox_ = new QComboBox;
  derivativeCombobox_->addItems(derivativeFields);
  derivativeCombobox_->setCurrentIndex(2);
  groupBoxLayout->addWidget(derivativeCombobox_, 0, 3);

  groupBoxLayout->setColumnStretch(4, 1);

  connect(localityCombobox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFilters()));
  connect(derivativeCombobox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFilters()));

  algoTableView_ = new QTableView;
  RadioButtonDelegate * delegate = new RadioButtonDelegate(0, algoTableView_);
  LinkableItemDelegate * delegate2 = new LinkableItemDelegate(0, algoTableView_);
  algoTableView_->setItemDelegateForColumn(0, delegate);
  algoTableView_->setItemDelegateForColumn(3, delegate2);
  algoTableView_->setSelectionMode(QAbstractItemView::NoSelection);
  algoTableView_->verticalHeader()->hide();
  groupBoxLayout->addWidget(algoTableView_, 1, 0, 1, 5);
  groupBoxLayout->setRowStretch(1, 1);

  algoTableModel_ = new CustomStandardItemModel(solverNames_.getSize(), 5, algoTableView_);

  proxyModel_ = new AlgoFilterProxyModel;
  proxyModel_->setSourceModel(algoTableModel_);
  proxyModel_->setDynamicSortFilter(false);

  algoTableView_->setModel(proxyModel_);
  pageLayout->addWidget(groupBox);

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  connect(outputsSelectionGroupBox_, SIGNAL(outputsSelectionChanged(QStringList)), errorMessageLabel_, SLOT(reset()));

  //pageLayout->addStretch();
  pageLayout->addWidget(errorMessageLabel_);
}


void OptimizationAlgoPage::initialize(OptimizationAnalysis& analysis)
{
  algoTableModel_->clear();
  algoTableModel_->setHorizontalHeaderItem(0, new QStandardItem(tr("Name")));
  algoTableModel_->setHorizontalHeaderItem(1, new QStandardItem(tr("Locality")));
  algoTableModel_->setHorizontalHeaderItem(2, new QStandardItem(tr("Derivative")));
  algoTableModel_->setHorizontalHeaderItem(3, new QStandardItem(tr("Doc")));

  errorMessageLabel_->reset();
  solverNames_ = OptimizationAnalysis::GetSolverNames(analysis.getBounds(), analysis.getVariablesType());

  for (UnsignedInteger i = 0; i < solverNames_.getSize(); ++i) {
    algoTableModel_->setNotEditableItem(i, 0, solverNames_[i].c_str());
    algoTableModel_->setData(algoTableModel_->index(i, 0), (int)i, Qt::UserRole);
    switch(OptimizationAnalysis::AlgorithmDictionary[solverNames_[i]].getLocality()) {
    case 0:
      algoTableModel_->setNotEditableItem(i, 1, "Local");
      break;
    case 1:
      algoTableModel_->setNotEditableItem(i, 1, "Global");
      break;
    case 2:
      algoTableModel_->setNotEditableItem(i, 1, "L/G");
      break;
    }
    switch(OptimizationAnalysis::AlgorithmDictionary[solverNames_[i]].getDerivative()) {
    case 0:
      algoTableModel_->setNotEditableItem(i, 2, "None");
      break;
    case 1:
      algoTableModel_->setNotEditableItem(i, 2, "First");
      break;
    case 2:
      algoTableModel_->setNotEditableItem(i, 2, "Any");
      break;
    }
    const String docString = "<a href=\""+OptimizationAnalysis::AlgorithmDictionary[solverNames_[i]].getDoc()+"\">doc</a>";
    algoTableModel_->setNotEditableItem(i, 3, QString::fromStdString(docString));
    algoTableModel_->setData(algoTableModel_->index(i, 3), (QString)OptimizationAnalysis::AlgorithmDictionary[solverNames_[i]].getDoc().c_str(), Qt::UserRole);

    algoTableModel_->setNotEditableItem(i, 4, QString::number(OptimizationAnalysis::AlgorithmDictionary[solverNames_[i]].getPriority()));
    algoTableModel_->setData(algoTableModel_->index(i, 4), (int)OptimizationAnalysis::AlgorithmDictionary[solverNames_[i]].getPriority(), Qt::UserRole);
  }

  algoTableView_->hideColumn(4);
  algoTableView_->horizontalHeader()->hideSection(4);

  // method
  const String methodName = analysis.getSolverName();
  const Description::const_iterator it = std::find(solverNames_.begin(), solverNames_.end(), methodName);
  UnsignedInteger index = 0;
  //if algo is no longer compatible default to Cobyla
  if (solverNames_.getSize()) {
    if (it!=solverNames_.end())
      index = it - solverNames_.begin();
    else {
      index = std::find(solverNames_.begin(), solverNames_.end(), "Cobyla") - solverNames_.begin();
      if (index == solverNames_.getSize())
        index = 0;
      LOGWARN(OSS() << "Optimization problem has changed and " << methodName << " algorithm is no longer available... Using " << solverNames_[index] <<" as default.");
    }
    algoTableView_->selectRow(index);
    algoTableModel_->setData(algoTableModel_->index(index, 0), true, Qt::CheckStateRole);
  }
  updateFilters();

  // update outputs list
  PhysicalModel model = analysis.getPhysicalModel();
  outputsSelectionGroupBox_->updateComboBoxModel(model.getSelectedOutputsNames(), analysis.getInterestVariables());

  connect(algoTableView_, SIGNAL(clicked(QModelIndex)), this, SLOT(updateRadioButtonsAlgoTable(QModelIndex)));
  connect(algoTableView_, SIGNAL(clicked(QModelIndex)), this, SLOT(openDoc(QModelIndex)));
}



Description OptimizationAlgoPage::getInterestVariables() const
{
  return QtOT::StringListToDescription(outputsSelectionGroupBox_->getSelectedOutputsNames());
}


String OptimizationAlgoPage::getSolverName() const
{
  // check
  if (!(algoTableModel_ && algoTableView_))
    throw InternalException(HERE) << "Error in OptimizationAlgoPage::getSolverName";

  QModelIndex selectedAlgoIndex;
  for (int i = 1; i < algoTableModel_->rowCount(); ++i)
    if (algoTableModel_->data(algoTableModel_->index(i, 0), Qt::CheckStateRole).toBool())
      selectedAlgoIndex = algoTableModel_->index(i, 0);

  if (selectedAlgoIndex.isValid())
    return solverNames_[algoTableModel_->data(selectedAlgoIndex, Qt::UserRole).toInt()];
  else
    return solverNames_[0];
}


bool OptimizationAlgoPage::validatePage()
{
  if (outputsSelectionGroupBox_->getSelectedOutputsNames().size() != 1)
  {
    errorMessageLabel_->setErrorMessage(tr("Only one output must be selected"));
    return false;
  }
  if (!solverNames_.getSize()) {
    errorMessageLabel_->setErrorMessage(tr("Cannot find a compatible algorithm"));
    return false;
  }
  if (!errorMessageLabel_->text().isEmpty())
    return false;
  emit outputSelected();
  return QWizardPage::validatePage();
}

void OptimizationAlgoPage::updateRadioButtonsAlgoTable(QModelIndex current)
{
  // check
  if (!algoTableModel_ || current.row() < 0 || current.column() > 0)
    return;

  // set CheckStateRole of algoTableModel_
  for (int i = 0; i < algoTableModel_->rowCount(); ++i)
  {
    if (algoTableModel_->index(i, 0).row() == proxyModel_->mapToSource(current).row())
      algoTableModel_->setData(algoTableModel_->index(i, 0), true, Qt::CheckStateRole);
    else
      algoTableModel_->setData(algoTableModel_->index(i, 0), false, Qt::CheckStateRole);
  }
}

void OptimizationAlgoPage::openDoc(QModelIndex current)
{
  // check
  if (!algoTableModel_ || current.row() < 0 || current.column() != 3)
    return;

  const int currentRow = proxyModel_->mapToSource(current).row();
  QString urlLink = algoTableModel_->data(algoTableModel_->index(currentRow, 3), Qt::UserRole).toString();
  FileTools::OpenUrl(urlLink);
}


void OptimizationAlgoPage::updateFilters() {
  errorMessageLabel_->reset();
  const int derivIndex = derivativeCombobox_->currentIndex();
  const int localIndex = localityCombobox_->currentIndex();
  QList<int> selectedDerivative;
  QList<int> selectedLocality;

  for (UnsignedInteger i = 0; i < solverNames_.getSize(); ++ i) {
    AlgorithmProperty property = OptimizationAnalysis::AlgorithmDictionary[solverNames_[i]];
    if (property.getDerivative() == derivIndex ||
        derivIndex == AlgorithmProperty::AnyD)
      selectedDerivative.append(i);
    if (property.getLocality() == localIndex||
        localIndex == AlgorithmProperty::AnyL)
      selectedLocality.append(i);
  }

  proxyModel_->setDerivativeFilter(selectedDerivative);
  proxyModel_->setLocalityFilter(selectedLocality);

  // if selected algo got filtered take the non filtered algo with highest priority
  for (int i = 0; i < algoTableModel_->rowCount(); ++i) {
    if (algoTableModel_->data(algoTableModel_->index(i, 0), Qt::CheckStateRole).toBool()) {
      const int selectedIndex = algoTableModel_->index(i, 0).row();
      if (!proxyModel_->mapFromSource(proxyModel_->sourceModel()->index(selectedIndex,0)).isValid()) {
        algoTableModel_->setData(algoTableModel_->index(i, 0), false, Qt::CheckStateRole);
        int prioIndex = proxyModel_->mapToSource(proxyModel_->index(0, 0)).row();
        for(int j = 1; j<proxyModel_->rowCount(); ++j) {
          const int modelIndex = proxyModel_->mapToSource(proxyModel_->index(j, 0)).row();
          if (algoTableModel_->data(algoTableModel_->index(modelIndex, 4), Qt::UserRole).toInt() >
              algoTableModel_->data(algoTableModel_->index(prioIndex, 4), Qt::UserRole).toInt()) {
            prioIndex = proxyModel_->mapToSource(proxyModel_->index(j, 0)).row();
          }
        }
        algoTableModel_->setData(algoTableModel_->index(prioIndex, 0), true, Qt::CheckStateRole);
      }
      break;
    }
  }

  // If all algos get filtered, default to first so that the previous for loop can correctly
  // take place during next update
  if (!selectedLocality.size() || !selectedDerivative.size()) {
    algoTableModel_->setData(algoTableModel_->index(0, 0), true, Qt::CheckStateRole);
    errorMessageLabel_->setErrorMessage(tr("Cannot find a compatible algorithm"));
  }

  algoTableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

AlgoFilterProxyModel::AlgoFilterProxyModel(QObject *parent)
  : QSortFilterProxyModel(parent)
{}

void AlgoFilterProxyModel::setDerivativeFilter(const QList<int> & derivativeFilter)
{
  derivativeFilter_ = derivativeFilter;
  invalidateFilter();
}

void AlgoFilterProxyModel::setLocalityFilter(const QList<int> & localityFilter)
{
  localityFilter_ = localityFilter;
  invalidateFilter();
}

bool AlgoFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const {
  QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
  return derivativeFilter_.contains(index.row()) && localityFilter_.contains(index.row());
}

}
