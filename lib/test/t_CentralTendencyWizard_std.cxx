#include "otgui/MonteCarloAnalysis.hxx"
#include "otgui/TaylorExpansionMomentsAnalysis.hxx"
#include "otgui/CentralTendencyWizard.hxx"
#include "otgui/SymbolicPhysicalModel.hxx"

#include <openturns/OTtypes.hxx>
#include <openturns/Normal.hxx>

#include <QtTest/QtTest>

namespace OTGUI
{
class TestCentralTendencyWizard : public QObject
{
  Q_OBJECT

public:
  TestCentralTendencyWizard()
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
  void TestOutputsSelection()
  {
    // create the analysis
    MonteCarloAnalysis analysis("analysis", model);

    // create the wizard
    CentralTendencyWizard wizard(analysis);
    wizard.show();

    // checks

    // - first page
    OutputsSelectionGroupBox * outputsSelectionGroupBox = wizard.introPage_->findChild<OutputsSelectionGroupBox*>();
    TemporaryLabel * errorMessageLabel = wizard.introPage_->findChild<TemporaryLabel*>();
    TitledComboBox * comboBox = outputsSelectionGroupBox->findChild<TitledComboBox*>();
    ListWidgetWithCheckBox * listWidget = outputsSelectionGroupBox->findChild<ListWidgetWithCheckBox*>();

    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->text().isEmpty(), "Label must be empty");

    QTest::mouseClick(comboBox, Qt::LeftButton); // open listwidget
    QTest::mouseClick(listWidget->viewport(), Qt::LeftButton); // deselect all
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!errorMessageLabel->text().isEmpty(), "Label must be not empty");

    wizard.next();
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0"); // can not go to next page

    QTest::mouseClick(listWidget->viewport(), Qt::LeftButton); // select all
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->text().isEmpty(), "Label must be empty");
  }


  void TestMonteCarloAnalysis()
  {
    // create the analysis
    MonteCarloAnalysis analysis("analysis", model);

    // create the wizard
    CentralTendencyWizard wizard(analysis);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();
    QVERIFY2(buttonGroup->checkedId() == CentralTendencyIntroPage::MonteCarlo, "Checked button must be MonteCarlo");
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0");

    // - second page
    wizard.next();
    TemporaryLabel * errorMessageLabel = wizard.monteCarloPage_->findChild<TemporaryLabel*>();
    QVERIFY2(wizard.currentId() == 1, "Current page ID must be 1");
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->text().isEmpty(), "Label must be empty");

    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters()==analysis.getParameters();
    QVERIFY2(analysisEquality, "The two MonteCarloAnalysis must be equal");
  }


  void TestTaylor()
  {
    // create the analysis
    TaylorExpansionMomentsAnalysis analysis("analysis", model);

    // create the wizard
    CentralTendencyWizard wizard(analysis);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();
    QVERIFY2(buttonGroup->checkedId() == CentralTendencyIntroPage::TaylorExpansionMoments, "Checked button must be Taylor");
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0");

    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters()==analysis.getParameters();
    QVERIFY2(analysisEquality, "The two TaylorExpansionMomentsAnalysis must be equal");
  }


  void TestAnalysisModification()
  {
    // create the analysis
    MonteCarloAnalysis analysis("analysis", model);

    // create the wizard
    CentralTendencyWizard wizard(analysis);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();
    buttonGroup->button(CentralTendencyIntroPage::TaylorExpansionMoments)->click();

    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters()==TaylorExpansionMomentsAnalysis("analysis", model).getParameters();
    QVERIFY2(analysisEquality, "The two TaylorExpansionMomentsAnalysis must be equal");
  }
};
}

QTEST_MAIN(OTGUI::TestCentralTendencyWizard)
#include "t_CentralTendencyWizard_std.moc"
