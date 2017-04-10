//                                               -*- C++ -*-
/**
 *  @brief QWizard introducing the methods to define designs of experiments
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
#include "otgui/DesignOfExperimentWizard.hxx"

#include "otgui/ImportDesignOfExperimentPage.hxx"
#include "otgui/DeterministicDesignPage.hxx"
#include "otgui/ProbabilisticDesignPage.hxx"

#include <QVBoxLayout>
#include <QTableView>
#include <QGroupBox>
#include <QRadioButton>

namespace OTGUI {

IntroPage::IntroPage(const DesignOfExperiment & designOfExperiment, QWidget *parent)
  : QWizardPage(parent)
{
  setTitle(tr("Introduction"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  QGroupBox * methodBox = new QGroupBox(tr("Type of design of experiment"));
  QVBoxLayout * methodLayout = new QVBoxLayout(methodBox);

  methodGroup_ = new QButtonGroup;
  QRadioButton * buttonToChooseMethod = new QRadioButton(tr("Deterministic"));
  if (designOfExperiment.getImplementation()->getClassName() != "FromFileDesignOfExperiment")
  {
// TODO   + || designOfExperiment.getImplementation()->getClassName() == "GeneratedDeterministicDesignOfExperiment"
    buttonToChooseMethod->setChecked(true);
  }
  methodGroup_->addButton(buttonToChooseMethod, IntroPage::deterministic);
  methodLayout->addWidget(buttonToChooseMethod);

//   TODO
//   buttonToChooseMethod = new QRadioButton(tr("Probabilistic"));
//   buttonToChooseMethod->setEnabled(designOfExperiment.getPhysicalModel().hasStochasticInputs());
//   if (designOfExperiment.getImplementation()->getClassName() == "GeneratedProbabilisticDesignOfExperiment")
//     buttonToChooseMethod->setChecked(true);
//   methodGroup_->addButton(buttonToChooseMethod, IntroPage::probabilistic);
//   methodLayout->addWidget(buttonToChooseMethod);
  buttonToChooseMethod = new QRadioButton(tr("Import data"));
  if (designOfExperiment.getImplementation()->getClassName() == "FromFileDesignOfExperiment")
    buttonToChooseMethod->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethod, IntroPage::import);
  methodLayout->addWidget(buttonToChooseMethod);

  pageLayout->addWidget(methodBox);
}


int IntroPage::nextId() const
{
  switch (IntroPage::Method(methodGroup_->checkedId()))
  {
    case IntroPage::deterministic:
    {
      return DesignOfExperimentWizard::Page_Deterministic;
    }
//     TODO
//     case IntroPage::probabilistic:
//     {
//       return DesignOfExperimentWizard::Page_Probabilistic;
//     }
    case IntroPage::import:
    {
      return DesignOfExperimentWizard::Page_Import;
    }
    default:
      return -1;
      break;
  }
}


DesignOfExperimentWizard::DesignOfExperimentWizard(const OTStudy& otStudy, const PhysicalModel & physicalModel, QWidget* parent)
  : OTguiWizard(parent)
  , designOfExperiment_(DesignOfExperiment(otStudy.getAvailableDesignOfExperimentName(), physicalModel))
{
  buildInterface();
}


DesignOfExperimentWizard::DesignOfExperimentWizard(const DesignOfExperiment & designOfExperiment, QWidget *parent)
  : OTguiWizard(parent)
  , designOfExperiment_(designOfExperiment.getImplementation()->clone())
{
  designOfExperiment_.getImplementation()->initialize();
  buildInterface();
}


void DesignOfExperimentWizard::buildInterface()
{
  setWindowTitle(tr("Design of experiment"));

  introPage_ = new IntroPage(designOfExperiment_);
  setPage(Page_Intro, introPage_);
  DeterministicDesignPage * deterministicDesignPage = new DeterministicDesignPage(designOfExperiment_.getImplementation());
  connect(deterministicDesignPage, SIGNAL(designOfExperimentChanged(const DesignOfExperiment&)), this, SLOT(setDesignOfExperiment(const DesignOfExperiment&)));
  setPage(Page_Deterministic, deterministicDesignPage);
  // TODO
//   setPage(Page_Probabilistic, new ProbabilisticDesignPage(designOfExperiment_));
  ImportDesignOfExperimentPage * importPage = new ImportDesignOfExperimentPage(designOfExperiment_.getImplementation());
  connect(importPage, SIGNAL(designOfExperimentChanged(const DesignOfExperiment&)), this, SLOT(setDesignOfExperiment(const DesignOfExperiment&)));
  setPage(Page_Import, importPage);

  setStartId(Page_Intro);
}


int DesignOfExperimentWizard::nextId() const
{
  switch (currentId())
  {
    case Page_Intro:
      return introPage_->nextId();
    case Page_Deterministic:
  // TODO
//     case Page_Probabilistic:
    case Page_Import:
    default:
      return -1;
  }
}


DesignOfExperiment DesignOfExperimentWizard::getDesignOfExperiment() const
{
  return designOfExperiment_;
}


void DesignOfExperimentWizard::setDesignOfExperiment(const DesignOfExperiment & designOfExperiment)
{
  designOfExperiment_ = designOfExperiment;
}
}
