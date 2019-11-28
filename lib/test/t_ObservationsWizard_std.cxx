#include "persalys/Observations.hxx"
#include "persalys/ObservationsWizard.hxx"
#include "persalys/SymbolicPhysicalModel.hxx"
#include "persalys/SampleTableModel.hxx"

#include <openturns/OTType.hxx>
#include <openturns/Normal.hxx>

#include <QtTest/QtTest>
#include <QAbstractButton>

#include <stdio.h>

using namespace OT;

namespace PERSALYS
{
class TestObservationsWizard : public QObject
{
  Q_OBJECT

public:
  TestObservationsWizard()
  {
    // create the model
    Input Q("Q", 10200, Normal(10200, 100), "Primary energy");
    Input E("E", 3000, Normal(3000, 15), "Produced electric energy");
    Input C("C", 4000, Normal(4000, 60), "Valued thermal energy");
    Output Ep("Ep", "Primary energy savings");
    Output Ep2("Ep2", "Primary energy savings");
    InputCollection inputCollection(3);
    inputCollection[0] = Q;
    inputCollection[1] = E;
    inputCollection[2] = C;
    OutputCollection outputCollection(1, Ep);
    outputCollection.add(Ep2);

    Description formula(2, "1-(Q/((E/((1-0.05)*0.54))+(C/0.8)))");
    model_ = SymbolicPhysicalModel("model", inputCollection, outputCollection, formula);
  }

private:
  SymbolicPhysicalModel model_;

private slots:
  void TestImport()
  {
    // create the observations
    String filename = "normal2.csv";
    Normal(5).getSample(10).exportToCSVFile(filename);
    Observations obs("obs", model_, filename, Indices(1, 2), Indices(1, 0), Description(1, "E"), Description(1, "Ep2"));

    // create the wizard
    ObservationsWizard wizard(obs);
    wizard.show();

    TemporaryLabel * errorMessageLabel = wizard.page_->findChild<TemporaryLabel*>();
    SampleTableModel * model = wizard.page_->findChild<SampleTableModel*>();
    QLineEdit * fileLineEdit = wizard.page_->findChild<QLineEdit*>();

    // checks
    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");
    QVERIFY2(fileLineEdit->text() == filename.c_str(), "wrong filename");
    QVERIFY2(model->headerData(0, Qt::Horizontal).toString() == "Ep2", "header not valid");
    QVERIFY2(model->headerData(2, Qt::Horizontal).toString() == "E", "header not valid");

    model->setHeaderData(0, Qt::Horizontal, QString(), Qt::DisplayRole);
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!errorMessageLabel->text().isEmpty(), "Label must be not empty");

    model->setHeaderData(0, Qt::Horizontal, "Ep2", Qt::DisplayRole);
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->text().isEmpty(), "Label must be empty");

    model->setHeaderData(2, Qt::Horizontal, "", Qt::DisplayRole);
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!errorMessageLabel->text().isEmpty(), "Label must be not empty");

    model->setHeaderData(2, Qt::Horizontal, "E", Qt::DisplayRole);
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->text().isEmpty(), "Label must be empty");

    QVERIFY2(wizard.getDesignOfExperiment().__repr__()==obs.__repr__(), "The two Observations must be equal");

    remove(filename.c_str());
  }
};
}

QTEST_MAIN(PERSALYS::TestObservationsWizard)
#include "t_ObservationsWizard_std.moc"
