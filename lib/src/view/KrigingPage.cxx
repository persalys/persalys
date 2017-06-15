//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define a kriging analysis
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
#include "otgui/MetaModelAnalysisWizard.hxx"

#include "otgui/ParametersDefinitionWizard.hxx"
#include "otgui/CollapsibleGroupBox.hxx"
#include "otgui/KrigingAnalysis.hxx"

#include <openturns/OTBase.hxx>

#include <QToolButton>
#include <QVBoxLayout>

using namespace OT;

namespace OTGUI {

KrigingPage::KrigingPage(QWidget* parent)
  : QWizardPage(parent)
  , covarianceModelComboBox_(0)
  , maternParameterNuLabel_(0)
  , maternParameterNuSpinBox_(0)
  , generalizedModelParameterPLabel_(0)
  , generalizedModelParameterPSpinBox_(0)
  , basisTypeComboBox_(0)
  , leaveOneOutCheckBox_(0)
  , optimizationCheckBox_(0)
  , scaleLineEdit_(0)
  , inputsNames_()
  , scales_()
  , amplitudeSpinBox_(0)
{
  buildInterface();
}


void KrigingPage::buildInterface()
{
  setTitle(tr("Kriging parameters"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  // Kriging parameters group
  QGroupBox * parametersBox = new QGroupBox(tr("Parameters"));
  QGridLayout * parametersLayout = new QGridLayout(parametersBox);

  // correlation model
  parametersLayout->addWidget(new QLabel(tr("Covariance model")), 0, 0, 1, 2);
  covarianceModelComboBox_ = new QComboBox;
  covarianceModelComboBox_->addItems(QStringList() << tr("Squared exponential")
                                                   << tr("Absolute exponential")
                                                   << tr("Generalized exponential")
                                                   << QString::fromUtf8("Matérn"));
  parametersLayout->addWidget(covarianceModelComboBox_, 0, 1, 1, 2);

  connect(covarianceModelComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCovarianceModel(int)));

  // Matern parameter nu
  //  -label
  maternParameterNuLabel_ = new QLabel(tr("nu"));
  parametersLayout->addWidget(maternParameterNuLabel_, 1, 1);

  //  -spinbox
  maternParameterNuSpinBox_ = new DoubleSpinBox;
  maternParameterNuSpinBox_->setMinimum(1e-12);
  parametersLayout->addWidget(maternParameterNuSpinBox_, 1, 2);
  maternParameterNuSpinBox_->setValue(ResourceMap::GetAsNumericalScalar("MaternModel-DefaultNu"));

  // GeneralizedExponential parameter P
  //  -label
  generalizedModelParameterPLabel_ = new QLabel(tr("p"));
  parametersLayout->addWidget(generalizedModelParameterPLabel_, 2, 1);

  //  -spinbox
  generalizedModelParameterPSpinBox_ = new DoubleSpinBox;
  generalizedModelParameterPSpinBox_->setMinimum(1e-12); // TODO setMaximum with the next OT version
  parametersLayout->addWidget(generalizedModelParameterPSpinBox_, 2, 2);
  generalizedModelParameterPSpinBox_->setValue(1.0);

  // basis
  parametersLayout->addWidget(new QLabel(tr("Trend basis type")), 3, 0);
  basisTypeComboBox_ = new QComboBox;
  basisTypeComboBox_->addItems(QStringList() << tr("Constant")
                                             << tr("Linear")
                                             << tr("Quadratic"));
  parametersLayout->addWidget(basisTypeComboBox_, 3, 1, 1, 2);
  basisTypeComboBox_->setCurrentIndex(0);

  parametersLayout->setColumnStretch(0, 1);
  parametersLayout->setColumnStretch(2, 2);
  pageLayout->addWidget(parametersBox);

  // Metamodel validation
  QGroupBox * validationGroupBox = new QGroupBox(tr("Validation"));
  QGridLayout * validationLayout = new QGridLayout(validationGroupBox);

  // -- LOO
  leaveOneOutCheckBox_ = new QCheckBox;
  validationLayout->addWidget(leaveOneOutCheckBox_, 0, 0);
  validationLayout->addWidget(new QLabel(tr("Compute Q2 by Leave-one-out")), 0, 1);
  validationLayout->setColumnStretch(2, 1);
  pageLayout->addWidget(validationGroupBox);


  // Advanced parameters

  // kriging advanced parameters
  CollapsibleGroupBox * advancedParamGroupBox = new CollapsibleGroupBox;
  advancedParamGroupBox->setTitle(tr("Advanced parameters"));
  QGridLayout * advancedParamGroupBoxLayout = new QGridLayout(advancedParamGroupBox);

  // request parameters optimization
  optimizationCheckBox_ = new QCheckBox(tr("Optimize covariance model parameters"));
  optimizationCheckBox_->setToolTip(tr("Optimize scale and amplitude parameters"));
  advancedParamGroupBoxLayout->addWidget(optimizationCheckBox_,  0, 0, 1, 3);

  // scale
  advancedParamGroupBoxLayout->addWidget(new QLabel(tr("Scale")), 1, 0);

  scaleLineEdit_ = new QLineEdit;
  scaleLineEdit_->setReadOnly(true);
  advancedParamGroupBoxLayout->addWidget(scaleLineEdit_, 1, 1);

  QToolButton * editButton = new QToolButton;
  editButton->setText("...");
  advancedParamGroupBoxLayout->addWidget(editButton, 1, 2);
  connect(editButton, SIGNAL(pressed()), this, SLOT(openScaleDefinitionWizard()));

  // amplitude
  // same amplitude for all the outputs
  advancedParamGroupBoxLayout->addWidget(new QLabel(tr("Amplitude")), 2, 0);

  amplitudeSpinBox_ = new DoubleSpinBox;
  amplitudeSpinBox_->setMinimum(1e-12);
  advancedParamGroupBoxLayout->addWidget(amplitudeSpinBox_, 2, 1, 1, 2);

  advancedParamGroupBox->setExpanded(false);
  pageLayout->addWidget(advancedParamGroupBox);

  initialize(KrigingAnalysis());
}


void KrigingPage::initialize(const Analysis& analysis)
{
  const KrigingAnalysis * analysis_ptr = dynamic_cast<const KrigingAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
    return;

  inputsNames_ = analysis_ptr->getEffectiveInputSample().getDescription();

  // covariance model
  const CovarianceModel covModel(analysis_ptr->getCovarianceModel());
  const String covarianceModelName = covModel.getImplementation()->getClassName();

  if (covarianceModelName == "SquaredExponential")
  {
    covarianceModelComboBox_->setCurrentIndex(0);
  }
  else if (covarianceModelName == "AbsoluteExponential")
  {
    covarianceModelComboBox_->setCurrentIndex(1);
  }
  else if (covarianceModelName == "MaternModel")
  {
    covarianceModelComboBox_->setCurrentIndex(3);
    const double nu = dynamic_cast<MaternModel*>(covModel.getImplementation().get())->getNu();
    maternParameterNuSpinBox_->setValue(nu);
  }
  else if (covarianceModelName == "GeneralizedExponential")
  {
    covarianceModelComboBox_->setCurrentIndex(2);
    const double p = dynamic_cast<GeneralizedExponential*>(covModel.getImplementation().get())->getP();
    generalizedModelParameterPSpinBox_->setValue(p);
  }
  maternParameterNuLabel_->setVisible(covarianceModelName == "MaternModel");
  maternParameterNuSpinBox_->setVisible(covarianceModelName == "MaternModel");
  generalizedModelParameterPSpinBox_->setVisible(covarianceModelName == "GeneralizedExponential");
  generalizedModelParameterPLabel_->setVisible(covarianceModelName == "GeneralizedExponential");

  // basis
  const UnsignedInteger basisDim = analysis_ptr->getBasis().getDimension();
  const UnsignedInteger basisSize = analysis_ptr->getBasis().getSize();
  if (basisSize == (basisDim+1))
    basisTypeComboBox_->setCurrentIndex(1);
  else if (basisSize == ((basisDim+1)*(basisDim+2)/2))
    basisTypeComboBox_->setCurrentIndex(2);
  else
    basisTypeComboBox_->setCurrentIndex(0);

  // validation
  leaveOneOutCheckBox_->setChecked(analysis_ptr->isLeaveOneOutValidation());

  // optimization
  optimizationCheckBox_->setChecked(analysis_ptr->getOptimizeParameters());

  // scale
  scales_ = covModel.getScale();
  updateScaleLineEdit();

  // amplitude
  amplitudeSpinBox_->setValue(covModel.getAmplitude()[0]);
}


void KrigingPage::updateScaleLineEdit()
{
  QString scaleText;
  for (UnsignedInteger i=0; i<scales_.getSize(); ++i)
  {
    scaleText += QString::number(scales_[i]);
    if (i < scales_.getSize()-1)
      scaleText += "; ";
  }
  scaleLineEdit_->setText(scaleText);
}


void KrigingPage::updateCovarianceModel(int modelType)
{
  maternParameterNuLabel_->setVisible(modelType == 3);
  maternParameterNuSpinBox_->setVisible(modelType == 3);
  generalizedModelParameterPLabel_->setVisible(modelType == 2);
  generalizedModelParameterPSpinBox_->setVisible(modelType == 2);
}


void KrigingPage::updateCovarianceModel(DesignOfExperiment doe)
{
  const KrigingAnalysis analysis("unnamed", doe);
  initialize(analysis);
}


void KrigingPage::openScaleDefinitionWizard()
{
  const QStringList labels = QStringList() << tr("Input") << tr("Scale");
  ParametersDefinitionWizard * paramDefinitionWizard = new ParametersDefinitionWizard(inputsNames_, scales_, labels, this);
  paramDefinitionWizard->setWindowTitle(tr("Scale"));

  if (paramDefinitionWizard->exec())
  {
    scales_ = paramDefinitionWizard->getValues();
    updateScaleLineEdit();
  }
}


Analysis KrigingPage::getAnalysis(const String& name, const DesignOfExperiment& doe) const
{
  KrigingAnalysis analysis(name, doe);

  // covariance model
  const NumericalPoint amplitude(1, amplitudeSpinBox_->value());
  switch (covarianceModelComboBox_->currentIndex())
  {
    case 0: // SquaredExponential
      analysis.setCovarianceModel(SquaredExponential(scales_, amplitude));
      break;
    case 1: // AbsoluteExponential
      analysis.setCovarianceModel(AbsoluteExponential(scales_, amplitude));
      break;
    case 2: // GeneralizedExponential
      analysis.setCovarianceModel(GeneralizedExponential(scales_, amplitude, generalizedModelParameterPSpinBox_->value()));
      break;
    case 3: // MaternModel
      analysis.setCovarianceModel(MaternModel(scales_, amplitude, maternParameterNuSpinBox_->value()));
      break;
    default:
      throw InvalidArgumentException(HERE) << "The covariance model type is not recognized";
  }

  // basis
  const UnsignedInteger inputDimension = analysis.getEffectiveInputSample().getDimension();

  switch (basisTypeComboBox_->currentIndex())
  {
    case 0: // Constant
      analysis.setBasis(ConstantBasisFactory(inputDimension).build());
      break;
    case 1: // Linear
      analysis.setBasis(LinearBasisFactory(inputDimension).build());
      break;
    case 2: // Quadratic
      analysis.setBasis(QuadraticBasisFactory(inputDimension).build());
      break;
    default:
      throw InvalidArgumentException(HERE) << "The basis type is not recognized";
  }
  // validation
  analysis.setLeaveOneOutValidation(leaveOneOutCheckBox_->isChecked());
  // optimization
  analysis.setOptimizeParameters(optimizationCheckBox_->isChecked());

  return analysis;
}
}