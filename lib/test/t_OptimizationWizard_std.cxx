#include "persalys/OptimizationAnalysis.hxx"
#include "persalys/OptimizationWizard.hxx"
#include "persalys/SymbolicPhysicalModel.hxx"

#include <openturns/OTtypes.hxx>
#include <openturns/Normal.hxx>

#include <QtTest/QtTest>

namespace PERSALYS
{
class TestOptimizationWizard : public QObject
{
  Q_OBJECT

public:
  TestOptimizationWizard()
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
    OutputCollection outputCollection(2, Ep);
    outputCollection[0] = Output("Ep", "Primary energy savings");
    outputCollection[1] = Output("EpFake", "Primary energy savings");

    OT::Description formula(2, "1-(Q/((E/((1-0.05)*0.54))+(C/0.8)))");
    model = SymbolicPhysicalModel("model", inputCollection, outputCollection, formula);
  }

private:
  SymbolicPhysicalModel model;

private slots:
  void TestOutputsSelection()
  {
    // create the analysis
    OptimizationAnalysis analysis("analysis", model);
    OT::Interval initialBounds(analysis.getBounds());

    // create the wizard
    OptimizationWizard wizard(analysis);
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
    QTest::mouseClick(listWidget->viewport(), Qt::LeftButton); // select all
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!errorMessageLabel->text().isEmpty(), "Label must be not empty");

    wizard.next();
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0"); // can not go to next page

    QRect rect = listWidget->visualItemRect(listWidget->item(0));
    QTest::mouseClick(listWidget->viewport(), Qt::LeftButton, 0, rect.center()); // deselect all
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!errorMessageLabel->text().isEmpty(), "Label must be not empty");

    rect = listWidget->visualItemRect(listWidget->item(1));
    QTest::mouseClick(listWidget->viewport(), Qt::LeftButton, 0, rect.center()); // check row 1
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->text().isEmpty(), "Label must be empty");
  }


  void TestOptimTable()
  {
    // create the analysis
    OptimizationAnalysis analysis("analysis", model);
    OT::Interval initialBounds(analysis.getBounds());

    // create the wizard
    OptimizationWizard wizard(analysis);
    wizard.show();

    // checks
    OptimizationBoundsPage * boundsPage = wizard.boundsPage_;
    wizard.next();
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(boundsPage->errorMessageLabel_->text().isEmpty(), "Label must be empty");

    // - second page
    boundsPage->tableModel_->setData(boundsPage->tableModel_->index(1, 3), 10300, Qt::EditRole);
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!boundsPage->errorMessageLabel_->text().isEmpty(), "Label must be not empty");

    wizard.next();
    QVERIFY2(wizard.currentId() == 1, "Current page ID must be 1"); // can not go to next page

    wizard.boundsPage_->tableModel_->setData(boundsPage->tableModel_->index(1, 3), 10400, Qt::EditRole);
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!boundsPage->errorMessageLabel_->text().isEmpty(), "Label must be not empty");

    wizard.boundsPage_->tableModel_->setData(boundsPage->tableModel_->index(0, 0), false, Qt::CheckStateRole);
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!boundsPage->errorMessageLabel_->text().isEmpty(), "Label must be not empty");

    boundsPage->tableModel_->setData(boundsPage->tableModel_->index(0, 0), true, Qt::CheckStateRole);
    boundsPage->tableModel_->setData(boundsPage->tableModel_->index(1, 3), initialBounds.getLowerBound()[0], Qt::EditRole);
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(boundsPage->errorMessageLabel_->text().isEmpty(), "Label must be empty");
  }


  void TestOptimisation()
  {
    // create the analysis
    OptimizationAnalysis analysis("analysis", model);
    OT::Interval initialBounds(analysis.getBounds());

    // create the wizard
    OptimizationWizard wizard(analysis);
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
    QVERIFY2(analysisEquality, "The two OptimizationAnalysis must be equal");
  }
};
}

QTEST_MAIN(PERSALYS::TestOptimizationWizard)
#include "t_OptimizationWizard_std.moc"
