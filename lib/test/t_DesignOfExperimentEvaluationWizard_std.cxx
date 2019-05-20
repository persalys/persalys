#include "persalys/ProbabilisticDesignOfExperiment.hxx"
#include "persalys/DesignOfExperimentEvaluationWizard.hxx"
#include "persalys/SymbolicPhysicalModel.hxx"
#include "persalys/Study.hxx"

#include <openturns/OTtypes.hxx>
#include <openturns/Normal.hxx>

#include <QtTest/QtTest>

namespace PERSALYS
{
class TestDesignOfExperimentEvaluationWizard : public QObject
{
  Q_OBJECT

public:
  TestDesignOfExperimentEvaluationWizard()
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
    ProbabilisticDesignOfExperiment analysis("analysis", model);

    // create the wizard
    DesignOfExperimentEvaluationWizard wizard(analysis);
    wizard.show();

    // checks

    // - first page
    OutputsSelectionGroupBox * outputsSelectionGroupBox = wizard.findChild<OutputsSelectionGroupBox*>();
    TemporaryLabel * errorMessageLabel = wizard.findChild<TemporaryLabel*>();
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


  void TestAnalysis()
  {
    // create the analysis
    ProbabilisticDesignOfExperiment analysis("analysis", model);

    // create the wizard
    DesignOfExperimentEvaluationWizard wizard(analysis);
    wizard.show();

    // checks
    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters()==analysis.getParameters();
    QVERIFY2(analysisEquality, "The two DesignOfExperimentEvaluation must be equal");
  }


  void TestFromModel()
  {
    Study aStudy;
    PhysicalModel aModel(model);
    aStudy.add(aModel);

    // create analyses
    ProbabilisticDesignOfExperiment analysis1("analysis1", aModel);
    aStudy.add(analysis1);
    ProbabilisticDesignOfExperiment analysis2("analysis2", aModel);
    analysis2.setSize(analysis1.getSize() * 2);
    aStudy.add(analysis2);

    // create the wizard
    DesignOfExperimentEvaluationWizard wizard(aModel);
    wizard.show();

    // checks
    QVERIFY2(wizard.doesComboBox_->count() == 2, "The combobox must have two items");
    QVERIFY2(wizard.getAnalysis().getName() == "analysis1", "The analysis must be named : analysis1");
    QVERIFY2(wizard.doeLabel_->text().contains(QString::number(analysis1.getSize())), "Wrong label");

    wizard.doesComboBox_->setCurrentIndex(1);
    QVERIFY2(wizard.getAnalysis().getName() == "analysis2", "The analysis must be named : analysis2");
    QVERIFY2(wizard.doeLabel_->text().contains(QString::number(analysis1.getSize()*2)), "Wrong label");

    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");
  }
};
}

QTEST_MAIN(PERSALYS::TestDesignOfExperimentEvaluationWizard)
#include "t_DesignOfExperimentEvaluationWizard_std.moc"
