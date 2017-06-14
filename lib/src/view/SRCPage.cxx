//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define src analysis
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
#include "otgui/SRCPage.hxx"

#include "otgui/SRCAnalysis.hxx"
#include "otgui/CollapsibleGroupBox.hxx"

#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>

using namespace OT;

namespace OTGUI {

SRCPage::SRCPage(QWidget* parent)
  : QWizardPage(parent)
  , sampleSizeSpinbox_(0)
  , seedSpinbox_(0)
{
  buildInterface();
}


void SRCPage::buildInterface()
{
  setTitle(tr("SRC parameters"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  // sample size
  QGroupBox * sampleSizeGroupBox = new QGroupBox(tr("Evaluation parameter"));
  QGridLayout * sampleSizeLayout = new QGridLayout(sampleSizeGroupBox);

  QLabel * sampleSizeLabel = new QLabel(tr("Sample size"));
  sampleSizeLayout->addWidget(sampleSizeLabel, 0, 0);

  sampleSizeSpinbox_ = new LogSpinBox;
  sampleSizeSpinbox_->setMinimum(2);
  sampleSizeSpinbox_->setMaximum(std::numeric_limits<int>::max());
  sampleSizeLabel->setBuddy(sampleSizeSpinbox_);
  sampleSizeLayout->addWidget(sampleSizeSpinbox_, 0, 1);

  pageLayout->addWidget(sampleSizeGroupBox);

  // advanced parameters
  CollapsibleGroupBox * advancedParamGroupBox = new CollapsibleGroupBox;
  advancedParamGroupBox->setTitle(tr("Advanced parameters"));
  QGridLayout * advancedParamGroupBoxLayout = new QGridLayout(advancedParamGroupBox);

  // seed
  QLabel * seedLabel = new QLabel(tr("Seed"));
  advancedParamGroupBoxLayout->addWidget(seedLabel, 0, 0);

  seedSpinbox_ = new QSpinBox;
  seedSpinbox_->setMaximum(std::numeric_limits<int>::max());
  seedLabel->setBuddy(seedSpinbox_);
  advancedParamGroupBoxLayout->addWidget(seedSpinbox_, 0, 1);

  advancedParamGroupBox->setExpanded(false);
  pageLayout->addWidget(advancedParamGroupBox);

  initialize(SRCAnalysis());
}


void SRCPage::initialize(const Analysis& analysis)
{
  const SRCAnalysis * analysis_ptr = dynamic_cast<const SRCAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
    return;

  sampleSizeSpinbox_->setValue(analysis_ptr->getSimulationsNumber());
  seedSpinbox_->setValue(analysis_ptr->getSeed());
}


Analysis SRCPage::getAnalysis(const String& name, const PhysicalModel& physicalModel) const
{
  SRCAnalysis analysis(name, physicalModel);
  analysis.setSimulationsNumber(sampleSizeSpinbox_->value());
  analysis.setSeed(seedSpinbox_->value());

  return analysis;
}


int SRCPage::nextId() const
{
  return -1;
}


bool SRCPage::validatePage()
{
  return true;
}
}