#include "persalys/SobolAnalysis.hxx"
#include "persalys/SensitivityAnalysisWizard.hxx"
#include "persalys/SymbolicPhysicalModel.hxx"
#include "persalys/ErrorWidget.hxx"

#include <openturns/OTtypes.hxx>
#include <openturns/Normal.hxx>

#include <QtTest/QtTest>

namespace PERSALYS
{
class TestSensitivityAnalysisWizard : public QObject
{
  Q_OBJECT

public:
  TestSensitivityAnalysisWizard()
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
  void TestOutputsSelection() const
  {
    // create the analysis
    SobolAnalysis analysis("analysis", model);

    // create the wizard
    SensitivityAnalysisWizard wizard(analysis);
    wizard.show();

    // checks

    // - first page
    const auto * outputsSelectionGroupBox = wizard.introPage_->findChild<OutputsSelectionGroupBox*>();
    const auto * errorMessageLabel = wizard.introPage_->findChild<ErrorWidget*>();
    auto * comboBox = outputsSelectionGroupBox->findChild<TitledComboBox*>();
    const auto * listWidget = outputsSelectionGroupBox->findChild<ListWidgetWithCheckBox*>();

    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->toPlainText().isEmpty(), "Label must be empty");

    QTest::mouseClick(comboBox, Qt::LeftButton); // open listwidget
    QTest::mouseClick(listWidget->viewport(), Qt::LeftButton); // deselect all
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!errorMessageLabel->toPlainText().isEmpty(), "Label must be not empty");

    wizard.next();
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0"); // can not go to next page

    QTest::mouseClick(listWidget->viewport(), Qt::LeftButton); // select all
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->toPlainText().isEmpty(), "Label must be empty");
  }


  void TestSobol() const
  {
    // create the analysis
    SobolAnalysis analysis("analysis", model);

    // create the wizard
    SensitivityAnalysisWizard wizard(analysis);
    wizard.show();

    // checks

    // - first page
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0");
    // - second page
    wizard.next();
    const auto * errorMessageLabel = wizard.sobolPage_->findChild<ErrorWidget*>();
    QVERIFY2(wizard.currentId() == 1, "Current page ID must be 1");
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->toPlainText().isEmpty(), "Label must be empty");

    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters() == analysis.getParameters();
    QVERIFY2(analysisEquality, "The two SobolAnalysis must be equal");
  }
};
}

QTEST_MAIN(PERSALYS::TestSensitivityAnalysisWizard)
#include "t_SensitivityAnalysisWizard_std.moc"
