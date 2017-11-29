//                                               -*- C++ -*-
/**
 *  @brief QWizard for inference
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
#include "otgui/InferenceWizard.hxx"

#include "otgui/CheckableHeaderView.hxx"
#include "otgui/DistributionDictionary.hxx"
#include "otgui/DistributionsForInferenceWidget.hxx"
#include "otgui/ResizableStackedWidget.hxx"
#include "otgui/DoubleSpinBox.hxx"
#include "otgui/TranslationManager.hxx"

#include <openturns/OTDistribution.hxx>
#include <openturns/FittingTest.hxx>

#include <QHBoxLayout>
#include <QGroupBox>

using namespace OT;

namespace OTGUI
{

InferenceWizard::InferenceWizard(const Analysis& analysis, QWidget* parent)
  : AnalysisWizard(analysis, parent)
  , currentVarName_("")
  , inference_()
  , interestVar_()
  , distFactoriesForEachInterestVar_()
  , errorMessageLabel_(0)
  , pageValidity_(true)
  , varTableModel_(0)
{
  initialize();

  buildInterface();
}


void InferenceWizard::initialize()
{
  InferenceAnalysis * analysis_ptr = dynamic_cast<InferenceAnalysis*>(analysis_.getImplementation().get());
  Q_ASSERT(analysis_ptr);
  inference_ = *analysis_ptr;
  interestVar_ = inference_.getInterestVariables();

  const Description doeVarNames(inference_.getDesignOfExperiment().getSample().getDescription());

  currentVarName_ = doeVarNames[0];

  for (UnsignedInteger i = 0; i < doeVarNames.getSize(); ++i)
  {
    const String varName = doeVarNames[i];
    if (inference_.getInterestVariables().contains(varName))
      distFactoriesForEachInterestVar_[varName] = inference_.getDistributionsFactories(varName);
    else
      distFactoriesForEachInterestVar_[varName] = FittingTest::DistributionFactoryCollection();
  }
}


void InferenceWizard::buildInterface()
{
  setWindowTitle(tr("Inference"));

  QWizardPage * page = new QWizardPage(this);
  QVBoxLayout * pageLayout = new QVBoxLayout(page);
  QHBoxLayout * splitter = new QHBoxLayout;

  // table view
  QTableView * varTableView = new QTableView;
  varTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  varTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  varTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  varTableView->setShowGrid(false);

  // - model
  const Description varNames(inference_.getDesignOfExperiment().getSample().getDescription());
  Interval::BoolCollection isVarChecked(varNames.getSize());
  for (UnsignedInteger i = 0; i < varNames.getSize(); ++i)
    isVarChecked[i] = inference_.getInterestVariables().contains(varNames[i]);

  varTableModel_ = new VariablesInferenceTableModel(varNames, isVarChecked, varTableView);
  varTableView->setModel(varTableModel_);

  connect(varTableView, SIGNAL(clicked(QModelIndex)), varTableView, SLOT(setCurrentIndex(QModelIndex)));
  connect(varTableModel_, SIGNAL(selectionChanged(OT::Description, OT::String)), this, SLOT(updateInterestVar(OT::Description, OT::String)));
  connect(varTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), this, SLOT(selectedVarChanged(QModelIndex, QModelIndex)));

  // - header
  CheckableHeaderView * varTableHeaderView = new CheckableHeaderView;
  varTableView->setHorizontalHeader(varTableHeaderView);
  varTableView->verticalHeader()->hide();
  varTableHeaderView->setChecked(!isVarChecked.contains(false));
  connect(varTableModel_, SIGNAL(checked(bool)), varTableHeaderView, SLOT(setChecked(bool)));

  // - resize table
  int w = varTableView->horizontalHeader()->length();
  varTableView->resizeColumnToContents(0);
  if (varTableView->horizontalHeader()->length() > w)
    w = varTableView->horizontalHeader()->length();
  int x1, y1, x2, y2;
  varTableView->getContentsMargins(&x1, &y1, &x2, &y2);
  varTableView->setFixedWidth(w + x1 + x2);
  varTableView->horizontalHeader()->resizeSection(0, w + x1 + x2);

  splitter->addWidget(varTableView);

  // list dist
  ResizableStackedWidget * stackWidget = new ResizableStackedWidget;
  for (UnsignedInteger i = 0; i < varNames.getSize(); ++i)
  {
    QStringList dist;
    if (inference_.getInterestVariables().contains(varNames[i]))
    {
      FittingTest::DistributionFactoryCollection collection(inference_.getDistributionsFactories(varNames[i]));
      for (UnsignedInteger j = 0; j < collection.getSize(); ++j)
      {
        String str = collection[j].getImplementation()->getClassName();
        dist << TranslationManager::GetTranslatedDistributionName(str.substr(0, str.find("Factory")));
      }
    }

    DistributionsForInferenceWidget * distWidget = new DistributionsForInferenceWidget(dist, this);
    connect(distWidget, SIGNAL(distributionsListChanged(QStringList)), this, SLOT(updateDistListForVar(QStringList)));

    stackWidget->addWidget(distWidget);
  }
  splitter->addWidget(stackWidget);
  connect(this, SIGNAL(currentVarChanged(int)), stackWidget, SLOT(setCurrentIndex(int)));
  connect(this, SIGNAL(currentVarChecked(bool)), stackWidget, SLOT(setEnabled(bool)));

  pageLayout->addLayout(splitter);

  // level
  QGroupBox * ksGroupBox = new QGroupBox(tr("Kolmogorov-Smirnov"));
  QGridLayout * levelLayout = new QGridLayout(ksGroupBox);
  QLabel * levelLabel = new QLabel(tr("Level"));
  levelLayout->addWidget(levelLabel, 0, 0);

  DoubleSpinBox * levelSpinbox = new DoubleSpinBox;
  levelSpinbox->setRange(0.0 + 1.e-6, 1. - 1.e-6);
  levelSpinbox->setSingleStep(0.1);
  // display alpha
  levelSpinbox->setValue(inference_.getLevel());
  connect(levelSpinbox, SIGNAL(valueChanged(double)), this, SLOT(levelChanged(double)));
  levelLayout->addWidget(levelSpinbox, 0, 1);
  levelLayout->setColumnStretch(1, 10);

  pageLayout->addWidget(ksGroupBox);

  varTableView->selectRow(0);

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  pageLayout->addWidget(errorMessageLabel_);

  addPage(page);
}


void InferenceWizard::selectedVarChanged(QModelIndex current, QModelIndex /*previous*/)
{
  const int currentRow = current.row();
  currentVarName_ = inference_.getDesignOfExperiment().getSample().getDescription()[currentRow];

  const bool isChecked = interestVar_.contains(currentVarName_);

  emit currentVarChanged(currentRow);
  emit currentVarChecked(isChecked);
}


void InferenceWizard::updateDistListForVar(QStringList dist)
{
  errorMessageLabel_->setText("");

  FittingTest::DistributionFactoryCollection distCollection;
  for (int i = 0; i < dist.size(); ++i)
  {
    const String distName = TranslationManager::GetDistributionName(dist[i]);
    distCollection.add(DistributionDictionary::BuildDistributionFactory(distName));
  }

  distFactoriesForEachInterestVar_[currentVarName_] = distCollection;

  if (!distCollection.getSize())
  {
    Description vars = inference_.getDesignOfExperiment().getSample().getDescription();
    const Description::const_iterator it = std::find(vars.begin(), vars.end(), currentVarName_);
    varTableModel_->setData(varTableModel_->index(it - vars.begin(), 0), Qt::Unchecked, Qt::CheckStateRole);
  }
}


void InferenceWizard::updateInterestVar(Description interestVar, String varName)
{
  errorMessageLabel_->setText("");
  pageValidity_ = true;

  interestVar_ = interestVar;

  if (!interestVar.getSize())
  {
    pageValidity_ = false;
    const QString errorMessage = tr("Select at least one variable");
    errorMessageLabel_->setText(QString("<font color=red>%1</font>").arg(errorMessage));
  }

  emit currentVarChecked(interestVar.contains(varName));
}


void InferenceWizard::levelChanged(double level)
{
  try
  {
    inference_.setLevel(level);
  }
  catch (std::exception& ex)
  {
    //
  }
}


bool InferenceWizard::validateCurrentPage()
{
  if (!pageValidity_)
    return false;

  inference_.setInterestVariables(interestVar_);

  for (UnsignedInteger i = 0; i < interestVar_.getSize(); ++i)
  {
    if (distFactoriesForEachInterestVar_.find(interestVar_[i]) != distFactoriesForEachInterestVar_.end())
    {
      FittingTest::DistributionFactoryCollection factoryCollection(distFactoriesForEachInterestVar_.find(interestVar_[i])->second);
      if (factoryCollection.getSize())
      {
        inference_.setDistributionsFactories(interestVar_[i], factoryCollection);
      }
      else
      {
        const QString errorMessage = tr("At least one distribution must be tested for the selected variable '%1'").arg(interestVar_[i].c_str());
        errorMessageLabel_->setText(QString("<font color=red>%1</font>").arg(errorMessage));
        return false;
      }
    }
  }
  analysis_ = inference_;

  return QWizard::validateCurrentPage();
}
}
