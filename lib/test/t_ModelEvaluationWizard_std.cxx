#include "otgui/ModelEvaluation.hxx"
#include "otgui/ModelEvaluationWizard.hxx"
#include "otgui/SymbolicPhysicalModel.hxx"

#include <openturns/OTtypes.hxx>
#include <openturns/Normal.hxx>

#include <QtTest/QtTest>

namespace OTGUI
{
class TestModelEvaluationWizard : public QObject
{
  Q_OBJECT

public:
  TestModelEvaluationWizard()
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
    ModelEvaluation analysis("analysis", model);

    // create the wizard
    ModelEvaluationWizard wizard(analysis);
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
    QTest::keyClick(listWidget, Qt::Key_Space); // deselect all
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!errorMessageLabel->text().isEmpty(), "Label must be not empty");

    wizard.next();
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0"); // can not go to next page

    QTest::keyClick(listWidget, Qt::Key_Space); // select all
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->text().isEmpty(), "Label must be empty");
  }


  void TestAnalysis()
  {
    // create the analysis
    ModelEvaluation analysis("analysis", model);

    // create the wizard
    ModelEvaluationWizard wizard(analysis);
    wizard.show();

    // checks
    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters()==analysis.getParameters();
    QVERIFY2(analysisEquality, "The two ModelEvaluation must be equal");
  }
};
}

QTEST_MAIN(OTGUI::TestModelEvaluationWizard)
#include "t_ModelEvaluationWizard_std.moc"
