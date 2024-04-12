//                                               -*- C++ -*-
/**
 *  @brief QWizard for inference
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#include "persalys/InferenceWizard.hxx"

#include "persalys/CheckableHeaderView.hxx"
#include "persalys/DistributionDictionary.hxx"
#include "persalys/DistributionsForInferenceWidget.hxx"
#include "persalys/ResizableStackedWidget.hxx"
#include "persalys/DoubleSpinBox.hxx"
#include "persalys/TranslationManager.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/CollapsibleGroupBox.hxx"

#include <openturns/OTDistribution.hxx>
#include <openturns/FittingTest.hxx>

#include <QHBoxLayout>
#include <QGroupBox>

using namespace OT;

namespace PERSALYS
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

  const Description doeVarNames(inference_.getDesignOfExperiment().getSample().getDescription());

  // interest variables
  for (UnsignedInteger i = 0; i < inference_.getInterestVariables().getSize(); ++i)
  {
    if (doeVarNames.contains(inference_.getInterestVariables()[i]))
      interestVar_.add(inference_.getInterestVariables()[i]);
  }

  // current variable
  currentVarName_ = doeVarNames[0];

  // distributions
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
  docLink_ = "user_manual/graphical_interface/data_analysis/user_manual_data_analysis.html#marginalsinferencewizard";

  QWizardPage * page = new QWizardPage(this);
  QVBoxLayout * pageLayout = new QVBoxLayout(page);

  QLabel * label = new QLabel;
  label->setWordWrap(true);
  label->setText(tr("Firstly, select variables. Then for each variable, list distributions to infer from the sample."));
  pageLayout->addWidget(label);

  QHBoxLayout * splitter = new QHBoxLayout;

  QWidget * leftWidget = new QWidget;
  QVBoxLayout * leftWidgetLayout = new QVBoxLayout(leftWidget);
  // table view
  VariablesTableView * varTableView = new VariablesTableView;
  varTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  varTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  varTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  varTableView->setShowGrid(false);
  varTableView->setStyleSheet("QTableView::item:selected{background-color: " + ApplicationColor["lightColor"] + ";color: doubledarkgray;}");
  connect(varTableView, SIGNAL(applyToAllRequested()), this, SLOT(applyCurrentDistToAll()));

  // - model
  const Description varNames(inference_.getDesignOfExperiment().getSample().getDescription());
  Interval::BoolCollection isVarChecked(varNames.getSize());
  for (UnsignedInteger i = 0; i < varNames.getSize(); ++i)
    isVarChecked[i] = inference_.getInterestVariables().contains(varNames[i]);

  varTableModel_ = new VariablesSelectionTableModel(varNames, isVarChecked, varTableView);
  varTableView->setModel(varTableModel_);

  connect(varTableView, SIGNAL(clicked(QModelIndex)), varTableView, SLOT(setCurrentIndex(QModelIndex)));
  connect(varTableModel_, SIGNAL(selectionChanged(OT::Description, OT::String)), this, SLOT(updateInterestVar(OT::Description, OT::String)));
  connect(varTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), this, SLOT(selectedVarChanged(QModelIndex, QModelIndex)));

  // - header
  CheckableHeaderView * varTableHeaderView = new CheckableHeaderView;
  varTableView->setHorizontalHeader(varTableHeaderView);
  varTableView->verticalHeader()->hide();

  // - resize table
  int w = varTableView->horizontalHeader()->length();
  varTableView->resizeColumnToContents(0);
  if (varTableView->horizontalHeader()->length() > w)
    w = varTableView->horizontalHeader()->length();
  const QMargins margins(varTableView->contentsMargins());
  varTableView->setFixedWidth(w + margins.left() + margins.right());
  varTableView->horizontalHeader()->resizeSection(0, w + margins.left() + margins.right());

  leftWidgetLayout->addWidget(varTableView);
  splitter->addWidget(leftWidget);

  // list dist
  stackWidget_ = new ResizableStackedWidget;
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

    DistributionsForInferenceWidget * distWidget = new DistributionsForInferenceWidget(dist, Description(1, varNames[i]), this);
    connect(distWidget, SIGNAL(distributionsListChanged(QStringList)), this, SLOT(updateDistListForVar(QStringList)));

    stackWidget_->addWidget(distWidget);
  }
  splitter->addWidget(stackWidget_, 1);
  connect(this, SIGNAL(currentVarChanged(int)), stackWidget_, SLOT(setCurrentIndex(int)));
  connect(this, SIGNAL(currentVarChecked(bool)), stackWidget_, SLOT(setEnabled(bool)));

  pageLayout->addLayout(splitter);

  // level
  QGroupBox * ksGroupBox = new QGroupBox(tr("Test type"));
  QGridLayout * levelLayout = new QGridLayout(ksGroupBox);

  testTypeComboBox_ = new QComboBox;
  QStringList types;
  types << tr("Kolmogorov-Smirnov") << tr("Lilliefors");
  testTypeComboBox_->addItems(types);
  levelLayout->addWidget(testTypeComboBox_, 0, 0);

  switch(inference_.getTestType())
  {
    case InferenceAnalysis::Lilliefors:
      testTypeComboBox_->setCurrentIndex(1);
      break;
    case InferenceAnalysis::Kolmogorov:
      testTypeComboBox_->setCurrentIndex(0);
      break;
    default:
      throw InvalidArgumentException(HERE) << "Unknown test type.";
  }

  QLabel * levelLabel = new QLabel(tr("Level"));
  levelLayout->addWidget(levelLabel, 0, 1);

  levelSpinbox_ = new DoubleSpinBox;
  levelSpinbox_->setRange(0.0 + 1.e-6, 1. - 1.e-6);
  levelSpinbox_->setSingleStep(0.1);
  // display alpha
  levelSpinbox_->setValue(inference_.getLevel());
  levelLayout->addWidget(levelSpinbox_, 0, 2);
  levelLayout->setColumnStretch(3, 1);

  pageLayout->addWidget(ksGroupBox);

  CollapsibleGroupBox * icGroupBox = new CollapsibleGroupBox(tr("Advanced Parameters"));
  QGridLayout * paramICLayout = new QGridLayout(icGroupBox);
  icGroupBox->setExpanded(inference_.getEstimateParametersConfidenceInterval());

  int row = 0;
  QLabel * paramICLabel = new QLabel(tr("Estimate parameters confidence interval"));
  paramICLayout->addWidget(paramICLabel, row, 0);

  paramICCheckBox_ = new QCheckBox;
  paramICCheckBox_->setChecked(inference_.getEstimateParametersConfidenceInterval());
  paramICLayout->addWidget(paramICCheckBox_, row, 1);

  paramICLabel = new QLabel(tr("Confidence interval level"));
  paramICLayout->addWidget(paramICLabel, ++row, 0);

  icLevelSpinbox_ = new DoubleSpinBox;
  icLevelSpinbox_->setRange(0.0 + 1.e-6, 1. - 1.e-6);
  icLevelSpinbox_->setValue(inference_.getParametersConfidenceIntervalLevel());
  icLevelSpinbox_->setEnabled(inference_.getEstimateParametersConfidenceInterval());
  paramICLayout->addWidget(icLevelSpinbox_, row, 1);

  label = new QLabel(tr("Lilliefors precision"));
  paramICLayout->addWidget(label, ++row, 0);

  bool enableLilliefors = testTypeComboBox_->currentIndex();
  lillieforsPrecisionSpinbox_ = new DoubleSpinBox;
  lillieforsPrecisionSpinbox_->setRange(0.0 + 1.e-6, 1. - 1.e-6);
  lillieforsPrecisionSpinbox_->setSingleStep(0.01);
  lillieforsPrecisionSpinbox_->setValue(inference_.getLillieforsPrecision());
  lillieforsPrecisionSpinbox_->setEnabled(enableLilliefors);
  paramICLayout->addWidget(lillieforsPrecisionSpinbox_, row, 1);

  label = new QLabel(tr("Lilliefors minimum sampling size"));
  paramICLayout->addWidget(label, ++row, 0);

  lillieforsMinimumSamplingSizeSpinbox_ = new LogSpinBox;
  lillieforsMinimumSamplingSizeSpinbox_->setValue(inference_.getLillieforsMinimumSamplingSize());
  lillieforsMinimumSamplingSizeSpinbox_->setEnabled(enableLilliefors);
  paramICLayout->addWidget(lillieforsMinimumSamplingSizeSpinbox_, row, 1);

  label = new QLabel(tr("Lilliefors maximum sampling size"));
  paramICLayout->addWidget(label, ++row, 0);

  lillieforsMaximumSamplingSizeSpinbox_ = new LogSpinBox;
  lillieforsMaximumSamplingSizeSpinbox_->setValue(inference_.getLillieforsMaximumSamplingSize());
  lillieforsMaximumSamplingSizeSpinbox_->setEnabled(enableLilliefors);
  paramICLayout->addWidget(lillieforsMaximumSamplingSizeSpinbox_, row, 1);

  paramICLayout->setColumnStretch(2, 1);
  pageLayout->addWidget(icGroupBox);

  connect(paramICCheckBox_, &QCheckBox::toggled, [ = ](bool toggled)
  {
    icLevelSpinbox_->setEnabled(toggled);
  });

  connect(testTypeComboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged), [ = ](int index)
  {
    lillieforsPrecisionSpinbox_->setEnabled(index);
    lillieforsMinimumSamplingSizeSpinbox_->setEnabled(index);
    lillieforsMaximumSamplingSizeSpinbox_->setEnabled(index);
  });

  varTableView->selectRow(0);

  // error message
  errorMessageLabel_ = new TemporaryLabel;
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
  errorMessageLabel_->reset();
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


void InferenceWizard::applyCurrentDistToAll()
{
  if (!interestVar_.contains(currentVarName_))
    return;

  const Description varNames(inference_.getDesignOfExperiment().getSample().getDescription());
  FittingTest::DistributionFactoryCollection distCollection(distFactoriesForEachInterestVar_[currentVarName_]);
  for (UnsignedInteger i = 0; i < varNames.getSize(); ++i)
  {
    if (interestVar_.contains(varNames[i]))
    {
      distFactoriesForEachInterestVar_[interestVar_[i]] = distCollection;
      QStringList dist;
      for (UnsignedInteger j = 0; j < distCollection.getSize(); ++j)
      {
        String str = distCollection[j].getImplementation()->getClassName();
        dist << TranslationManager::GetTranslatedDistributionName(str.substr(0, str.find("Factory")));
      }
      DistributionsForInferenceWidget * distWidget = static_cast<DistributionsForInferenceWidget *>(stackWidget_->widget(i));
      distWidget->updateDistributions(dist);
    }
  }
}


void InferenceWizard::updateInterestVar(Description interestVar, String varName)
{
  errorMessageLabel_->reset();
  pageValidity_ = true;

  interestVar_ = interestVar;

  if (!interestVar.getSize())
  {
    pageValidity_ = false;
    errorMessageLabel_->setTemporaryErrorMessage(tr("Select at least one variable"));
  }

  emit currentVarChecked(interestVar.contains(varName));
}


Analysis InferenceWizard::getAnalysis() const
{
  // get the doe
  DesignOfExperiment doe = dynamic_cast<const DesignOfExperimentAnalysis*>(analysis_.getImplementation().get())->getDesignOfExperiment();
  InferenceAnalysis newAnalysis(analysis_.getName(), doe);
  newAnalysis.setInterestVariables(interestVar_);
  newAnalysis.setLevel(levelSpinbox_->value());
  newAnalysis.setEstimateParametersConfidenceInterval(paramICCheckBox_->isChecked());
  newAnalysis.setParametersConfidenceIntervalLevel(icLevelSpinbox_->value());
  newAnalysis.setTestType(testTypeComboBox_->currentIndex());
  if (newAnalysis.getTestType() == InferenceAnalysis::Lilliefors)
  {
    newAnalysis.setLillieforsPrecision(lillieforsPrecisionSpinbox_->value());
    newAnalysis.setLillieforsMinimumSamplingSize(lillieforsMinimumSamplingSizeSpinbox_->value());
    newAnalysis.setLillieforsMaximumSamplingSize(lillieforsMaximumSamplingSizeSpinbox_->value());
  }

  for (UnsignedInteger i = 0; i < interestVar_.getSize(); ++i)
  {
    if (distFactoriesForEachInterestVar_.find(interestVar_[i]) != distFactoriesForEachInterestVar_.end())
    {
      FittingTest::DistributionFactoryCollection factoryCollection(distFactoriesForEachInterestVar_.find(interestVar_[i])->second);
      if (factoryCollection.getSize())
      {
        newAnalysis.setDistributionsFactories(interestVar_[i], factoryCollection);
      }
    }
  }
  return newAnalysis;
}


bool InferenceWizard::validateCurrentPage()
{
  if (!pageValidity_)
    return false;

  if (!interestVar_.getSize())
  {
    errorMessageLabel_->setTemporaryErrorMessage(tr("Select at least one variable"));
    return false;
  }

  for (UnsignedInteger i = 0; i < interestVar_.getSize(); ++i)
  {
    if (distFactoriesForEachInterestVar_.find(interestVar_[i]) != distFactoriesForEachInterestVar_.end())
    {
      FittingTest::DistributionFactoryCollection factoryCollection(distFactoriesForEachInterestVar_.find(interestVar_[i])->second);
      if (!factoryCollection.getSize())
      {
        const QString errorMessage = tr("At least one distribution must be tested for the selected variable '%1'").arg(interestVar_[i].c_str());
        errorMessageLabel_->setTemporaryErrorMessage(errorMessage);
        return false;
      }
    }
  }

  return QWizard::validateCurrentPage();
}
}
