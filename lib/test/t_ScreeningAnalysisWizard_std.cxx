#include "persalys/MorrisAnalysis.hxx"
#include "persalys/ScreeningAnalysisWizard.hxx"
#include "persalys/SymbolicPhysicalModel.hxx"

#include <openturns/OTtypes.hxx>
#include <openturns/Normal.hxx>

#include <QtTest/QtTest>

namespace PERSALYS
{
class TestScreeningAnalysisWizard : public QObject
{
  Q_OBJECT

public:
  TestScreeningAnalysisWizard()
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
    MorrisAnalysis analysis("analysis", model);
    OT::Interval initialBounds(analysis.getBounds());

    // create the wizard
    ScreeningAnalysisWizard wizard(analysis);
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


  void TestMorrisPage()
  {
    // create the analysis
    MorrisAnalysis analysis("analysis", model);
    OT::Interval initialBounds(analysis.getBounds());

    // create the wizard
    ScreeningAnalysisWizard wizard(analysis);
    wizard.show();

    // checks

    // - second page
    wizard.next();
    TemporaryLabel * errorMessageLabel = wizard.morrisPage_->findChild<TemporaryLabel*>();
    QTableView * tableView = wizard.morrisPage_->findChild<QTableView*>();

    QVERIFY2(wizard.currentId() == 1, "Current page ID must be 1");
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->text().isEmpty(), "Label must be empty");

    tableView->model()->setData(tableView->model()->index(0, 2), 10500, Qt::EditRole);
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!errorMessageLabel->text().isEmpty(), "Label must be not empty");

    tableView->model()->setData(tableView->model()->index(0, 2), initialBounds.getLowerBound()[0], Qt::EditRole);
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->text().isEmpty(), "Label must be empty");
  }


  void TestMorris()
  {
    // create the analysis
    MorrisAnalysis analysis("analysis", model);
    OT::Interval initialBounds(analysis.getBounds());

    // create the wizard
    ScreeningAnalysisWizard wizard(analysis);
    wizard.show();

    // checks

    // - first page
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0");
    // - second page
    wizard.next();
    QVERIFY2(wizard.currentId() == 1, "Current page ID must be 1");
    // - third page
    wizard.next();
    QVERIFY2(wizard.currentId() == 2, "Current page ID must be 2");

    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters()==analysis.getParameters();
    QVERIFY2(analysisEquality, "The two MorrisAnalysis must be equal");
  }
};
}

QTEST_MAIN(PERSALYS::TestScreeningAnalysisWizard)
#include "t_ScreeningAnalysisWizard_std.moc"
