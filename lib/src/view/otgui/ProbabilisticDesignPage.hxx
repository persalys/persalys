// ProbabilisticDesignPage.hxx

#ifndef PROBABILISTICDESIGNPAGE_H
#define PROBABILISTICDESIGNPAGE_H

#include "otgui/DesignOfExperiment.hxx"

#include <QWizardPage>

namespace OTGUI {
class ProbabilisticDesignPage : public QWizardPage
{
  Q_OBJECT

public:
  ProbabilisticDesignPage(const DesignOfExperiment & designOfExperiment, QWidget *parent = 0);

  bool validatePage();

protected:
  void buildInterface();

public slots:
  void setDesignOfExperiment(DesignOfExperiment &);
signals:
  void designOfExperimentChanged(const DesignOfExperiment & designOfExperiment);

private:
  DesignOfExperiment designOfExperiment_;
  bool pageValidity_;
  QWidget * designsWidget_ ;
};
}
#endif