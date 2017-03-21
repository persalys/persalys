//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define approximation reliability analysis
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
#include "otgui/ApproximationReliabilityPage.hxx"

#include "otgui/FORMAnalysis.hxx"

#include <QVBoxLayout>
#include <QRadioButton>
#include <QGroupBox>

using namespace OT;

namespace OTGUI {

ApproximationReliabilityPage::ApproximationReliabilityPage(QWidget* parent)
  : QWizardPage(parent)
{
  buildInterface();
}


void ApproximationReliabilityPage::buildInterface()
{
  setTitle(tr("Approximation method"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  QGroupBox * methodBox = new QGroupBox(tr("Method"));
  QVBoxLayout * methodLayout = new QVBoxLayout(methodBox);

  // radioButtons
  methodGroup_ = new QButtonGroup;
  // FORM
  QRadioButton * buttonToChooseMethod = new QRadioButton(tr("First order analysis (FORM)"));
  buttonToChooseMethod->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethod, ApproximationReliabilityPage::FORM);
  methodLayout->addWidget(buttonToChooseMethod);

  pageLayout->addWidget(methodBox);

  // optimization widgets
  optimWidget_ = new OptimizationWidget(this);

  pageLayout->addWidget(optimWidget_);
}


void ApproximationReliabilityPage::initialize(const Analysis& analysis)
{
  methodGroup_->button(ApproximationReliabilityPage::FORM)->click();
  optimWidget_->initialize(analysis);
}


Analysis ApproximationReliabilityPage::getAnalysis(const String& name, const LimitState& limitState) const
{
  FORMAnalysis analysis(name, limitState);
  analysis.setOptimizationAlgorithm(optimWidget_->getOptimizationAlgorithm());
  analysis.setPhysicalStartingPoint(optimWidget_->getOptimizationAlgorithm().getStartingPoint());

  return analysis;
}


int ApproximationReliabilityPage::nextId() const
{
  return -1;
}


bool ApproximationReliabilityPage::validatePage()
{
  return true;
}
}