#include "persalys/GridDesignOfExperiment.hxx"
#include "persalys/ImportedDesignOfExperiment.hxx"
#include "persalys/ProbabilisticDesignOfExperiment.hxx"
#include "persalys/DesignOfExperimentWizard.hxx"
#include "persalys/SymbolicPhysicalModel.hxx"

#include <openturns/OTtypes.hxx>
#include <openturns/Normal.hxx>

#include <QtTest/QtTest>
#include <QAbstractButton>

namespace PERSALYS
{
class TestDesignOfExperimentWizard : public QObject
{
  Q_OBJECT

public:
  TestDesignOfExperimentWizard()
  {
    // create the model
    Input Q("Q", 10200, OT::Normal(10200, 100), "Primary energy");
    Input E("E", 3000, OT::Normal(3000, 15), "Produced electric energy");
    Input C("C", 4000, OT::Normal(4000, 60), "Valued thermal energy");
    Output Ep("Ep", "Primary energy savings");
    InputCollection inputCollection(3);
    inputCollection[0] = Q;
    inputCollection[1] = E;
    inputCollection[2] = C;
    OutputCollection outputCollection(1, Ep);

    OT::Description formula(1, "1-(Q/((E/((1-0.05)*0.54))+(C/0.8)))");
    model = SymbolicPhysicalModel("model", inputCollection, outputCollection, formula);
  }

private:
  SymbolicPhysicalModel model;

private slots:
  void TestDeterministic()
  {
    // create the analysis
    GridDesignOfExperiment doe("analysis", model);

    // create the wizard
    DesignOfExperimentWizard wizard(doe);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();
    QVERIFY2(buttonGroup->checkedId() == DesignOfExperimentIntroPage::Deterministic, "Checked button must be Deterministic");
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0");

    // - second page
    wizard.next();
    TemporaryLabel * errorMessageLabel = wizard.gridPage_->findChild<TemporaryLabel*>();
    QVERIFY2(wizard.currentId() == 1, "Current page ID must be 1");
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->text().isEmpty(), "Label must be empty");

    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters()==doe.getParameters();
    QVERIFY2(analysisEquality, "The two GridDesignOfExperiment must be equal");
  }


  void TestProbabilistic()
  {
    // create the analysis
    ProbabilisticDesignOfExperiment doe("analysis", model);

    // create the wizard
    DesignOfExperimentWizard wizard(doe);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();
    QVERIFY2(buttonGroup->checkedId() == DesignOfExperimentIntroPage::Probabilistic, "Checked button must be Probabilistic");
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0");

    // - second page
    wizard.next();
    QVERIFY2(wizard.currentId() == 2, "Current page ID must be 2");
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");

    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters()==doe.getParameters();
    QVERIFY2(analysisEquality, "The two ProbabilisticDesignOfExperiment must be equal");
  }


  void TestImport()
  {
    // create the analysis
    ImportedDesignOfExperiment doe("analysis", model);

    // create the wizard
    DesignOfExperimentWizard wizard(doe);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();
    QVERIFY2(buttonGroup->checkedId() == DesignOfExperimentIntroPage::Import, "Checked button must be Import");
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0");

    // - second page
    wizard.next();
    TemporaryLabel * errorMessageLabel = wizard.importPage_->findChild<TemporaryLabel*>();
    QVERIFY2(wizard.currentId() == 3, "Current page ID must be 3");
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!errorMessageLabel->text().isEmpty(), "Label must be not empty");

    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters()==doe.getParameters();
    QVERIFY2(analysisEquality, "The two ImportedDesignOfExperiment must be equal");
  }


  void TestAnalysisModification()
  {
    // create the analysis
    GridDesignOfExperiment doe("analysis", model);

    // create the wizard
    DesignOfExperimentWizard wizard(doe);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();
    buttonGroup->button(DesignOfExperimentIntroPage::Probabilistic)->click();
    QVERIFY2(wizard.nextId() == 2, "Next page ID must be 2");
    bool analysisEquality = wizard.getAnalysis().getParameters()==ProbabilisticDesignOfExperiment("analysis", model).getParameters();
    QVERIFY2(analysisEquality, "The two ProbabilisticDesignOfExperiment must be equal");

    buttonGroup->button(DesignOfExperimentIntroPage::Import)->click();
    QVERIFY2(wizard.nextId() == 3, "Next page ID must be 3");
    analysisEquality = wizard.getAnalysis().getParameters()==ImportedDesignOfExperiment("analysis", model).getParameters();
    QVERIFY2(analysisEquality, "The two ImportedDesignOfExperiment must be equal");
  }
};
}

QTEST_MAIN(PERSALYS::TestDesignOfExperimentWizard)
#include "t_DesignOfExperimentWizard_std.moc"
