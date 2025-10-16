//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define a probabilistic design of experiments
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "persalys/ProbabilisticDesignPage.hxx"

#include "persalys/ProbabilisticDesignOfExperiment.hxx"
#include "persalys/CollapsibleGroupBox.hxx"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QRadioButton>
#include <QGroupBox>
#include <QListView>
#include <QStandardItemModel>

using namespace OT;

namespace PERSALYS
{

ProbabilisticDesignPage::ProbabilisticDesignPage(QWidget* parent)
  : QWizardPage(parent)
{
  buildInterface();
}

void ProbabilisticDesignPage::buildInterface()
{
  setTitle(tr("Probabilistic design of experiments"));

  auto * mainLayout = new QVBoxLayout(this);

  auto * designGroupBox = new QGroupBox(tr("Designs"));
  auto * designGroupBoxLayout = new QGridLayout(designGroupBox);

  designButtonsGroup_ = new QButtonGroup(this);

  // Monte Carlo button
  auto * mcButton = new QRadioButton(tr("Monte Carlo"));
  mcButton->setChecked(true);
  designButtonsGroup_->addButton(mcButton, ProbabilisticDesignPage::MonteCarlo);
  designGroupBoxLayout->addWidget(mcButton, 0, 0);

  // LHS button
  auto * lhsButton = new QRadioButton(tr("LHS"));
  lhsButton->setToolTip(tr("Latin Hypercube Sampling"));
  designButtonsGroup_->addButton(lhsButton, ProbabilisticDesignPage::LHS);
  designGroupBoxLayout->addWidget(lhsButton, 1, 0);

  // LHS Warning
  lhsWarningLabel_ = new QLabel(tr("Warning: LHS is designed for independent variables. With dependent inputs, the sample may not preserve LHS properties and results can be less reliable."));
  lhsWarningLabel_->setWordWrap(true);
  lhsWarningLabel_->setStyleSheet("color: orange;");
  lhsWarningLabel_->setVisible(false);
  designGroupBoxLayout->addWidget(lhsWarningLabel_, 2, 0);

  // LHS advanced params
  auto * lhsAdvancedParamsGroupBox = new CollapsibleGroupBox;
  lhsAdvancedParamsGroupBox->setTitle(tr("Optimisation"));
  auto * lhsAdvancedParamsGroupBoxLayout = new QGridLayout(lhsAdvancedParamsGroupBox);

  lhsAdvancedParamsGroupBoxLayout->addWidget(new QLabel(tr("Optimisation algorithm")), 0, 0);
  optimComboBox_ = new QComboBox;
  optimComboBox_->addItem(tr("None"), ProbabilisticDesignPage::LHS);
  optimComboBox_->addItem(tr("Simulated Annealing LHS"), ProbabilisticDesignPage::SALHS);
  optimComboBox_->addItem(tr("Monte Carlo LHS"), ProbabilisticDesignPage::MCLHS);
  lhsAdvancedParamsGroupBoxLayout->addWidget(optimComboBox_, 0, 1);

  lhsAdvancedParamsGroupBoxLayout->addWidget(new QLabel(tr("Space filling")), 1, 0);
  spaceFillingComboBox_ = new QComboBox;
  spaceFillingComboBox_->addItem(tr("PhiP"), 0);
  spaceFillingComboBox_->addItem(tr("minDist"), 1);
  spaceFillingComboBox_->addItem(tr("C2"), 2);
  spaceFillingComboBox_->setCurrentIndex(0);
  lhsAdvancedParamsGroupBoxLayout->addWidget(spaceFillingComboBox_, 1, 1);

  lhsAdvancedParamsGroupBoxLayout->addWidget(new QLabel(tr("Number of simulations")), 2, 0);
  mcLhsSizeSB_ = new LogSpinBox;
  mcLhsSizeSB_->setValue(1000);
  lhsAdvancedParamsGroupBoxLayout->addWidget(mcLhsSizeSB_, 2, 1);

  designGroupBoxLayout->addWidget(lhsAdvancedParamsGroupBox, 1, 1);

  // Centralise the UI update logic for LHS advanced params
  auto updateLHSUI = [this, lhsButton]() {
    const bool lhsSelected = lhsButton->isChecked();

    const int variant = optimComboBox_->currentData().toInt(); // enum DesignType value
    const bool isSA    = (variant == ProbabilisticDesignPage::SALHS);
    const bool isMCLHS = (variant == ProbabilisticDesignPage::MCLHS);

    optimComboBox_->setEnabled(lhsSelected);
    spaceFillingComboBox_->setEnabled(lhsSelected && (isSA || isMCLHS));
    mcLhsSizeSB_->setEnabled(lhsSelected && isMCLHS);

    if (auto * view = qobject_cast<QListView *>(spaceFillingComboBox_->view()))
      view->setRowHidden(1, isSA);
    if (auto * model = qobject_cast<QStandardItemModel*>(spaceFillingComboBox_->model())) {
      if (auto * item = model->item(1)) {
        if (isSA)
          item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        else
          item->setFlags(item->flags() | Qt::ItemIsEnabled);
      }
    }
    if (isSA && spaceFillingComboBox_->currentIndex() == 1)
      spaceFillingComboBox_->setCurrentIndex(0);
  };

  // Update on LHS toggle and on optimisation choice
  connect(lhsButton, &QRadioButton::toggled, this, [updateLHSUI](bool) { updateLHSUI(); });
  connect(optimComboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [updateLHSUI](int) { updateLHSUI(); });

  // Quasi Monte Carlo button
  auto * qmcButton = new QRadioButton(tr("Quasi-Monte Carlo"));
  designButtonsGroup_->addButton(qmcButton, ProbabilisticDesignPage::QuasiMonteCarlo);
  designGroupBoxLayout->addWidget(qmcButton, 3, 0);

  mainLayout->addWidget(designGroupBox);

  // sample size
  auto * sampleSizeGroupBox = new QGroupBox(tr("Generation parameter"));
  auto * sampleSizeLayout = new QGridLayout(sampleSizeGroupBox);

  auto * sampleSizeLabel = new QLabel(tr("Sample size"));
  sampleSizeLayout->addWidget(sampleSizeLabel, 0, 0);
  sampleTimeLabel_ = new QLabel(tr("Estimated duration (s):"));
  sampleSizeLayout->addWidget(sampleTimeLabel_, 1, 0);

  sampleSizeSpinBox_ = new LogSpinBox;
  sampleSizeSpinBox_->setMinimum(1);
  sampleSizeSpinBox_->setMaximum(std::numeric_limits<int>::max());
  sampleSizeSpinBox_->setValue(static_cast<double>(OT::ResourceMap::GetAsUnsignedInteger("WeightedExperiment-DefaultSize")));
  sampleSizeLabel->setBuddy(sampleSizeSpinBox_);
  sampleSizeLayout->addWidget(sampleSizeSpinBox_, 0, 1);

  sampleTimeValueLabel_ = new QLabel("0"); // text depends on modelEvalTime_, modified by signal
  sampleSizeLayout->addWidget(sampleTimeValueLabel_, 1, 1);
  mainLayout->addWidget(sampleSizeGroupBox);

  sampleTimeValueLabel_->setVisible(false);
  sampleTimeLabel_->setVisible(false);

  connect(sampleSizeSpinBox_, QOverload<double>::of(&LogSpinBox::valueChanged),
  this, [this](double) { updateEstimatedDuration(); });

  // advanced parameters
  auto * advancedParamsGroupBox = new CollapsibleGroupBox;
  advancedParamsGroupBox->setTitle(tr("Advanced parameters"));
  auto * advancedParamsGroupBoxLayout = new QVBoxLayout(advancedParamsGroupBox);

  auto * seedLabel = new QLabel(tr("Seed"));
  advancedParamsGroupBoxLayout->addWidget(seedLabel);

  seedSpinBox_ = new QSpinBox;
  seedSpinBox_->setMaximum(std::numeric_limits<int>::max());
  seedLabel->setBuddy(seedSpinBox_);
  advancedParamsGroupBoxLayout->addWidget(seedSpinBox_);

  advancedParamsGroupBox->setExpanded(false);

  mainLayout->addWidget(advancedParamsGroupBox);

  // Ensure initial state is consistent with default selection (MC checked, LHS unchecked)
  updateLHSUI();
}

void ProbabilisticDesignPage::initialize(const Analysis& analysis)
{
  const auto * doe = dynamic_cast<const DesignOfExperimentEvaluation*>(analysis.getImplementation().get());
  if (!doe)
    return;
  
  modelEvalTime_ = doe->getPhysicalModel().getEvalTime();

  blockSize_ = doe->getBlockSize();

  // check copula independance
  const bool independentCopula = doe->getPhysicalModel().getCopula().hasIndependentCopula();
  lhsWarningLabel_->setVisible(!independentCopula);

  // initialize widgets if the analysis is already a ProbabilisticDesignOfExperiment
  const auto * probaDOE = dynamic_cast<const ProbabilisticDesignOfExperiment *>(doe);
  if (!probaDOE)
    return;
  
  const String designName{probaDOE->getDesignName()};
  if (designName == "LHS" && independentCopula)
  {
    designButtonsGroup_->button(ProbabilisticDesignPage::LHS)->click();
    optimComboBox_->setCurrentIndex(0);
  }
  else if (designName == "SALHS" && independentCopula)
  {
    designButtonsGroup_->button(ProbabilisticDesignPage::LHS)->click();
    optimComboBox_->setCurrentIndex(1);
  }
  else if (designName == "MCLHS" && independentCopula)
  {
    designButtonsGroup_->button(ProbabilisticDesignPage::LHS)->click();
    optimComboBox_->setCurrentIndex(2);
    mcLhsSizeSB_->setValue(static_cast<double>(probaDOE->getMCLHSSize()));
  }
  else if (designName == "QUASI_MONTE_CARLO")
  {
    designButtonsGroup_->button(ProbabilisticDesignPage::QuasiMonteCarlo)->click();
  }
  else
  {
    designButtonsGroup_->button(ProbabilisticDesignPage::MonteCarlo)->click();
  }

  const String spaceFilling{probaDOE->getSpaceFilling()};
  if (spaceFilling == "PhiP" && independentCopula)
    spaceFillingComboBox_->setCurrentIndex(0);
  else if (spaceFilling == "minDist" && independentCopula)
    spaceFillingComboBox_->setCurrentIndex(1);
  else if (spaceFilling == "C2" && independentCopula)
    spaceFillingComboBox_->setCurrentIndex(2);
  
  sampleSizeSpinBox_->setValue(static_cast<double>(probaDOE->getSize()));
  seedSpinBox_->setValue(static_cast<int>(probaDOE->getSeed()));

  const int variant = optimComboBox_->currentData().toInt();
  mcLhsSizeSB_->setEnabled(variant == ProbabilisticDesignPage::MCLHS);
  spaceFillingComboBox_->setEnabled(variant == ProbabilisticDesignPage::SALHS ||
                                    variant == ProbabilisticDesignPage::MCLHS);
  
  updateEstimatedDuration();
}

Analysis ProbabilisticDesignPage::getAnalysis(const String &name, const PhysicalModel &model) const
{
  String designType;
  const int checked = designButtonsGroup_->checkedId();
  if (checked == ProbabilisticDesignPage::LHS)
  {
    const int opt = optimComboBox_->currentIndex(); // 0=None, 1=SALHS, 2=MCLHS
    if (opt == 1)      designType = "SALHS";
    else if (opt == 2) designType = "MCLHS";
    else               designType = "LHS";
  }
  else if (checked == ProbabilisticDesignPage::QuasiMonteCarlo)
  {
    designType = "QUASI_MONTE_CARLO";
  }
  else
  {
    designType = "MONTE_CARLO";
  }

  String spaceFilling;
  switch (spaceFillingComboBox_->currentIndex())
  {
    case 1: spaceFilling = "minDist"; break;
    case 2: spaceFilling = "C2"; break;
    default: spaceFilling = "PhiP"; break;
  }

  ProbabilisticDesignOfExperiment doe{name,
    model,
    static_cast<UnsignedInteger>(sampleSizeSpinBox_->value()),
    designType,
    spaceFilling,
    mcLhsSizeSB_->isEnabled() ? static_cast<UnsignedInteger>(mcLhsSizeSB_->value()) : 0u
  };

  doe.setSeed(seedSpinBox_->value());
  doe.setBlockSize(blockSize_);
  
  return doe;
}

void ProbabilisticDesignPage::updateEstimatedDuration()
{
  if (!sampleTimeValueLabel_ || !sampleTimeLabel_ || !sampleSizeSpinBox_) return;

  const double seconds = sampleSizeSpinBox_->value() * modelEvalTime_;
  sampleTimeValueLabel_->setText(QString::number(seconds));
  const bool show = seconds > 1e-6;
  sampleTimeValueLabel_->setVisible(show);
  sampleTimeLabel_->setVisible(show);
}

} // namespace PERSALYS
