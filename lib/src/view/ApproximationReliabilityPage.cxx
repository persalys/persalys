//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define approximation reliability analysis
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#include "persalys/ApproximationReliabilityPage.hxx"

#include "persalys/FORMAnalysis.hxx"
#include "persalys/SORMAnalysis.hxx"
#include "persalys/FORMImportanceSamplingAnalysis.hxx"
#include "persalys/CollapsibleGroupBox.hxx"
#include "persalys/ParametersDefinitionWizard.hxx"
#include "persalys/QtTools.hxx"

#include <openturns/Cobyla.hxx>
#include <openturns/AbdoRackwitz.hxx>
#include <openturns/SQP.hxx>

#include <QGroupBox>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QLabel>
#include <QToolButton>
#include <QDebug>

using namespace OT;

namespace PERSALYS
{

ApproximationReliabilityPage::ApproximationReliabilityPage(QWidget* parent)
  : QWizardPage(parent)
  , inputNames_()
  , startingPoint_()
  , pointLineEdit_(0)
  , algoChoice_(0)
{
  buildInterface();
}


void ApproximationReliabilityPage::buildInterface()
{
  setTitle(tr("Optimization parameters"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  // optimization algorithm ---------- ----------
  algoChoice_ = new QButtonGroup(this);

  QGroupBox * optimAlgoGroup = new QGroupBox(tr("Algorithm"));
  QVBoxLayout * optimAlgoGroupLayout = new QVBoxLayout(optimAlgoGroup);

  // Abdo-Rackwitz
  QRadioButton * abdoRackwitzRadioButton = new QRadioButton(tr("Abdo-Rackwitz"));
  abdoRackwitzRadioButton->setChecked(true);
  optimAlgoGroupLayout->addWidget(abdoRackwitzRadioButton);
  algoChoice_->addButton(abdoRackwitzRadioButton, ApproximationReliabilityPage::AbdoRackwitzAlgo);

  // Cobyla
  QRadioButton * cobylaRadioButton = new QRadioButton(tr("Cobyla"));
  optimAlgoGroupLayout->addWidget(cobylaRadioButton);
  algoChoice_->addButton(cobylaRadioButton, ApproximationReliabilityPage::CobylaAlgo);

  // SQP
  QRadioButton * sqpRadioButton = new QRadioButton(tr("SQP"));
  optimAlgoGroupLayout->addWidget(sqpRadioButton);
  algoChoice_->addButton(sqpRadioButton, ApproximationReliabilityPage::SQPAlgo);

  pageLayout->addWidget(optimAlgoGroup);

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

  pageLayout->addWidget(pointGroup);

  // Advanced parameters ---------------------
  CollapsibleGroupBox * advancedParamGroupBox = new CollapsibleGroupBox;
  advancedParamGroupBox->setTitle(tr("Advanced parameters"));
  stoppingCriteriaLayout_ = new OptimizationStoppingCriteria(advancedParamGroupBox);
  pageLayout->addWidget(advancedParamGroupBox);

  // initialize widgets
  initialize(FORMAnalysis());
}


void ApproximationReliabilityPage::initialize(const Analysis& analysis)
{
  const ReliabilityAnalysis * analysis_ptr = dynamic_cast<const ReliabilityAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
    return;

  inputNames_ = analysis_ptr->getPhysicalModel().getStochasticInputNames();
  startingPoint_ = analysis_ptr->getPhysicalModel().getDistribution().getMean();
  updatePointLineEdit();

  const ApproximationAnalysis * approxAnalysis_ptr = dynamic_cast<const ApproximationAnalysis*>(analysis.getImplementation().get());

  if (!approxAnalysis_ptr)
    return;

  OptimizationAlgorithm solver = approxAnalysis_ptr->getOptimizationAlgorithm();

  // if solver.getStartingPoint is valid, we use it
  if (solver.getStartingPoint().getSize() == inputNames_.getSize())
  {
    startingPoint_ = solver.getStartingPoint();
    updatePointLineEdit();
  }

  // initialize widgets
  const String solverName(solver.getImplementation()->getClassName());
  if (solverName == "AbdoRackwitz")
    algoChoice_->button(ApproximationReliabilityPage::AbdoRackwitzAlgo)->click();
  else if (solverName ==  "Cobyla")
    algoChoice_->button(ApproximationReliabilityPage::CobylaAlgo)->click();
  else if (solverName ==  "SQP")
    algoChoice_->button(ApproximationReliabilityPage::SQPAlgo)->click();

  stoppingCriteriaLayout_->initialize(solver);
}


void ApproximationReliabilityPage::updatePointLineEdit()
{
  if (pointLineEdit_)
    pointLineEdit_->setText(QtOT::PointToString(startingPoint_));
}


void ApproximationReliabilityPage::openPointDefinitionWizard()
{
  if (inputNames_.getSize() != startingPoint_.getSize())
  {
    qDebug() << "Error: In ApproximationReliabilityPage::openPointDefinitionWizard: arguments have different dimensions";
    return;
  }

  const QStringList labels = QStringList() << tr("Input") << tr("Value");
  ParametersDefinitionWizard * paramDefinitionWizard = new ParametersDefinitionWizard(inputNames_, startingPoint_, labels, this);
  paramDefinitionWizard->setWindowTitle(tr("Physical starting point"));

  if (paramDefinitionWizard->exec())
  {
    startingPoint_ = paramDefinitionWizard->getValues();
    updatePointLineEdit();
  }

}


OptimizationAlgorithm ApproximationReliabilityPage::getOptimizationAlgorithm() const
{
  OptimizationAlgorithm optimAlgo;

  if (algoChoice_->checkedId() == ApproximationReliabilityPage::CobylaAlgo)
    optimAlgo = Cobyla();
  else if (algoChoice_->checkedId() == ApproximationReliabilityPage::AbdoRackwitzAlgo)
    optimAlgo = AbdoRackwitz();
  else if (algoChoice_->checkedId() == ApproximationReliabilityPage::SQPAlgo)
    optimAlgo = SQP();

  optimAlgo.setStartingPoint(startingPoint_);
  stoppingCriteriaLayout_->updateAlgorithm(optimAlgo);

  return optimAlgo;
}


void ApproximationReliabilityPage::updateAnalysis(const Analysis& analysis)
{
  ApproximationAnalysis * analysis_ptr = dynamic_cast<ApproximationAnalysis*>(analysis.getImplementation().get());
  if (!analysis_ptr)
    return;
  analysis_ptr->setOptimizationAlgorithm(getOptimizationAlgorithm());
  analysis_ptr->setPhysicalStartingPoint(startingPoint_);
}


bool ApproximationReliabilityPage::validatePage()
{
  return true;
}
}
