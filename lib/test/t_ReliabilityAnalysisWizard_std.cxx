#include "persalys/MonteCarloReliabilityAnalysis.hxx"
#include "persalys/FORMImportanceSamplingAnalysis.hxx"
#include "persalys/FORMAnalysis.hxx"
#include "persalys/SORMAnalysis.hxx"
#include "persalys/ReliabilityAnalysisWizard.hxx"
#include "persalys/SymbolicPhysicalModel.hxx"

#include <openturns/OTtypes.hxx>
#include <openturns/Normal.hxx>

#include <QtTest/QtTest>

namespace PERSALYS
{
class TestReliabilityAnalysisWizard : public QObject
{
  Q_OBJECT

public:
  TestReliabilityAnalysisWizard()
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
    SymbolicPhysicalModel model("model", inputCollection, outputCollection, formula);

    // create the limit state
    limitState = LimitState("limitState", model, "Ep", OT::Less(), 0.025);
  }

private:
  LimitState limitState;

private slots:
  void TestMonteCarloReliabilityAnalysis()
  {
    // create the analysis
    MonteCarloReliabilityAnalysis analysis("analysis", limitState);

    // create the wizard
    ReliabilityAnalysisWizard wizard(analysis);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();
    QVERIFY2(buttonGroup->checkedId() == ReliabilityIntroPage::MonteCarlo, "Checked button must be MonteCarlo");
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0");
    // - second page
    wizard.next();
    TemporaryLabel * errorMessageLabel = wizard.simulationPage_->findChild<TemporaryLabel*>();
    QVERIFY2(wizard.currentId() == 1, "Current page ID must be 1");
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->text().isEmpty(), "Label must be empty");

    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters()==analysis.getParameters();
    QVERIFY2(analysisEquality, "The two MonteCarloReliabilityAnalysis must be equal");
  }


  void TestFORMImportanceSamplingAnalysis()
  {
    // create the analysis
    FORMImportanceSamplingAnalysis analysis("analysis", limitState);

    // create the wizard
    ReliabilityAnalysisWizard wizard(analysis);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();
    QVERIFY2(buttonGroup->checkedId() == ReliabilityIntroPage::FORM_IS, "Checked button must be FORM_IS");
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0");
    // - second page
    wizard.next();
    TemporaryLabel * errorMessageLabel = wizard.simulationPage_->findChild<TemporaryLabel*>();
    QVERIFY2(wizard.currentId() == 1, "Current page ID must be 1");
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->text().isEmpty(), "Label must be empty");
    // - third page
    wizard.next();
    QVERIFY2(wizard.currentId() == 3, "Current page ID must be 3");
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");

    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters()==analysis.getParameters();
    QVERIFY2(analysisEquality, "The two FORMImportanceSamplingAnalysis must be equal");
  }


  void TestFORMAnalysis()
  {
    // create the analysis
    FORMAnalysis analysis("analysis", limitState);

    // create the wizard
    ReliabilityAnalysisWizard wizard(analysis);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();
    QVERIFY2(buttonGroup->checkedId() == ReliabilityIntroPage::FORM, "Checked button must be FORM");
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0");
    // - second page
    wizard.next();
    QVERIFY2(wizard.currentId() == 2, "Current page ID must be 2");
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");

    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters()==analysis.getParameters();
    QVERIFY2(analysisEquality, "The two FORMAnalysis must be equal");
  }


  void TestSORMAnalysis()
  {
    // create the analysis
    SORMAnalysis analysis("analysis", limitState);

    // create the wizard
    ReliabilityAnalysisWizard wizard(analysis);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();
    QVERIFY2(buttonGroup->checkedId() == ReliabilityIntroPage::SORM, "Checked button must be SORM");
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0");
    // - second page
    wizard.next();
    QVERIFY2(wizard.currentId() == 2, "Current page ID must be 2");
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");

    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters()==analysis.getParameters();
    QVERIFY2(analysisEquality, "The two SORMAnalysis must be equal");
  }


  void TestAnalysisModification()
  {
    // create the analysis
    MonteCarloReliabilityAnalysis analysis("analysis", limitState);

    // create the wizard
    ReliabilityAnalysisWizard wizard(analysis);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();

    buttonGroup->button(ReliabilityIntroPage::FORM_IS)->click();
    QVERIFY2(wizard.nextId() == 1, "Next page ID must be 1");
    bool analysisEquality = wizard.getAnalysis().getParameters()==FORMImportanceSamplingAnalysis("analysis", limitState).getParameters();
    QVERIFY2(analysisEquality, "The two FORMImportanceSamplingAnalysis must be equal");

    buttonGroup->button(ReliabilityIntroPage::FORM)->click();
    QVERIFY2(wizard.nextId() == 2, "Next page ID must be 2");
    analysisEquality = wizard.getAnalysis().getParameters()==FORMAnalysis("analysis", limitState).getParameters();
    QVERIFY2(analysisEquality, "The two FORMAnalysis must be equal");

    buttonGroup->button(ReliabilityIntroPage::SORM)->click();
    QVERIFY2(wizard.nextId() == 2, "Next page ID must be 2");
    analysisEquality = wizard.getAnalysis().getParameters()==SORMAnalysis("analysis", limitState).getParameters();
    QVERIFY2(analysisEquality, "The two SORMAnalysis must be equal");
  }


  void TestFromModel()
  {
    Study aStudy;
    PhysicalModel aModel(limitState.getPhysicalModel());
    aStudy.add(aModel);
    LimitState limitState1("limitState1", aModel, "Ep", OT::Less(), 0.022);
    aStudy.add(limitState1);
    LimitState limitState2("limitState2", aModel, "Ep", OT::Less(), 0.025);
    aStudy.add(limitState2);

    // create an analysis
    MonteCarloReliabilityAnalysis analysis1("analysis", limitState1);
    aStudy.add(analysis1);

    // create the wizard
    ReliabilityAnalysisWizard wizard(analysis1, true);
    wizard.show();

    // checks
    QComboBox * comboBox = wizard.introPage_->findChild<QComboBox*>();
    QLabel * label = wizard.introPage_->findChild<QLabel*>("limitStateLabel_");
    QVERIFY2(comboBox->count() == 2, "The combobox must have two items");
    QVERIFY2(label->text() == limitState1.__str__().c_str(), "Wrong limit state label");
    QVERIFY2(wizard.introPage_->getLimitState().getName() == "limitState1", "The limit state must be named : limitState1");

    comboBox->setCurrentIndex(1);
    QVERIFY2(label->text() == limitState2.__str__().c_str(), "Wrong limit state label");
    QVERIFY2(wizard.introPage_->getLimitState().getName() == "limitState2", "The limit state must be named : limitState2");

    QVERIFY2(wizard.nextId() == 1, "Next page ID must be 1");
  }
};
}

QTEST_MAIN(PERSALYS::TestReliabilityAnalysisWizard)
#include "t_ReliabilityAnalysisWizard_std.moc"
