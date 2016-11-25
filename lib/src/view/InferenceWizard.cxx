//                                               -*- C++ -*-
/**
 *  @brief QWizard for inference
 *
 *  Copyright 2015-2016 EDF-Phimeca
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

#include "otgui/AnalyticalPhysicalModel.hxx"
#include "otgui/InputTableProbabilisticModel.hxx"
#include "otgui/CheckableHeaderView.hxx"
#include "otgui/DistributionDictionary.hxx"
#include "otgui/DistributionsForInferenceWidget.hxx"
#include "otgui/ResizableStackedWidget.hxx"
#include "otgui/CustomStandardItemModel.hxx"
#include "otgui/VariablesInferenceTableModel.hxx"
#include "otgui/DoubleSpinBox.hxx"

#include <openturns/OTDistribution.hxx>
#include <openturns/FittingTest.hxx>

#include <QTableView>
#include <QHBoxLayout>
#include <QSplitter>
#include <QComboBox>
#include <QPushButton>

using namespace OT;

namespace OTGUI {

InferenceWizard::InferenceWizard(const OTStudy& otStudy, const DesignOfExperiment& designOfExperiment, QWidget* parent)
  : AnalysisWizard(InferenceAnalysis(otStudy.getAvailableAnalysisName("inference_"), designOfExperiment), parent)
  , currentVariableName_(designOfExperiment.getSample().getDescription()[0])
  , inference_(*dynamic_cast<InferenceAnalysis*>(&*analysis_.getImplementation()))
  , errorMessageLabel_(new QLabel)
  , pageValidity_(true)
{
  for (UnsignedInteger i=0; i<designOfExperiment.getSample().getDimension(); ++i)
  {
    const String variableName = inference_.getDesignOfExperiment().getSample().getDescription()[i];
    if (inference_.getInterestVariables().contains(variableName))
      distFactoriesForEachInterestVar_[variableName] = inference_.getDistributionsFactories(variableName);
    else
      distFactoriesForEachInterestVar_[variableName] = FittingTest::DistributionFactoryCollection();
  }

  buildInterface();
}


InferenceWizard::InferenceWizard(const Analysis& analysis, QWidget* parent)
  : AnalysisWizard(analysis, parent)
  , inference_(*dynamic_cast<InferenceAnalysis*>(&*analysis_.getImplementation()))
  , errorMessageLabel_(new QLabel)
  , pageValidity_(true)
{
  const Description doeVariablesNames(inference_.getDesignOfExperiment().getSample().getDescription());

  currentVariableName_ = doeVariablesNames[0];

  for (UnsignedInteger i=0; i<doeVariablesNames.getSize(); ++i)
  {
    const String variableName = doeVariablesNames[i];
    if (inference_.getInterestVariables().contains(variableName))
      distFactoriesForEachInterestVar_[variableName] = inference_.getDistributionsFactories(variableName);
    else
      distFactoriesForEachInterestVar_[variableName] = FittingTest::DistributionFactoryCollection();
  }

  buildInterface();
}


void InferenceWizard::buildInterface()
{
  setWindowTitle(tr("Inference"));

  QWizardPage * page = new QWizardPage(this);
  QVBoxLayout * pageLayout = new QVBoxLayout(page);
  QHBoxLayout * splitter = new QHBoxLayout;

  // table view
  QTableView * variablesTableView = new QTableView;
  variablesTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  variablesTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  variablesTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  variablesTableView->setShowGrid(false);

  // - model
  const Description variablesNames(inference_.getDesignOfExperiment().getSample().getDescription());
  Interval::BoolCollection isVariablesChecked(variablesNames.getSize());
  for (UnsignedInteger i=0; i<variablesNames.getSize(); ++i)
    isVariablesChecked[i] = inference_.getInterestVariables().contains(variablesNames[i]);
  VariablesInferenceTableModel * model = new VariablesInferenceTableModel(variablesNames, isVariablesChecked, variablesTableView);
  variablesTableView->setModel(model);

  connect(variablesTableView, SIGNAL(clicked(QModelIndex)), variablesTableView, SLOT(setCurrentIndex(QModelIndex)));
  connect(model, SIGNAL(selectionChanged(OT::Description, OT::String)), this, SLOT(updateInterestVariables(OT::Description, OT::String)));
  connect(variablesTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(selectedVariableChanged(QModelIndex,QModelIndex)));

  // - header
  CheckableHeaderView * variablesTableHeaderView = new CheckableHeaderView;
  variablesTableView->setHorizontalHeader(variablesTableHeaderView);
  variablesTableView->verticalHeader()->hide();
  variablesTableHeaderView->setChecked(!isVariablesChecked.contains(false));
  connect(model, SIGNAL(checked(bool)), variablesTableHeaderView, SLOT(setChecked(bool)));

  // - resize table
  int w = variablesTableView->horizontalHeader()->length();
  variablesTableView->resizeColumnToContents(0);
  if (variablesTableView->horizontalHeader()->length() > w)
    w = variablesTableView->horizontalHeader()->length();
  int x1, y1, x2, y2;
  variablesTableView->getContentsMargins(&x1, &y1, &x2, &y2);
  variablesTableView->setFixedWidth(w+x1+x2);
  variablesTableView->horizontalHeader()->resizeSection(0, w+x1+x2);

  splitter->addWidget(variablesTableView);

  // list distributions
  ResizableStackedWidget * stackWidget = new ResizableStackedWidget;
  for (UnsignedInteger i=0; i<variablesNames.getSize(); ++i)
  {
    QStringList distributions;
    if (inference_.getInterestVariables().contains(variablesNames[i]))
    {
      FittingTest::DistributionFactoryCollection collection(inference_.getDistributionsFactories(variablesNames[i]));
      for (UnsignedInteger j=0; j<collection.getSize(); ++j)
      {
        String str = collection[j].getImplementation()->getClassName();
        distributions << str.substr(0, str.find("Factory")).c_str();
      }
    }

    DistributionsForInferenceWidget * distributionsWidget = new DistributionsForInferenceWidget(distributions, this);
    connect(distributionsWidget, SIGNAL(distributionsListChanged(QStringList)), this, SLOT(updateDistributionsListForVariable(QStringList)));

    stackWidget->addWidget(distributionsWidget);
  }
  splitter->addWidget(stackWidget);
  connect(this, SIGNAL(currentVariableChanged(int)), stackWidget, SLOT(setCurrentIndex(int)));
  connect(this, SIGNAL(currentVariableChecked(bool)), stackWidget, SLOT(setEnabled(bool)));

  pageLayout->addLayout(splitter);

  // level
  QGridLayout * levelLayout = new QGridLayout;
  QLabel * levelLabel = new QLabel(tr("Level"));
  levelLayout->addWidget(levelLabel, 0, 0);
  DoubleSpinBox * levelSpinbox = new DoubleSpinBox;
  levelSpinbox->setRange(0.0+1.e-6, 1.-1.e-6);
  levelSpinbox->setSingleStep(0.01);
  levelSpinbox->setValue(inference_.getLevel());
  connect(levelSpinbox, SIGNAL(valueChanged(double)), this, SLOT(levelChanged(double)));
  levelLayout->addWidget(levelSpinbox, 0, 1);
  levelLayout->setColumnStretch(1, 10);

  pageLayout->addLayout(levelLayout);

  variablesTableView->selectRow(0);

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  pageLayout->addWidget(errorMessageLabel_);

  addPage(page);
}


void InferenceWizard::selectedVariableChanged(QModelIndex current, QModelIndex previous)
{
  const int currentRow = current.row();
  currentVariableName_ = inference_.getDesignOfExperiment().getSample().getDescription()[currentRow];

  const bool isChecked = inference_.getInterestVariables().contains(currentVariableName_);
  emit currentVariableChanged(currentRow);
  emit currentVariableChecked(isChecked);
}


void InferenceWizard::updateDistributionsListForVariable(QStringList distributions)
{
  FittingTest::DistributionFactoryCollection distCollection;
  for (int i=0; i<distributions.size(); ++i)
    distCollection.add(DistributionDictionary::BuildDistributionFactory(distributions[i].toStdString()));

  try
  {
    inference_.setDistributionsFactories(currentVariableName_, distCollection);
    distFactoriesForEachInterestVar_[currentVariableName_] = distCollection;
  }
  catch (std::exception & ex)
  {
    //
  }
}


void InferenceWizard::updateInterestVariables(Description interestVariables, OT::String varName)
{
  errorMessageLabel_->setText("");
  try
  {
    Description oldInterestVariables(inference_.getInterestVariables());
    inference_.setInterestVariables(interestVariables);
    if (!oldInterestVariables.contains(varName))
     inference_.setDistributionsFactories(varName, distFactoriesForEachInterestVar_[varName]);
    pageValidity_ = true;

    const bool isChecked = interestVariables.contains(varName);
    emit currentVariableChecked(isChecked);
  }
  catch (std::exception & ex)
  {
    pageValidity_ = false;
    QString errorMessage = tr("Select at least one variable");
    errorMessageLabel_->setText(QString("%1%2%3").arg("<font color=red>").arg(errorMessage).arg("</font>"));
  }
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

  const Description interestVariables(inference_.getInterestVariables());
  for (UnsignedInteger i=0; i<interestVariables.getSize(); ++i)
  {
    if (!inference_.getDistributionsFactories(interestVariables[i]).getSize())
    {
      QString errorMessage = tr("At least one distribution must be tested for the selected variable '%1'").arg(interestVariables[i].c_str());
      errorMessageLabel_->setText(QString("%1%2%3").arg("<font color=red>").arg(errorMessage).arg("</font>"));
      return false;
    }
  }
  analysis_ = inference_;

  return QWizard::validateCurrentPage();
}
}