// DeterministicDesignPage.hxx

#ifndef DETERMINISTICDESIGNPAGE_H
#define DETERMINISTICDESIGNPAGE_H

#include "otgui/ExperimentTableModel.hxx"

#include <QWizardPage>
#include <QTableView>
#include <QButtonGroup>

namespace OTGUI {
class DeterministicDesignPage : public QWizardPage
{
  Q_OBJECT

public:
  DeterministicDesignPage(const DesignOfExperiment & designOfExperiment, QWidget *parent = 0);

  bool validatePage();

protected:
  void buildInterface();

public slots:
  void setEnabledWidgets(int);
  void setDesignOfExperiment(DesignOfExperiment &);
signals:
  void designOfExperimentChanged(const DesignOfExperiment & designOfExperiment);

private:
  DesignOfExperiment designOfExperiment_;
  bool pageValidity_;
  QButtonGroup * methodGroup_;
  QTableView * tableView_;
  ExperimentTableModel * model_;
  QWidget * designsWidget_ ;
};
}
#endif