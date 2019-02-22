#include "otgui/FieldMonteCarloAnalysis.hxx"
#include "otgui/FieldMonteCarloWizard.hxx"
#include "otgui/SymbolicFieldModel.hxx"
#include "otgui/GridMeshModel.hxx"

#include <openturns/OTtypes.hxx>
#include <openturns/Normal.hxx>
#include <openturns/Uniform.hxx>

#include <QtTest/QtTest>

namespace OTGUI
{
class TestFieldMonteCarloWizard : public QObject
{
  Q_OBJECT

public:
  TestFieldMonteCarloWizard()
  {
    GridMeshModel meshModel(OT::Interval(0., 12.), OT::Indices(1, 5));

    Input z0("z0", 100, OT::Uniform(100, 150));
    Input v0("v0", 55, OT::Normal(55, 10));
    Input m("m", 80, OT::Normal(80, 8));
    Input c("c", 15, OT::Uniform(0, 30));
    Output z("z");
    InputCollection inputCollection(4);
    inputCollection[0] = z0;
    inputCollection[1] = v0;
    inputCollection[2] = m;
    inputCollection[2] = c;
    OutputCollection outputCollection(1, z);

    OT::Description formula(1, "max(0, z0 + (m * 9.81 / c) * t + (m / c) * (v0 - (m * 9.81 / c)) * (1 - exp(-t * c / m)))");
    model = SymbolicFieldModel("model", meshModel, inputCollection, outputCollection, formula);
  }

private:
  SymbolicFieldModel model;

private slots:
  void TestOutputsSelection()
  {
    // create the analysis
    FieldMonteCarloAnalysis analysis("analysis", model);

    // create the wizard
    FieldMonteCarloWizard wizard(analysis);
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
    FieldMonteCarloAnalysis analysis("analysis", model);

    // create the wizard
    FieldMonteCarloWizard wizard(analysis);
    wizard.show();

    // checks
    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters()==analysis.getParameters();
    QVERIFY2(analysisEquality, "The two FieldMonteCarloAnalysis must be equal");
  }
};
}

QTEST_MAIN(OTGUI::TestFieldMonteCarloWizard)
#include "t_FieldMonteCarloWizard_std.moc"
