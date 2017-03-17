//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a metamodel analysis
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
#include "otgui/MetaModelAnalysisWizard.hxx"

#include "otgui/DoubleSpinBox.hxx"
#include "otgui/ParametersDefinitionWizard.hxx"

#include "openturns/OTBase.hxx"

#include <QRadioButton>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QToolButton>
#include <QScrollArea>

using namespace OT;

namespace OTGUI {

MetaModelAnalysisWizard::MetaModelAnalysisWizard(const OTStudy& otStudy, const DesignOfExperiment& designOfExperiment, QWidget* parent)
  : AnalysisWizard(FunctionalChaosAnalysis(otStudy.getAvailableAnalysisName("chaos_"), designOfExperiment), parent)
  , chaos_(*dynamic_cast<FunctionalChaosAnalysis*>(&*analysis_.getImplementation()))
  , kriging_(KrigingAnalysis(otStudy.getAvailableAnalysisName("kriging_"), designOfExperiment))
  , errorMessageLabel_(new QLabel)
{
  buildInterface();
}


MetaModelAnalysisWizard::MetaModelAnalysisWizard(const Analysis& analysis, QWidget* parent)
  : AnalysisWizard(analysis, parent)
  , errorMessageLabel_(new QLabel)
{
  if (analysis.getImplementation()->getClassName() == "FunctionalChaosAnalysis")
  {
    chaos_ = *dynamic_cast<const FunctionalChaosAnalysis*>(&*analysis.getImplementation());
    kriging_ = KrigingAnalysis(chaos_.getName(), chaos_.getDesignOfExperiment());
    try
    {
      kriging_.setInterestVariables(chaos_.getInterestVariables());
    }
    catch (std::exception & ex)
    {
      // do nothing
    }
    
    kriging_.setLeaveOneOutValidation(chaos_.isLeaveOneOutValidation());
  }
  else // Kriging
  {
    kriging_ = *dynamic_cast<const KrigingAnalysis*>(&*analysis.getImplementation());
    chaos_ = FunctionalChaosAnalysis(kriging_.getName(), kriging_.getDesignOfExperiment());
    try
    {
      chaos_.setInterestVariables(kriging_.getInterestVariables());
    }
    catch (std::exception & ex)
    {
      // do nothing
    }
    chaos_.setLeaveOneOutValidation(kriging_.isLeaveOneOutValidation());
  }
  buildInterface();
}


void MetaModelAnalysisWizard::buildInterface()
{
  setWindowTitle(tr("Metamodel"));

  QWizardPage * page = new QWizardPage(this);
  QVBoxLayout * pageLayout = new QVBoxLayout(page);

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);

  // output selection
  outputsGroupBox_ = new OutputsSelectionGroupBox(chaos_.getDesignOfExperiment().getOutputSample().getDescription(), chaos_.getInterestVariables(), this);
  setInterestVariables(outputsGroupBox_->getSelectedOutputsNames());
  connect(outputsGroupBox_, SIGNAL(outputsSelectionChanged(QStringList)), this, SLOT(setInterestVariables(QStringList)));
  mainLayout->addWidget(outputsGroupBox_);

  // method selection
  QGroupBox * methodBox = new QGroupBox(tr("Method"));
  QVBoxLayout * methodLayout = new QVBoxLayout(methodBox);

  QButtonGroup * methodGroup = new QButtonGroup;

  QRadioButton * buttonToChooseMethod = new QRadioButton(tr("Functional Chaos"));
  if (analysis_.getImplementation()->getClassName() == "FunctionalChaosAnalysis")
    buttonToChooseMethod->setChecked(true);
  methodGroup->addButton(buttonToChooseMethod, MetaModelAnalysisWizard::chaos);
  methodLayout->addWidget(buttonToChooseMethod);

  buttonToChooseMethod = new QRadioButton(tr("Kriging"));
  if (analysis_.getImplementation()->getClassName() == "KrigingAnalysis")
    buttonToChooseMethod->setChecked(true);
  methodGroup->addButton(buttonToChooseMethod, MetaModelAnalysisWizard::kriging);
  methodLayout->addWidget(buttonToChooseMethod);

  connect(methodGroup, SIGNAL(buttonClicked(int)), this, SLOT(updateMethodWidgets(int)));
  mainLayout->addWidget(methodBox);

  // Chaos parameters group
  chaosParametersBox_ = new QGroupBox(tr("Parameters"));
  QHBoxLayout * chaosParametersLayout = new QHBoxLayout(chaosParametersBox_);

  // -- Polynomial degree
  const UnsignedInteger chaosDegree = chaos_.getChaosDegree();

  chaosParametersLayout->addWidget(new QLabel(tr("Polynomial degree")));
  QSpinBox * chaosDegreeSpinbox = new QSpinBox;
  chaosDegreeSpinbox->setRange(1, 30);
  chaosDegreeSpinbox->setValue(chaosDegree);
  connect(chaosDegreeSpinbox, SIGNAL(valueChanged(int)), this, SLOT(chaosDegreeChanged(int)));
  chaosParametersLayout->addWidget(chaosDegreeSpinbox);

  mainLayout->addWidget(chaosParametersBox_);

  // Kriging parameters group
  krigingParametersBox_ = new QGroupBox(tr("Parameters"));
  QGridLayout * krigingParametersLayout = new QGridLayout(krigingParametersBox_);

  // correlation model
  krigingParametersLayout->addWidget(new QLabel(tr("Covariance model")), 0, 0, 1, 2);
  QComboBox * krigingCorrelationModelComboBox = new QComboBox;
  krigingCorrelationModelComboBox->addItems(QStringList() << tr("Squared exponential")
                                                          << tr("Absolute exponential")
                                                          << tr("Generalized exponential")
                                                          << QString::fromUtf8("Matérn"));
  krigingParametersLayout->addWidget(krigingCorrelationModelComboBox, 0, 1, 1, 2);
  krigingCorrelationModelComboBox->setCurrentIndex(0);
  if (kriging_.getCovarianceModel().getImplementation()->getClassName() == "AbsoluteExponential")
    krigingCorrelationModelComboBox->setCurrentIndex(1);
  else if (kriging_.getCovarianceModel().getImplementation()->getClassName() == "GeneralizedExponential")
    krigingCorrelationModelComboBox->setCurrentIndex(2);
  else if (kriging_.getCovarianceModel().getImplementation()->getClassName() == "MaternModel")
    krigingCorrelationModelComboBox->setCurrentIndex(3);
  connect(krigingCorrelationModelComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCovarianceModel(int)));

  // Matern parameter nu
  //  -label
  maternParameterNuLabel_ = new QLabel(tr("nu"));
  maternParameterNuLabel_->setVisible(krigingCorrelationModelComboBox->currentIndex() == 3);
  krigingParametersLayout->addWidget(maternParameterNuLabel_, 1, 1);

  //  -spinbox
  maternParameterNuSpinBox_ = new DoubleSpinBox;
  maternParameterNuSpinBox_->setMinimum(1e-12);
  maternParameterNuSpinBox_->setVisible(krigingCorrelationModelComboBox->currentIndex() == 3);
  krigingParametersLayout->addWidget(maternParameterNuSpinBox_, 1, 2);
  maternParameterNuSpinBox_->setValue(ResourceMap::GetAsNumericalScalar("MaternModel-DefaultNu"));
  if (kriging_.getCovarianceModel().getImplementation()->getClassName() == "MaternModel")
  {
    double nu = dynamic_cast<MaternModel*>(&*kriging_.getCovarianceModel().getImplementation())->getNu();
    maternParameterNuSpinBox_->setValue(nu);
  }
  connect(maternParameterNuSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(updateMaternParameterNu(double)));

  // GeneralizedExponential parameter P
  //  -label
  generalizedModelParameterPLabel_ = new QLabel(tr("p"));
  generalizedModelParameterPLabel_->setVisible(krigingCorrelationModelComboBox->currentIndex() == 2);
  krigingParametersLayout->addWidget(generalizedModelParameterPLabel_, 2, 1);

  //  -spinbox
  generalizedModelParameterPSpinBox_ = new DoubleSpinBox;
  generalizedModelParameterPSpinBox_->setMinimum(1e-12); // TODO setMaximum with the next OT version
  generalizedModelParameterPSpinBox_->setVisible(krigingCorrelationModelComboBox->currentIndex() == 2);
  krigingParametersLayout->addWidget(generalizedModelParameterPSpinBox_, 2, 2);
  generalizedModelParameterPSpinBox_->setValue(1.0);
  if (kriging_.getCovarianceModel().getImplementation()->getClassName() == "GeneralizedExponential")
  {
    double p = dynamic_cast<GeneralizedExponential*>(&*kriging_.getCovarianceModel().getImplementation())->getP();
    generalizedModelParameterPSpinBox_->setValue(p);
  }
  connect(generalizedModelParameterPSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(updateGeneralizedModelParameterP(double)));

  // basis
  krigingParametersLayout->addWidget(new QLabel(tr("Trend basis type")), 3, 0);
  QComboBox * krigingBasisTypeComboBox = new QComboBox;
  krigingBasisTypeComboBox->addItems(QStringList() << tr("Constant") << tr("Linear") << tr("Quadratic"));
  krigingParametersLayout->addWidget(krigingBasisTypeComboBox, 3, 1, 1, 2);
  krigingBasisTypeComboBox->setCurrentIndex(0);
  const UnsignedInteger dim = kriging_.getBasis().getDimension();
  if (kriging_.getBasis().getSize() == (dim+1))
    krigingBasisTypeComboBox->setCurrentIndex(1);
  else if (kriging_.getBasis().getSize() == ((dim+1)*(dim+2)/2))
    krigingBasisTypeComboBox->setCurrentIndex(2);
  connect(krigingBasisTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateBasis(int)));

  krigingParametersLayout->setColumnStretch(0, 1);
  krigingParametersLayout->setColumnStretch(2, 2);
  mainLayout->addWidget(krigingParametersBox_);

  // Metamodel validation
  QGroupBox * validationGroupBox = new QGroupBox(tr("Validation"));
  QGridLayout * validationLayout = new QGridLayout(validationGroupBox);

  // -- LOO
  QCheckBox * leaveOneOutCheckBox = new QCheckBox;
  leaveOneOutCheckBox->setChecked(chaos_.isLeaveOneOutValidation());
  validationLayout->addWidget(leaveOneOutCheckBox, 0, 0);
  validationLayout->addWidget(new QLabel(tr("Compute Q2 by Leave-one-out")), 0, 1);
  connect(leaveOneOutCheckBox, SIGNAL(toggled(bool)), this, SLOT(looValidationChanged(bool)));
  validationLayout->setColumnStretch(2, 1);
  mainLayout->addWidget(validationGroupBox);


  // Avanced parameters

  // kriging advanced parameters
  krigingAdvancedParamGroupBox_ = new CollapsibleGroupBox;
  krigingAdvancedParamGroupBox_->setTitle(tr("Advanced parameters"));
  QGridLayout * krigingAdvancedParamGroupBox_Layout = new QGridLayout(krigingAdvancedParamGroupBox_);

  // request parameters optimization
  QCheckBox * optimizationCheckBox = new QCheckBox(tr("Optimize covariance model parameters"));
  optimizationCheckBox->setToolTip(tr("Optimize scale and amplitude parameters"));
  optimizationCheckBox->setChecked(kriging_.getOptimizeParameters());
  krigingAdvancedParamGroupBox_Layout->addWidget(optimizationCheckBox,  0, 0, 1, 3);
  connect(optimizationCheckBox, SIGNAL(clicked(bool)), this, SLOT(updateOptimizeParameters(bool)));

  // scale
  krigingAdvancedParamGroupBox_Layout->addWidget(new QLabel(tr("Scale")), 1, 0);

  scaleLineEdit_ = new QLineEdit;
  scaleLineEdit_->setReadOnly(true);
  krigingAdvancedParamGroupBox_Layout->addWidget(scaleLineEdit_, 1, 1);
  updateScaleLineEdit();

  QToolButton * editButton = new QToolButton;
  editButton->setText("...");
  krigingAdvancedParamGroupBox_Layout->addWidget(editButton, 1, 2);
  connect(editButton, SIGNAL(pressed()), this, SLOT(openScaleDefinitionWizard()));

  // amplitude
  // same amplitude for all the outputs
  krigingAdvancedParamGroupBox_Layout->addWidget(new QLabel(tr("Amplitude")), 2, 0);

  amplitudeSpinBox_ = new DoubleSpinBox;
  amplitudeSpinBox_->setMinimum(1e-12);
  krigingAdvancedParamGroupBox_Layout->addWidget(amplitudeSpinBox_, 2, 1, 1, 2);
  amplitudeSpinBox_->setValue(kriging_.getCovarianceModel().getAmplitude()[0]);
  connect(amplitudeSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(updateAmplitude(double)));

  krigingAdvancedParamGroupBox_->setExpanded(false);
  mainLayout->addWidget(krigingAdvancedParamGroupBox_);

  // functional chaos advanced parameters
  chaosAdvancedParamGroupBox_ = new CollapsibleGroupBox;
  chaosAdvancedParamGroupBox_->setTitle(tr("Advanced parameters"));
  QGridLayout * chaosAdvancedParamGroupBox_Layout = new QGridLayout(chaosAdvancedParamGroupBox_);

  QCheckBox * sparseCheckBox_ = new QCheckBox(tr("Sparse"));
  sparseCheckBox_->setChecked(chaos_.getSparseChaos());
  connect(sparseCheckBox_, SIGNAL(toggled(bool)), this, SLOT(sparseChaosChanged(bool)));
  chaosAdvancedParamGroupBox_Layout->addWidget(sparseCheckBox_, 0, 0);

  chaosAdvancedParamGroupBox_->setExpanded(false);
  mainLayout->addWidget(chaosAdvancedParamGroupBox_);


  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);

  mainLayout->addStretch();
  mainLayout->addWidget(errorMessageLabel_);

  // set widgets
  updateMethodWidgets(methodGroup->checkedId());

  scrollArea->setWidget(mainWidget);
  pageLayout->addWidget(scrollArea);

  addPage(page);
}


void MetaModelAnalysisWizard::updateScaleLineEdit()
{
  QString scaleText;
  for (UnsignedInteger i=0; i<kriging_.getCovarianceModel().getScale().getSize(); ++i)
  {
    scaleText += QString::number(kriging_.getCovarianceModel().getScale()[i]);
    if (i < kriging_.getCovarianceModel().getScale().getSize()-1)
      scaleText += "; ";
  }
  scaleLineEdit_->setText(scaleText);
}


void MetaModelAnalysisWizard::updateMethodWidgets(int id)
{
  chaosParametersBox_->setVisible(id == 0);
  chaosAdvancedParamGroupBox_->setVisible(id == 0);

  krigingParametersBox_->setVisible(id == 1);
  krigingAdvancedParamGroupBox_->setVisible(id == 1);
}


void MetaModelAnalysisWizard::chaosDegreeChanged(int degree)
{
  chaos_.setChaosDegree(degree);
}


void MetaModelAnalysisWizard::sparseChaosChanged(bool sparse)
{
  chaos_.setSparseChaos(sparse);
}


void MetaModelAnalysisWizard::looValidationChanged(bool looValidation)
{
  chaos_.setLeaveOneOutValidation(looValidation);
  kriging_.setLeaveOneOutValidation(looValidation);
}


void MetaModelAnalysisWizard::setInterestVariables(QStringList outputsList)
{
  errorMessageLabel_->setText("");

  Description desc(outputsList.size());
  for (int i=0; i<outputsList.size(); ++i)
    desc[i] = outputsList[i].toUtf8().constData();

  try
  {
    chaos_.setInterestVariables(desc);
    kriging_.setInterestVariables(desc);
  }
  catch (InvalidDimensionException exception)
  {
    // check in validateCurrentPage
  }
}


void MetaModelAnalysisWizard::updateCovarianceModel(int modelType)
{
  const NumericalPoint scale(kriging_.getCovarianceModel().getScale());
  const NumericalPoint amplitude(kriging_.getCovarianceModel().getAmplitude());

  switch (modelType)
  {
    case 0: // SquaredExponential
      kriging_.setCovarianceModel(SquaredExponential(scale, amplitude));
      break;
    case 1: // AbsoluteExponential
      kriging_.setCovarianceModel(AbsoluteExponential(scale, amplitude));
      break;
    case 2: // GeneralizedExponential
      kriging_.setCovarianceModel(GeneralizedExponential(scale, amplitude, generalizedModelParameterPSpinBox_->value()));
      break;
    case 3: // MaternModel
      kriging_.setCovarianceModel(MaternModel(scale, amplitude, maternParameterNuSpinBox_->value()));
      break;
    default:
      throw InvalidArgumentException(HERE) << "The covariance model type is not recognized";
  }
  generalizedModelParameterPLabel_->setVisible(modelType == 2);
  generalizedModelParameterPSpinBox_->setVisible(modelType == 2);
  maternParameterNuLabel_->setVisible(modelType == 3);
  maternParameterNuSpinBox_->setVisible(modelType == 3);
}


void MetaModelAnalysisWizard::updateOptimizeParameters(bool optimize)
{
  kriging_.setOptimizeParameters(optimize);
}


void MetaModelAnalysisWizard::updateAmplitude(double amplitude)
{
  CovarianceModel model(kriging_.getCovarianceModel());
  model.setAmplitude(NumericalPoint(1, amplitude));
  kriging_.setCovarianceModel(model);
}


void MetaModelAnalysisWizard::updateMaternParameterNu(double nu)
{
  const NumericalPoint scale(kriging_.getCovarianceModel().getScale());
  const NumericalPoint amplitude(kriging_.getCovarianceModel().getAmplitude());
  try
  {
    kriging_.setCovarianceModel(MaternModel(scale, amplitude, nu));
  }
  catch (std::exception& ex)
  {
    // do nothing
  }
}


void MetaModelAnalysisWizard::updateGeneralizedModelParameterP(double p)
{
  const NumericalPoint scale(kriging_.getCovarianceModel().getScale());
  const NumericalPoint amplitude(kriging_.getCovarianceModel().getAmplitude());
  try
  {
    kriging_.setCovarianceModel(GeneralizedExponential(scale, amplitude, p));
  }
  catch (std::exception& ex)
  {
    // do nothing
  }
}


void MetaModelAnalysisWizard::updateBasis(int basisType)
{
  const UnsignedInteger inputDimension = kriging_.getEffectiveInputSample().getDimension();

  switch (basisType)
  {
    case 0: // Constant
      kriging_.setBasis(ConstantBasisFactory(inputDimension).build());
      break;
    case 1: // Linear
      kriging_.setBasis(LinearBasisFactory(inputDimension).build());
      break;
    case 2: // Quadratic
      kriging_.setBasis(QuadraticBasisFactory(inputDimension).build());
      break;
    default:
      throw InvalidArgumentException(HERE) << "The basis type is not recognized";
  }
}


void MetaModelAnalysisWizard::openScaleDefinitionWizard()
{
  const Description inputsNames(kriging_.getEffectiveInputSample().getDescription());
  const QStringList labels = QStringList() << tr("Input") << tr("Scale");
  ParametersDefinitionWizard * paramDefinitionWizard = new ParametersDefinitionWizard(inputsNames, kriging_.getCovarianceModel().getScale(), labels, this);
  paramDefinitionWizard->setWindowTitle(tr("Scale"));

  if (paramDefinitionWizard->exec())
  {
    CovarianceModel covModel(kriging_.getCovarianceModel());
    covModel.setScale(paramDefinitionWizard->getValues());
    kriging_.setCovarianceModel(covModel);
    updateScaleLineEdit();
  }
}


bool MetaModelAnalysisWizard::validateCurrentPage()
{
  errorMessageLabel_->setText("");
  if (!outputsGroupBox_->getSelectedOutputsNames().size())
  {
    errorMessageLabel_->setText(QString("%1%2%3").arg("<font color=red>").arg(tr("At least one output must be selected")).arg("</font>"));
    return false;
  }
  if (chaosParametersBox_->isVisible())
  {
    const UnsignedInteger n = chaos_.getChaosDegree() + chaos_.getDesignOfExperiment().getInputSample().getDimension();
    // compute binomial coefficient
    const UnsignedInteger minimumSize  = FunctionalChaosAnalysis::BinomialCoefficient(n, chaos_.getChaosDegree());
    if (chaos_.getDesignOfExperiment().getInputSample().getSize() < minimumSize)
    {
      QString errorMessage = tr("Design of experiment size too small : %1. It must be superior or equal to C(degree+nbInputs, degree) = %2").arg(chaos_.getDesignOfExperiment().getInputSample().getSize()).arg(minimumSize);
      errorMessageLabel_->setText(QString("%1%2%3").arg("<font color=red>").arg(errorMessage).arg("</font>"));
      return false;
    }
    analysis_ = chaos_;
  }
  else
  {
    analysis_ = kriging_;
  }

  return QWizard::validateCurrentPage();
}
}
