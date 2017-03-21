//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define a probabilistic design of experiment
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
#include "otgui/ProbabilisticDesignPage.hxx"

#include <QVBoxLayout>
#include <QRadioButton>
#include <QGroupBox>
#include <QButtonGroup>

using namespace OT;

namespace OTGUI {

ProbabilisticDesignPage::ProbabilisticDesignPage(const DesignOfExperiment & designOfExperiment, QWidget* parent)
  : QWizardPage(parent)
  , designOfExperiment_(designOfExperiment)
{
  buildInterface();
}

void ProbabilisticDesignPage::buildInterface()
{
  setTitle(tr("Probabilistic design of experiment"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  QGroupBox * methodGroup = new QGroupBox(tr("Designs"));
  QVBoxLayout * methodLayout = new QVBoxLayout(methodGroup);
  QButtonGroup * designsGroup = new QButtonGroup;
  designsWidget_ = new QWidget;
  QVBoxLayout * designsLayout = new QVBoxLayout(designsWidget_);
  QRadioButton * buttonToChooseDesign = new QRadioButton(tr("Latin Hypercube Sampling"));
  buttonToChooseDesign->setChecked(true);
  designsGroup->addButton(buttonToChooseDesign);
  designsLayout->addWidget(buttonToChooseDesign);

  buttonToChooseDesign = new QRadioButton(tr("Quasi-Monte Carlo"));
  designsGroup->addButton(buttonToChooseDesign);
  designsLayout->addWidget(buttonToChooseDesign);

  buttonToChooseDesign = new QRadioButton(tr("Monte Carlo"));
  designsGroup->addButton(buttonToChooseDesign);
  designsLayout->addWidget(buttonToChooseDesign);

  methodLayout->addWidget(designsWidget_);
  pageLayout->addWidget(methodGroup);
}


void ProbabilisticDesignPage::setDesignOfExperiment(DesignOfExperiment & designOfExperiment)
{
  designOfExperiment_ = designOfExperiment;
}


bool ProbabilisticDesignPage::validatePage()
{
  return pageValidity_;
}
}