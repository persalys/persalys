//                                               -*- C++ -*-
/**
 *  @brief QWidget to configure optimization algorithm
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
#include "otgui/OptimizationWidget.hxx"

#include "otgui/FORMImportanceSamplingAnalysis.hxx"
#include "otgui/CollapsibleGroupBox.hxx"
#include "otgui/ParametersDefinitionWizard.hxx"

#include "openturns/Cobyla.hxx"
#include "openturns/AbdoRackwitz.hxx"
#include "openturns/SQP.hxx"

#include <limits>

#include <QGroupBox>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QLabel>
#include <QToolButton>
#include <QDebug>

using namespace OT;

namespace OTGUI {

OptimizationWidget::OptimizationWidget(QWidget* parent)
  : QWidget(parent)
  , optimAlgo_(Cobyla())
  , inputNames_()
  , pointLineEdit_(0)
{
  buildInterface();
}


void OptimizationWidget::buildInterface()
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);

  // optimization algorithm ---------- ----------
  algoChoice_ = new QButtonGroup;

  QGroupBox * optimAlgoGroup = new QGroupBox(tr("Algorithm"));
  QVBoxLayout * optimAlgoGroupLayout = new QVBoxLayout(optimAlgoGroup);

  // Abdo-Rackwitz
  QRadioButton * abdoRackwitzRadioButton = new QRadioButton(tr("Abdo-Rackwitz"));
  abdoRackwitzRadioButton->setChecked(true);
  optimAlgoGroupLayout->addWidget(abdoRackwitzRadioButton);
  algoChoice_->addButton(abdoRackwitzRadioButton, OptimizationWidget::AbdoRackwitzAlgo);

  // Cobyla
  QRadioButton * cobylaRadioButton = new QRadioButton(tr("Cobyla"));
  optimAlgoGroupLayout->addWidget(cobylaRadioButton);
  algoChoice_->addButton(cobylaRadioButton, OptimizationWidget::CobylaAlgo);

  // SQP
  QRadioButton * sqpRadioButton = new QRadioButton(tr("SQP"));
  optimAlgoGroupLayout->addWidget(sqpRadioButton);
  algoChoice_->addButton(sqpRadioButton, OptimizationWidget::SQPAlgo);

  connect(algoChoice_, SIGNAL(buttonClicked(int)), this, SLOT(updateAlgorithmType(int)));
  mainLayout->addWidget(optimAlgoGroup);

  // starting point
  QGroupBox * pointGroup = new QGroupBox(tr("Starting point"));
  QGridLayout * pointGroupLayout = new QGridLayout(pointGroup);

  QLabel * pointLabel = new QLabel(tr("Physical starting point"));
  pointGroupLayout->addWidget(pointLabel, 0, 0);

  pointLineEdit_ = new QLineEdit;
  pointLineEdit_->setReadOnly(true);
  pointGroupLayout->addWidget(pointLineEdit_, 0, 1);

  QToolButton * editButton = new QToolButton;
  editButton->setText("...");
  pointGroupLayout->addWidget(editButton, 0, 2);
  connect(editButton, SIGNAL(pressed()), this, SLOT(openPointDefinitionWizard()));

  mainLayout->addWidget(pointGroup);

  // Advanced parameters ---------- ----------
  CollapsibleGroupBox * advancedParamGroupBox = new CollapsibleGroupBox;
  advancedParamGroupBox->setTitle(tr("Advanced parameters"));
  QGridLayout * advancedGroupLayout = new QGridLayout(advancedParamGroupBox);

  // max number iterations
  QLabel * label = new QLabel(tr("Number of iterations"));
  advancedGroupLayout->addWidget(label, 0, 0);
  iterationsSpinBox_ = new UIntSpinBox;
  label->setBuddy(iterationsSpinBox_);
  iterationsSpinBox_->setRange(1, 2e9);
  iterationsSpinBox_->setSingleStep(5);
  connect(iterationsSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(updateNumberIterations(double)));
  advancedGroupLayout->addWidget(iterationsSpinBox_, 0, 1);

  // Absolute error
  label = new QLabel(tr("Absolute error"));
  advancedGroupLayout->addWidget(label, 1, 0);
  absoluteErrSpinBox_ = new LogDoubleSpinBox;
  label->setBuddy(absoluteErrSpinBox_);
  absoluteErrSpinBox_->setRange(std::numeric_limits<double>::min(), 1.0);
  connect(absoluteErrSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(updateAbsoluteError(double)));
  advancedGroupLayout->addWidget(absoluteErrSpinBox_, 1, 1);

  // Relative error
  label = new QLabel(tr("Relative error"));
  advancedGroupLayout->addWidget(label, 2, 0);
  relativeErrSpinBox_ = new LogDoubleSpinBox;
  label->setBuddy(relativeErrSpinBox_);
  relativeErrSpinBox_->setRange(std::numeric_limits<double>::min(), 1.0);
  connect(relativeErrSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(updateRelativeError(double)));
  advancedGroupLayout->addWidget(relativeErrSpinBox_, 2, 1);

  // Residual error
  label = new QLabel(tr("Residual error"));
  advancedGroupLayout->addWidget(label, 1, 2);
  residualErrSpinBox_ = new LogDoubleSpinBox;
  label->setBuddy(residualErrSpinBox_);
  residualErrSpinBox_->setRange(std::numeric_limits<double>::min(), 1.0);
  connect(residualErrSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(updateResidualError(double)));
  advancedGroupLayout->addWidget(residualErrSpinBox_, 1, 3);

  // Constraint error
  label = new QLabel(tr("Constraint error"));
  advancedGroupLayout->addWidget(label, 2, 2);
  constraintErrSpinBox_ = new LogDoubleSpinBox;
  label->setBuddy(constraintErrSpinBox_);
  constraintErrSpinBox_->setRange(std::numeric_limits<double>::min(), 1.0);
  connect(constraintErrSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(updateConstraintError(double)));
  advancedGroupLayout->addWidget(constraintErrSpinBox_, 2, 3);

  mainLayout->addWidget(advancedParamGroupBox);

  updateWidgets();
}


void OptimizationWidget::updateWidgets()
{
  if (optimAlgo_.getImplementation()->getClassName() == "AbdoRackwitz")
    algoChoice_->button(OptimizationWidget::AbdoRackwitzAlgo)->click();
  else if (optimAlgo_.getImplementation()->getClassName() ==  "Cobyla")
    algoChoice_->button(OptimizationWidget::CobylaAlgo)->click();
  else
    algoChoice_->button(OptimizationWidget::SQPAlgo)->click();

  updatePointLineEdit();

  iterationsSpinBox_->setValue(optimAlgo_.getMaximumIterationNumber());
  absoluteErrSpinBox_->setValue(optimAlgo_.getMaximumAbsoluteError());
  relativeErrSpinBox_->setValue(optimAlgo_.getMaximumRelativeError());
  residualErrSpinBox_->setValue(optimAlgo_.getMaximumResidualError());
  constraintErrSpinBox_->setValue(optimAlgo_.getMaximumConstraintError());
}


void OptimizationWidget::initialize(const Analysis& analysis)
{
  const ReliabilityAnalysis * analysis_ptr = dynamic_cast<const ReliabilityAnalysis*>(analysis.getImplementation().get());
  inputNames_ = analysis_ptr->getPhysicalModel().getStochasticInputNames();

  const ApproximationAnalysis * approxAnalysis_ptr = dynamic_cast<const ApproximationAnalysis*>(analysis_ptr);
  if (approxAnalysis_ptr)
    optimAlgo_ = approxAnalysis_ptr->getOptimizationAlgorithm();

  // if analysis is not an ApproximationAnalysis or if number of inputs changed
  if (!approxAnalysis_ptr ||( optimAlgo_.getStartingPoint().getSize() != inputNames_.getSize()))
    optimAlgo_.setStartingPoint(analysis_ptr->getPhysicalModel().getComposedDistribution().getMean());

  updateWidgets();
}


OptimizationSolver OptimizationWidget::getOptimizationAlgorithm() const
{
  return optimAlgo_;
}


void OptimizationWidget::updatePointLineEdit()
{
  QString pointText;
  for (UnsignedInteger i=0; i<optimAlgo_.getStartingPoint().getSize(); ++i)
  {
    pointText += QString::number(optimAlgo_.getStartingPoint()[i]);
    if (i < optimAlgo_.getStartingPoint().getSize()-1)
      pointText += "; ";
  }
  pointLineEdit_->setText(pointText);
}


void OptimizationWidget::openPointDefinitionWizard()
{
  if (inputNames_.getSize() != optimAlgo_.getStartingPoint().getSize())
  {
    qDebug() << "Error: In OptimizationWidget::openPointDefinitionWizard: arguments have different dimensions";
    return;
  }

  const QStringList labels = QStringList() << tr("Input") << tr("Value");
  ParametersDefinitionWizard * paramDefinitionWizard = new ParametersDefinitionWizard(inputNames_, optimAlgo_.getStartingPoint(), labels, this);
  paramDefinitionWizard->setWindowTitle(tr("Physical starting point"));

  if (paramDefinitionWizard->exec())
  {
    optimAlgo_.setStartingPoint(paramDefinitionWizard->getValues());
    updatePointLineEdit();
  }
}


void OptimizationWidget::updateAlgorithmType(int algoType)
{
  OptimizationSolver optimAlgo(optimAlgo_);

  if (algoType == OptimizationWidget::CobylaAlgo)
    optimAlgo_ = Cobyla();
  else if (algoType == OptimizationWidget::AbdoRackwitzAlgo)
    optimAlgo_ = AbdoRackwitz();
  else if (algoType == OptimizationWidget::SQPAlgo)
    optimAlgo_ = SQP();

  optimAlgo_.setStartingPoint(optimAlgo.getStartingPoint());
  optimAlgo_.setMaximumIterationNumber(optimAlgo.getMaximumIterationNumber());
  optimAlgo_.setMaximumAbsoluteError(optimAlgo.getMaximumAbsoluteError());
  optimAlgo_.setMaximumRelativeError(optimAlgo.getMaximumRelativeError());
  optimAlgo_.setMaximumResidualError(optimAlgo.getMaximumResidualError());
  optimAlgo_.setMaximumConstraintError(optimAlgo.getMaximumConstraintError());
}


void OptimizationWidget::updateNumberIterations(double iter)
{
  optimAlgo_.setMaximumIterationNumber((UnsignedInteger)iter);
}


void OptimizationWidget::updateAbsoluteError(double error)
{
  optimAlgo_.setMaximumAbsoluteError(error);
}


void OptimizationWidget::updateRelativeError(double error)
{
  optimAlgo_.setMaximumRelativeError(error);
}


void OptimizationWidget::updateResidualError(double error)
{
  optimAlgo_.setMaximumResidualError(error);
}


void OptimizationWidget::updateConstraintError(double error)
{
  optimAlgo_.setMaximumConstraintError(error);
}
}