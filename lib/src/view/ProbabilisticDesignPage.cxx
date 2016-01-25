// ProbabilisticDesignPage.cxx

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