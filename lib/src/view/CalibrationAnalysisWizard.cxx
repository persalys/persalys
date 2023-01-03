//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define simulation reliability analysis
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
#include "persalys/CalibrationAnalysisWizard.hxx"

#include "persalys/DistributionDictionary.hxx"
#include "persalys/MarginalsWidget.hxx"
#include "persalys/DependenciesWidget.hxx"
#include "persalys/SymbolicPhysicalModel.hxx"
#include "persalys/SpinBoxDelegate.hxx"
#include "persalys/CheckableHeaderView.hxx"
#include "persalys/QtTools.hxx"

#include <openturns/Normal.hxx>
#include <openturns/Dirac.hxx>
#include <openturns/NormalCopula.hxx>

#include <QVBoxLayout>
#include <QRadioButton>
#include <QGroupBox>

using namespace OT;

namespace PERSALYS
{

CalibrationIntroPage::CalibrationIntroPage(QWidget * parent)
  : QWizardPage(parent)
  , observationsComboBox_(0)
  , observationsComboBoxModel_(0)
  , observationsLabel_(0)
  , methodGroup_(0)
{
  setTitle(tr("Calibration parameters"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  // choose the limit state
  QGroupBox * observationsBox = new QGroupBox(tr("Observations"));
  QVBoxLayout * observationsLayout = new QVBoxLayout(observationsBox);

  observationsComboBox_ = new QComboBox;
  observationsLayout->addWidget(observationsComboBox_);
  connect(observationsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(changeObservationsLabel(int)));
  observationsComboBoxModel_ = new QStandardItemModel(observationsComboBox_);
  observationsComboBox_->setModel(observationsComboBoxModel_);

  observationsLabel_ = new QLabel;
//   observationsLabel_->setObjectName("observationsLabel_");
  observationsLayout->addWidget(observationsLabel_);

  pageLayout->addWidget(observationsBox);

  // choose the method
  QGroupBox * methodBox = new QGroupBox(tr("Method"));
  QVBoxLayout * methodLayout = new QVBoxLayout(methodBox);

  methodGroup_ = new QButtonGroup(this);


  // - Linear least squares
  QRadioButton * linearLeastSquaresButton = new QRadioButton(tr("Linear least squares"));
  registerField("linearLeastSquaresButton", linearLeastSquaresButton);
  linearLeastSquaresButton->setChecked(true);
  methodGroup_->addButton(linearLeastSquaresButton, CalibrationIntroPage::LinearLeastSquares);
  methodLayout->addWidget(linearLeastSquaresButton);

  // - Nonlinear least squares
  QRadioButton * nonLinearLeastSquaresButton = new QRadioButton(tr("Nonlinear least squares"));
  registerField("nonLinearLeastSquaresButton", nonLinearLeastSquaresButton);
  methodGroup_->addButton(nonLinearLeastSquaresButton, CalibrationIntroPage::NonlinearLeastSquares);
  methodLayout->addWidget(nonLinearLeastSquaresButton);

  // - Linear Gaussian
  QRadioButton * linearGaussianButton = new QRadioButton(tr("Linear Gaussian") + " (BLUE)");
  registerField("linearGaussianButton", linearGaussianButton);
  methodGroup_->addButton(linearGaussianButton, CalibrationIntroPage::LinearGaussian);
  methodLayout->addWidget(linearGaussianButton);

  // - Nonlinear Gaussian
  QRadioButton * nonLinearGaussianButton = new QRadioButton(tr("Nonlinear Gaussian") + " (3DVAR)");
  registerField("nonLinearGaussianButton", nonLinearGaussianButton);
  methodGroup_->addButton(nonLinearGaussianButton, CalibrationIntroPage::NonlinearGaussian);
  methodLayout->addWidget(nonLinearGaussianButton);

  connect(methodGroup_, static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), [=](QAbstractButton * button){emit methodChanged(methodGroup_->id(button));});

  pageLayout->addWidget(methodBox);
}


void CalibrationIntroPage::initialize(const Analysis& analysis, QList<DesignOfExperiment> observationsList)
{
  // limit state
  for (int i = 0; i < observationsList.count(); ++i)
  {
    QStandardItem * item = new QStandardItem(observationsList[i].getName().c_str());
    item->setData(QVariant::fromValue(observationsList[i]));
    observationsComboBoxModel_->appendRow(item);
  }
  const CalibrationAnalysis * analysis_ptr = dynamic_cast<const CalibrationAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
    return;

  // method
  const String methodName = analysis_ptr->getMethodName();
  const Description names(CalibrationAnalysis::GetMethodNames());
  const Description::const_iterator it = std::find(names.begin(), names.end(), methodName);
  const UnsignedInteger index = it - names.begin();
  methodGroup_->button(index)->click();
}


DesignOfExperiment CalibrationIntroPage::getObservations() const
{
  const int itemRow = observationsComboBox_->currentIndex();
  if (itemRow < 0)
    return DesignOfExperiment();

  const QVariant variant = observationsComboBoxModel_->item(itemRow)->data();
  if (variant.canConvert<DesignOfExperiment>())
    return variant.value<DesignOfExperiment>();
  return DesignOfExperiment();
}


int CalibrationIntroPage::getMethodId() const
{
  return methodGroup_->checkedId();
}


void CalibrationIntroPage::changeObservationsLabel(int index)
{
  if (index < 0)
    return;

  const QVariant variant = observationsComboBoxModel_->item(index)->data();
  if (variant.canConvert<DesignOfExperiment>())
  {
    const DesignOfExperiment observations(variant.value<DesignOfExperiment>());
    emit observationsChanged(observations);
    QString text(tr("Observed variables") + " : " );
    text += observations.getSample().getDescription().__str__().c_str();
    text += "\n" + tr("Number of observations") + " : " + QString::number(observations.getSample().getSize());
    observationsLabel_->setText(text);
  }
}


// -----------------------------------------------------


CalibrationReferencePointPage::CalibrationReferencePointPage(/*const PhysicalModel& model,*/ QWidget *parent)
  : QWizardPage(parent)
  , tableModel_(0)
{
  setSubTitle(tr("Choose the input variables to calibrate and define the reference point of the algorithm"));
  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  QGridLayout * labelsLayout = new QGridLayout;
  labelsLayout->addWidget(new QLabel(tr("Number of inputs to calibrate") + " : "), 0, 0);
  calibratedLabel_ = new QLabel;
  labelsLayout->addWidget(calibratedLabel_, 0, 1, Qt::AlignLeft);
  pageLayout->addLayout(labelsLayout);
  labelsLayout->setColumnStretch(2, 1);

  tableView_ = new ExportableTableView;
  tableView_->setEditTriggers(QTableView::AllEditTriggers);
  // table header view
  CheckableHeaderView * outputTableHeaderView = new CheckableHeaderView;
  outputTableHeaderView->setStretchLastSection(true);
  tableView_->setHorizontalHeader(outputTableHeaderView);
  // spinbox delegate column 2
  SpinBoxDelegate * spinBoxDelegate = new SpinBoxDelegate(tableView_);
  spinBoxDelegate->setSpinBoxType(SpinBoxDelegate::doubleValue);
  tableView_->setItemDelegateForColumn(2, spinBoxDelegate);
  tableView_->setSelectionMode(QAbstractItemView::NoSelection);
  tableModel_ = new CalibrationTableModel(tableView_);
  connect(tableModel_, SIGNAL(modelChanged(PhysicalModel)), this, SIGNAL(modelChanged(PhysicalModel)));
  connect(tableModel_, &CalibrationTableModel::modelChanged, [=](const PhysicalModel &model) {calibratedLabel_->setText(QString::number(model.getStochasticInputNames().getSize()));});
  tableView_->setModel(tableModel_);
  pageLayout->addWidget(tableView_, 0, Qt::AlignTop);

  errorMessageLabel_ = new TemporaryLabel;
  connect(tableModel_, SIGNAL(modelChanged(PhysicalModel)), errorMessageLabel_, SLOT(reset()));
  pageLayout->addWidget(errorMessageLabel_, 0, Qt::AlignBottom);
}


void CalibrationReferencePointPage::updateData(const PhysicalModel &model)
{
  tableModel_->updateData(model);
  tableView_->resizeColumnsToContents();
  calibratedLabel_->setText(QString::number(model.getStochasticInputNames().getSize()));

  // resize table
  // if too many variables: no fixed height + use scrollbar
  if (tableView_->model()->rowCount() < RowNumberToScrollTable)
  {
    tableView_->resizeWithOptimalHeight();
  }
}


PointWithDescription CalibrationReferencePointPage::getFixedValues() const
{
  return tableModel_->getFixedValues();
}


PointWithDescription CalibrationReferencePointPage::getCalibratedInputs() const
{
  return tableModel_->getCalibratedInputs();
}


bool CalibrationReferencePointPage::validatePage()
{
  // check
  if (!tableModel_->getCalibratedInputs().getSize())
  {
    errorMessageLabel_->setErrorMessage(tr("Define at least a variable to calibrate"));
    return false;
  }

  return QWizardPage::validatePage();
}

// -----------------------------------------------------


CalibrationDistributionPage::CalibrationDistributionPage(const QString &title, const QString &subTitle, QWidget *parent)
  : QWizardPage(parent)
{
  setTitle(title);
  setSubTitle(subTitle);
  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  MarginalsWidget * marginalsWidget = new MarginalsWidget(model_, this);
  DependenciesWidget * dependenciesWidget = new DependenciesWidget(model_, this);
  QTabWidget * distTab = new QTabWidget;

  // First Tab: marginals
  distTab->addTab(marginalsWidget, tr("Marginals"));

  // Second Tab: dependencies
  distTab->addTab(dependenciesWidget, tr("Dependence"));
  connect(marginalsWidget, SIGNAL(updateDependenciesRequested()), dependenciesWidget, SLOT(updateWidgets()));
  connect(this, &CalibrationDistributionPage::updateWidgetRequested, marginalsWidget, &MarginalsWidget::updateProbabilisticModel);
  connect(this, &CalibrationDistributionPage::updateWidgetRequested, dependenciesWidget, &DependenciesWidget::updateWidgets);

  pageLayout->addWidget(distTab);
}


void CalibrationDistributionPage::updateData(const PhysicalModel &model)
{
  InputCollection inputs;
  for (UnsignedInteger i = 0; i < model.getInputDimension(); ++i)
    if (model.getInputs()[i].isStochastic())
      inputs.add(model.getInputs()[i]);
  model_.setInputs(inputs);

  if (!model.getDistribution().hasIndependentCopula())
  {
    for (UnsignedInteger i = 0; i < model.getCopulaCollection().getSize(); ++i)
      if (model.getCopulaCollection()[i].getImplementation()->getClassName() != "IndependentCopula")
        model_.setCopula(model.getCopulaCollection()[i].getDescription(), model.getCopulaCollection()[i]);
  }
  emit updateWidgetRequested();
}


Distribution CalibrationDistributionPage::getDistribution() const
{
  Distribution distribution(model_.getDistribution());
  if (distribution.getDimension() == model_.getStochasticInputNames().getSize())
    distribution.setDescription(model_.getStochasticInputNames());
  return distribution;
}

// -----------------------------------------------------


CalibrationParametersPage::CalibrationParametersPage(QWidget *parent)
  : QWizardPage(parent)
  , stoppingCriteriaLayout_(0)
{
  buildInterface();
}


void CalibrationParametersPage::buildInterface()
{
  setTitle(tr("Parameters"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  // confidence interval
  QGroupBox * ciGroup = new QGroupBox(tr("Confidence interval of the posterior distribution"));
  QVBoxLayout * ciGroupLayout = new QVBoxLayout(ciGroup);

  QHBoxLayout * ciLengthLayout = new QHBoxLayout;
  QLabel * ciLengthLabel = new QLabel(tr("Confidence interval length"));
  ciLengthLayout->addWidget(ciLengthLabel);
  ciLengthSpinbox_ = new DoubleSpinBox;
  ciLengthSpinbox_->setRange(0.0, 0.99);
  ciLengthSpinbox_->setSingleStep(0.01);
  ciLengthLayout->addWidget(ciLengthSpinbox_, 1, Qt::AlignLeft);
  ciGroupLayout->addLayout(ciLengthLayout);

  bootStrapWidget_ = new QWidget;
  QGridLayout * bootStrapLayout = new QGridLayout(bootStrapWidget_);
  bootStrapLayout->setContentsMargins(0, 0, 0, 0);
  bootstrapCheckBox_ = new QCheckBox(bootStrapWidget_);
  bootStrapLayout->addWidget(bootstrapCheckBox_, 0, 0);
  bootStrapLayout->addWidget(new QLabel(tr("Estimation by Bootstrap resampling")), 0, 1);

  QHBoxLayout * sampleSizeLayout = new QHBoxLayout;
  QLabel * sampleSizeLabel = new QLabel(tr("Sample size"));
  sampleSizeLayout->addWidget(sampleSizeLabel);
  bootStrapSizeSpinBox_ = new UIntSpinBox;
  bootStrapSizeSpinBox_->setMinimum(1);
  bootStrapSizeSpinBox_->setSingleStep(5);;
  connect(bootstrapCheckBox_, SIGNAL(clicked(bool)), bootStrapSizeSpinBox_, SLOT(setEnabled(bool)));
  sampleSizeLayout->addWidget(bootStrapSizeSpinBox_);
  bootStrapLayout->addLayout(sampleSizeLayout, 1, 1);

  bootStrapLayout->setColumnStretch(2, 1);
  ciGroupLayout->addWidget(bootStrapWidget_);

  pageLayout->addWidget(ciGroup);

  // optimization stopping criteria --------------------
  CollapsibleGroupBox * optimGroup = new CollapsibleGroupBox;
  optimGroup->setTitle(tr("Optimization stopping criteria"));
  stoppingCriteriaLayout_ = new OptimizationStoppingCriteria(optimGroup);
  pageLayout->addWidget(optimGroup);

  // maximum number of evaluations
  maxEvalWidget_ = new QWidget;
  QHBoxLayout * maxEvalLayout = new QHBoxLayout(maxEvalWidget_);
  QLabel * maxEvalLabel = new QLabel(tr("Maximum number of evaluations"));
  maxEvalLabel->setToolTip("= " + tr("Bootstrap_size * optimization_maximum_number_of_evaluations"));
  maxEvalLayout->addWidget(maxEvalLabel);
  maxNbEvalLabel_ = new QLabel;
  maxEvalLayout->addWidget(maxNbEvalLabel_);
  connect(bootStrapSizeSpinBox_, QOverload<double>::of(&UIntSpinBox::valueChanged), this, &CalibrationParametersPage::updateLabel);
  connect(stoppingCriteriaLayout_->getEvaluationSpinBox(), QOverload<double>::of(&UIntSpinBox::valueChanged), this, &CalibrationParametersPage::updateLabel);
  maxEvalLayout->setContentsMargins(0, 0, 0, 0);
  maxEvalLayout->addStretch();
  pageLayout->addWidget(maxEvalWidget_);
  connect(bootstrapCheckBox_, &QCheckBox::clicked, maxEvalWidget_, &QWidget::setVisible);
}


void CalibrationParametersPage::initialize(const Analysis &analysis)
{
  const CalibrationAnalysis * analysis_ptr = dynamic_cast<const CalibrationAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
    return;

  OptimizationAlgorithm solver = analysis_ptr->getOptimizationAlgorithm();

  // initialize widgets
  const UnsignedInteger BSsize = analysis_ptr->getBootStrapSize();
  ciLengthSpinbox_->setValue(analysis_ptr->getConfidenceIntervalLength());
  bootstrapCheckBox_->setChecked(BSsize > 0);
  bootStrapSizeSpinBox_->setEnabled(BSsize > 0);
  bootStrapSizeSpinBox_->setValue(BSsize > 0 ? BSsize : 100);
  maxEvalWidget_->setVisible(BSsize > 0);

  stoppingCriteriaLayout_->initialize(solver);
  updateLabel();
}


void CalibrationParametersPage::updateAnalysis(CalibrationAnalysis *analysis_ptr)
{
  analysis_ptr->setConfidenceIntervalLength(ciLengthSpinbox_->value());
  OptimizationAlgorithm optimAlgo(analysis_ptr->getOptimizationAlgorithm());
  stoppingCriteriaLayout_->updateAlgorithm(optimAlgo);
  analysis_ptr->setOptimizationAlgorithm(optimAlgo);
  analysis_ptr->setBootStrapSize(bootstrapCheckBox_->isChecked() ? bootStrapSizeSpinBox_->value() : 0);
}


void CalibrationParametersPage::initializePage()
{
  const bool showOptimWidget = field("nonLinearGaussianButton").toBool() || field("nonLinearLeastSquaresButton").toBool();
  bootStrapWidget_->setVisible(showOptimWidget);
  stoppingCriteriaLayout_->parentWidget()->setVisible(showOptimWidget);
  maxEvalWidget_->setVisible(showOptimWidget && bootstrapCheckBox_->isChecked());
}


void CalibrationParametersPage::updateLabel()
{
  maxNbEvalLabel_->setText(QString::number(bootStrapSizeSpinBox_->value() * stoppingCriteriaLayout_->getEvaluationSpinBox()->value()));
}

// -----------------------------------------------------

CalibrationAnalysisWizard::CalibrationAnalysisWizard(const Analysis &analysis, const bool isGeneralWizard, QWidget *parent)
  : AnalysisWizard(analysis, parent)
  , observationsList_()
  , introPage_(0)
  , refPointPage_(0)
  , priorDistPage_(0)
  , obsErrorDistPage_(0)
  , paramPage_(0)
{
  const DesignOfExperiment observations(dynamic_cast<CalibrationAnalysis*>(analysis_.getImplementation().get())->getObservations());

  if (isGeneralWizard)
  {
    if (Observer * obs = observations.getImplementation().get()->getObserver("Study"))
    {
      StudyImplementation * study = dynamic_cast<StudyImplementation*>(obs);
      Q_ASSERT(study);
      for (UnsignedInteger i = 0; i < study->getDataModels().getSize(); ++i)
      {
        if (study->getDataModels()[i].hasPhysicalModel())
        {
          const DesignOfExperiment observations_i(study->getDataModels()[i]);
          if (observations_i.getPhysicalModel() == observations.getPhysicalModel())
            observationsList_.append(observations_i);
        }
      }
    }
  }
  else
  {
    observationsList_.append(observations);
  }
  buildInterface();
}


void CalibrationAnalysisWizard::buildInterface()
{
  setWindowTitle(tr("Calibration"));
  docLink_ = "user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#calibrationwizard";

  // First Page: choose the observations and the method of calibration analysis
  introPage_ = new CalibrationIntroPage(this);
  introPage_->initialize(analysis_, observationsList_);
  setPage(Page_Intro, introPage_);
  connect(introPage_, SIGNAL(observationsChanged(DesignOfExperiment)), this, SLOT(updatePages(DesignOfExperiment)));

  // Second Page: choose the calibrated variables
  refPointPage_ = new CalibrationReferencePointPage(this);
  setPage(Page_RefPoint, refPointPage_);

  // third page : prior distribution
  priorDistPage_ = new CalibrationDistributionPage(tr("Gaussian prior distribution"),
                                                   tr("Define the covariance matrix of the parameters θ to calibrate."),
                                                   this);

  connect(refPointPage_, SIGNAL(modelChanged(PhysicalModel)), priorDistPage_, SLOT(updateData(PhysicalModel)));
  setPage(Page_PriorDist, priorDistPage_);


  // fourth page : observations error distribution
  obsErrorDistPage_ = new CalibrationDistributionPage(tr("Gaussian distribution of the output observations error"),
                                                      tr("Define the covariance matrix of the output observations error."),
                                                      this);
  setPage(Page_ObsErrorDist, obsErrorDistPage_);

  initializePages();

  // fifth page : parameters
  paramPage_ = new CalibrationParametersPage(this);
  paramPage_->initialize(analysis_);
  setPage(Page_Param, paramPage_);

  setStartId(Page_Intro);
}


void CalibrationAnalysisWizard::initializePages()
{
  const CalibrationAnalysis * analysis_ptr = dynamic_cast<const CalibrationAnalysis*>(analysis_.getImplementation().get());

  // - define Normal distribution
  InputCollection inputs;
  const Description observedInputs(analysis_ptr->getObservations().getInputSample().getDescription());
  const Description calibratedInputs(analysis_ptr->getPriorDistribution().getDescription());
  const Description fixedInputs(analysis_ptr->getFixedInputs().getDescription());
  for (UnsignedInteger i = 0; i < analysis_ptr->getPhysicalModel().getInputDimension(); ++i)
  {
    Input input(analysis_ptr->getPhysicalModel().getInputs()[i]);
    const String inputName(input.getName());

    // input to calibrate
    Description::const_iterator it = std::find(calibratedInputs.begin(), calibratedInputs.end(), inputName);
    if (it != calibratedInputs.end())
    {
      const UnsignedInteger j = it - calibratedInputs.begin();
      const Distribution dist_ind(analysis_ptr->getPriorDistribution().getMarginal(j));
      const Scalar value = dist_ind.getMean()[0];
      if (dist_ind.getImplementation()->getClassName() == "Normal")
        input.setDistribution(dist_ind);
      else
        input.setDistribution(DistributionDictionary::BuildDistribution("Normal", value));
      input.setValue(value);
      inputs.add(input);
      continue;
    }
    // fixed input
    it = std::find(fixedInputs.begin(), fixedInputs.end(), inputName);
    if (it != fixedInputs.end())
    {
      const UnsignedInteger j = it - fixedInputs.begin();
      const Scalar value = analysis_ptr->getFixedInputs()[j];
      const Distribution normal(DistributionDictionary::BuildDistribution("Normal", value));
      input.setDistribution(normal);
      input.setValue(value);
      input.setStochastic(false);
      inputs.add(input);
      continue;
    }
    // if observed input : do nothing
    // else : input is calibrated
    it = std::find(observedInputs.begin(), observedInputs.end(), inputName);
    if (it == observedInputs.end())
    {
      input.setDistribution(DistributionDictionary::BuildDistribution("Normal", input.getValue()));
      inputs.add(input);
    }
  }

  // fake model
  SymbolicPhysicalModel model;
  model.setInputs(inputs);
  if (!analysis_ptr->getPriorDistribution().hasIndependentCopula())
  {
    // build a NormalCopula
    // cannot take only priorDistribution.getCopula() because pm.setCopula
    // does not manage MarginalDistribution and ComposedCopula
    // We get first the indices of the correlated inputs
    CorrelationMatrix m(analysis_ptr->getPriorDistribution().getSpearmanCorrelation());
    QSet<int> ind;
    for (UnsignedInteger i = 0; i < m.getDimension(); ++i)
    {
      for (UnsignedInteger j = i + 1; j < m.getDimension(); ++j)
      {
        if (m(i, j) != 0)
        {
          ind << i;
          ind << j;
        }
      }
    }
    QList<int> indices(ind.values());
    CorrelationMatrix m2(ind.size());
    Description correlatedInputs(ind.size());
    for (UnsignedInteger i = 0; i < m2.getDimension(); ++i)
    {
      correlatedInputs[i] = calibratedInputs[indices[i]];
      for (UnsignedInteger j = i + 1; j < m2.getDimension(); ++j)
        m2(i, j) = m(indices[i], indices[j]);
    }
    model.setCopula(correlatedInputs, NormalCopula(NormalCopula::GetCorrelationFromSpearmanCorrelation(m2)));
  }

  // update the table of the reference point page and the prior distribution page
  refPointPage_->updateData(model);
  priorDistPage_->updateData(model);

  // define a fake model to build a probabilistic model window
  const Description observedOutput(analysis_ptr->getObservations().getOutputSample().getDescription());
  SymbolicPhysicalModel outputModel;
  for (UnsignedInteger i = 0; i < observedOutput.getSize(); ++i)
    outputModel.addInput(Input(observedOutput[i], Normal(0, std::sqrt(analysis_ptr->getErrorCovariance()(i, i)))));
  obsErrorDistPage_->updateData(outputModel);
}


// slot : keep argument because DOE can be changed (on IntroPage)
void CalibrationAnalysisWizard::updatePages(const DesignOfExperiment &DOE)
{
  // fake objects to reuse existing wizard pages
  const CalibrationAnalysis * analysis_ptr = dynamic_cast<const CalibrationAnalysis*>(analysis_.getImplementation().get());

  const Description observedVariables(DOE.getInputSample().getDescription());
  // - define Normal distribution
  InputCollection inputs;
  for (UnsignedInteger i = 0; i < analysis_ptr->getPhysicalModel().getInputDimension(); ++i)
  {
    Input input(analysis_ptr->getPhysicalModel().getInputs()[i]);
    if (!observedVariables.contains(input.getName()))
    {
      // by default, input to calibrate
      const Distribution normal(DistributionDictionary::BuildDistribution("Normal", input.getValue()));
      input.setDistribution(normal);
      inputs.add(input);
    }
  }
  SymbolicPhysicalModel model;
  model.setInputs(inputs);

  // update the table of the reference point page and the prior distribution page
  refPointPage_->updateData(model);
  priorDistPage_->updateData(model);

  // define a fake model to build a probabilistic model window
  const Description observedOutput(DOE.getOutputSample().getDescription());
  SymbolicPhysicalModel outputModel;
  for (UnsignedInteger i = 0; i < observedOutput.getSize(); ++i)
    outputModel.addInput(Input(observedOutput[i], Normal(0, 0.1)));
  obsErrorDistPage_->updateData(outputModel);
}


int CalibrationAnalysisWizard::nextId() const
{
  switch (currentId())
  {
    case Page_Intro:
      return Page_RefPoint;
    case Page_RefPoint:
      if (field("nonLinearGaussianButton").toBool() || field("linearGaussianButton").toBool())
        return Page_PriorDist;
      else
        return Page_Param;
    case Page_PriorDist:
      return Page_ObsErrorDist;
    case Page_ObsErrorDist:
      return Page_Param;
    default:
      return -1;
  }
}


Analysis CalibrationAnalysisWizard::getAnalysis() const
{
  CalibrationAnalysis calibration(analysis_.getName(), introPage_->getObservations());
  calibration.setMethodName(calibration.GetMethodNames()[introPage_->getMethodId()]);
  // if gaussian algorithm
  if (field("nonLinearGaussianButton").toBool() || field("linearGaussianButton").toBool())
  {
    calibration.setCalibratedInputs(priorDistPage_->getDistribution().getDescription(),
                                    priorDistPage_->getDistribution(),
                                    refPointPage_->getFixedValues().getDescription(),
                                    refPointPage_->getFixedValues()
                                   );
    calibration.setErrorCovariance(obsErrorDistPage_->getDistribution().getCovariance());
  }
  // if least squares algorithm
  else
  {
    ComposedDistribution distribution(ComposedDistribution::DistributionCollection(refPointPage_->getCalibratedInputs().getSize(), Dirac()));
    distribution.setParameter(refPointPage_->getCalibratedInputs());
    calibration.setCalibratedInputs(refPointPage_->getCalibratedInputs().getDescription(),
                                    distribution,
                                    refPointPage_->getFixedValues().getDescription(),
                                    refPointPage_->getFixedValues()
                                   );
  }
  paramPage_->updateAnalysis(&calibration);

  return calibration;
}
}
