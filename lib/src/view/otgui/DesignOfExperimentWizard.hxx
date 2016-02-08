// DesignOfExperimentWizard.hxx

#ifndef DESIGNOFEXPERIMENTWIZARD_H
#define DESIGNOFEXPERIMENTWIZARD_H

#include "otgui/OTStudy.hxx"
#include "otgui/OTguiWizard.hxx"

#include <QButtonGroup>

namespace OTGUI {

class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
  enum Method {deterministic, probabilistic, import};

  IntroPage(const DesignOfExperiment & designOfExperiment, QWidget *parent = 0);

  int nextId() const;

private:
  QButtonGroup * methodGroup_;
};


class DesignOfExperimentWizard : public OTguiWizard
{
  Q_OBJECT

public:

  enum {Page_Intro, Page_Deterministic, Page_Probabilistic, Page_Import};

  DesignOfExperimentWizard(OTStudy * otStudy, const PhysicalModel & physicalModel);
  DesignOfExperimentWizard(const DesignOfExperiment & designOfExperiment);

  int nextId() const;
  void validate();

protected:
  void buildInterface();

public slots:
  void setDesignOfExperiment(DesignOfExperiment & designOfExperiment);

private:
  IntroPage * introPage_;
  DesignOfExperiment designOfExperiment_;
  OTStudy * otStudy_;
};
}
#endif