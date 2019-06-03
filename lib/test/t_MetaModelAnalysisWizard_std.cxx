#include "persalys/FunctionalChaosAnalysis.hxx"
#include "persalys/KrigingAnalysis.hxx"
#include "persalys/ProbabilisticDesignOfExperiment.hxx"
#include "persalys/MetaModelAnalysisWizard.hxx"
#include "persalys/SymbolicPhysicalModel.hxx"

#include <openturns/OTtypes.hxx>
#include <openturns/Normal.hxx>

#include <QtTest/QtTest>

namespace PERSALYS
{
class TestMetaModelAnalysisWizard : public QObject
{
  Q_OBJECT

public:
  TestMetaModelAnalysisWizard()
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

    // create the design of experiment
    doe = ProbabilisticDesignOfExperiment("analysis", model, 100);
    doe.run();
  }

private:
  ProbabilisticDesignOfExperiment doe;

private slots:
  void TestOutputsSelection()
  {
    // create the analysis
    FunctionalChaosAnalysis analysis("analysis", doe);

    // create the wizard
    MetaModelAnalysisWizard wizard(analysis);
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


  void TestFunctionalChaos()
  {
    // create the analysis
    FunctionalChaosAnalysis analysis("analysis", doe);

    // create the wizard
    MetaModelAnalysisWizard wizard(analysis);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();
    QVERIFY2(buttonGroup->checkedId() == MetaModelIntroPage::Chaos, "Checked button must be Chaos");
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0");

    // - second page
    wizard.next();
    TemporaryLabel * errorMessageLabel = wizard.functionalChaosPage_->findChild<TemporaryLabel*>();
    QVERIFY2(wizard.currentId() == 1, "Current page ID must be 1");
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->text().isEmpty(), "Label must be empty");
    // - second page
    wizard.next();
    QVERIFY2(wizard.currentId() == 3, "Current page ID must be 3");
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");

    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters()==analysis.getParameters();
    QVERIFY2(analysisEquality, "The two FunctionalChaosAnalysis must be equal");
  }


  void TestKriging()
  {
    // create the analysis
    KrigingAnalysis analysis("analysis", doe);

    // create the wizard
    MetaModelAnalysisWizard wizard(analysis);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();
    QVERIFY2(buttonGroup->checkedId() == MetaModelIntroPage::Kriging, "Checked button must be Kriging");
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0");

    // - second page
    wizard.next();
    QVERIFY2(wizard.currentId() == 2, "Current page ID must be 2");
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    // - second page
    wizard.next();
    QVERIFY2(wizard.currentId() == 3, "Current page ID must be 3");
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");

    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters()==analysis.getParameters();
    QVERIFY2(analysisEquality, "The two KrigingAnalysis must be equal");
  }


  void TestAnalysisModification()
  {
    // create the analysis
    FunctionalChaosAnalysis analysis("analysis", doe);

    // create the wizard
    MetaModelAnalysisWizard wizard(analysis);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();
    buttonGroup->button(MetaModelIntroPage::Kriging)->click();

    QVERIFY2(wizard.nextId() == 2, "Next page ID must be 2");

    bool analysisEquality = wizard.getAnalysis().getParameters()==KrigingAnalysis("analysis", doe).getParameters();
    QVERIFY2(analysisEquality, "The two KrigingAnalysis must be equal");
  }


  void TestFromModel()
  {
    Study aStudy;
    PhysicalModel aModel(doe.getPhysicalModel());
    aStudy.add(aModel);

    // create doe
    ProbabilisticDesignOfExperiment analysis1("analysis1", aModel);
    analysis1.run();
    aStudy.add(analysis1);
    ProbabilisticDesignOfExperiment analysis2("analysis2", aModel);
    analysis2.setSize(analysis1.getSize() * 2);
    analysis2.run();
    aStudy.add(analysis2);

    // create an analysis
    FunctionalChaosAnalysis analysis3("analysis3", analysis1);
    aStudy.add(analysis3);

    // create the wizard
    MetaModelAnalysisWizard wizard(analysis3, true);
    wizard.show();

    // checks
    QComboBox * comboBox = wizard.introPage_->findChild<QComboBox*>();
    QLabel * label = wizard.introPage_->findChild<QLabel*>("doeLabel_");
    QVERIFY2(comboBox->count() == 2, "The combobox must have two items");
    QVERIFY2(label->text().contains(QString::number(analysis1.getSize())), "Wrong label");
    QVERIFY2(wizard.introPage_->getDesignOfExperiment().getName() == "analysis1", "The doe must be named : analysis1");

    comboBox->setCurrentIndex(1);
    QVERIFY2(label->text().contains(QString::number(analysis1.getSize()*2)), "Wrong label");
    QVERIFY2(wizard.introPage_->getDesignOfExperiment().getName() == "analysis2", "The doe must be named : analysis2");

    QVERIFY2(wizard.nextId() == 1, "Next page ID must be 1");
  }
};
}

QTEST_MAIN(PERSALYS::TestMetaModelAnalysisWizard)
#include "t_MetaModelAnalysisWizard_std.moc"
