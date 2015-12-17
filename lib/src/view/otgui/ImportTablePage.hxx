// ImportTablePage.hxx

#ifndef IMPORTTABLEPAGE_H
#define IMPORTTABLEPAGE_H

#include "OTguiTableView.hxx"
#include "otgui/DesignOfExperiment.hxx"

#include <QWizardPage>
#include <QLineEdit>

namespace OTGUI {
class ImportTablePage : public QWizardPage
{
  Q_OBJECT

public:
  ImportTablePage(const DesignOfExperiment & designOfExperiment, QWidget *parent = 0);

  bool validatePage();

protected:
  void buildInterface();

public slots:
  void openFileRequested();
  void columnChanged(Qt::Orientation, int, int);
signals:
  void dataImported(OT::NumericalSample);

private:
  DesignOfExperiment designOfExperiment_;
  bool pageValidity_;
  QLineEdit * filePathLineEdit_;
  OTguiTableView * dataPreviewTableView_;
};
}
#endif