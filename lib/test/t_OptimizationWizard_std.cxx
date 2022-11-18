#include "persalys/OptimizationAnalysis.hxx"
#include "persalys/OptimizationWizard.hxx"
#include "persalys/SymbolicPhysicalModel.hxx"

#include <openturns/OTtypes.hxx>
#include <openturns/Normal.hxx>

#include <QtTest/QtTest>

using namespace OT;

namespace PERSALYS
{
class TestOptimizationWizard : public QObject
{
  Q_OBJECT

public:
  TestOptimizationWizard()
  {
    // create the model
    Input Q("Q", 10200, Normal(10200, 100), "Primary energy");
    Input E("E", 3000, Normal(3000, 15), "Produced electric energy");
    Input C("C", 4000, Normal(4000, 60), "Valued thermal energy");
    Output Ep("Ep", "Primary energy savings");
    InputCollection inputCollection(3);
    inputCollection[0] = Q;
    inputCollection[1] = E;
    inputCollection[2] = C;
    OutputCollection outputCollection(2, Ep);
    outputCollection[0] = Output("Ep", "Primary energy savings");
    outputCollection[1] = Output("EpFake", "Primary energy savings");

    Description formula(2, "1-(Q/((E/((1-0.05)*0.54))+(C/0.8)))");
    model = SymbolicPhysicalModel("model", inputCollection, outputCollection, formula);
  }

private:
  SymbolicPhysicalModel model;

private slots:
  void TestOutputsSelection()
  {
    // create the analysis
    OptimizationAnalysis analysis("analysis", model);

    // create the wizard
    OptimizationWizard wizard(analysis);
    wizard.show();

    // checks
    OptimizationBoundsPage * boundsPage = wizard.boundsPage_;
    wizard.next();
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(boundsPage->errorMessageLabel_->text().isEmpty(), "Label must be empty");

    // - second page
    OutputsSelectionGroupBox * outputsSelectionGroupBox = wizard.algoPage_->findChild<OutputsSelectionGroupBox*>();
    TemporaryLabel * errorMessageLabel = wizard.algoPage_->findChild<TemporaryLabel*>();
    TitledComboBox * comboBox = outputsSelectionGroupBox->findChild<TitledComboBox*>();
    ListWidgetWithCheckBox * listWidget = outputsSelectionGroupBox->findChild<ListWidgetWithCheckBox*>();

    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->text().isEmpty(), "Label must be empty");

    QTest::mouseClick(comboBox, Qt::LeftButton); // open listwidget
    QTest::mouseClick(listWidget->viewport(), Qt::LeftButton); // select all
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!errorMessageLabel->text().isEmpty(), "Label must be not empty");

    wizard.next();
    QVERIFY2(wizard.currentId() == 1, "Current page ID must be 1"); // can not go to next page

    QRect rect = listWidget->visualItemRect(listWidget->item(0));
    QTest::mouseClick(listWidget->viewport(), Qt::LeftButton, Qt::NoModifier, rect.center()); // deselect all
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!errorMessageLabel->text().isEmpty(), "Label must be not empty");

    rect = listWidget->visualItemRect(listWidget->item(1));
    QTest::mouseClick(listWidget->viewport(), Qt::LeftButton, Qt::NoModifier, rect.center()); // check row 1
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->text().isEmpty(), "Label must be empty");
  }

  void TestOptimTable()
  {
    // create the analysis
    OptimizationAnalysis analysis("analysis", model);
    Interval initBounds(analysis.getBounds());
    Description variableInput(2);
    variableInput[0] = "Q";
    variableInput[1] = "E";
    analysis.setVariableInputs(variableInput);

    // create the wizard
    OptimizationWizard wizard(analysis);
    wizard.show();

    // checks
    OptimizationBoundsPage * boundsPage = wizard.boundsPage_;

    OptimizationTableModel * tableModel = boundsPage->tableModel_;
    // - first page
    QVERIFY2(tableModel->data(tableModel->index(0, 0), Qt::CheckStateRole).toInt() == Qt::Unchecked, "Header must be unchecked");
    for (int i = 1; i < 3; ++i)
      QVERIFY2(tableModel->data(tableModel->index(i, 0), Qt::CheckStateRole).toInt() == Qt::Checked, "Rows must be checked");
    QVERIFY2(tableModel->data(tableModel->index(3, 0), Qt::CheckStateRole).toInt() == Qt::Unchecked, "Last row must be unchecked");

    QVERIFY2(tableModel->data(tableModel->index(1, 0)).toString() == "Q", "wrong name");
    QVERIFY2(tableModel->data(tableModel->index(1, 1)).toString() == "Primary energy", "wrong description");
    QVERIFY2(tableModel->data(tableModel->index(1, 2)).toString() == "Continuous", "wrong value");
    QVERIFY2(tableModel->data(tableModel->index(1, 3)).toString() == "10200", "wrong value");
    QVERIFY2(tableModel->data(tableModel->index(1, 4)).toString() == QString::number(initBounds.getLowerBound()[0], 'g', 12), "wrong lower bound value");
    QVERIFY2(tableModel->data(tableModel->index(1, 5)).toString() == QString::number(initBounds.getUpperBound()[0], 'g', 12), "wrong upper bound value");

    // change value
    tableModel->setData(tableModel->index(1, 3), 10400, Qt::EditRole);
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!boundsPage->errorMessageLabel_->text().isEmpty(), "Label must be not empty");
    tableModel->setData(tableModel->index(1, 3), 10200, Qt::EditRole);
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(boundsPage->errorMessageLabel_->text().isEmpty(), "Label must be empty");

    // change lower bound
    tableModel->setData(tableModel->index(1, 4), 10400, Qt::EditRole);
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!boundsPage->errorMessageLabel_->text().isEmpty(), "Label must be not empty");
    tableModel->setData(tableModel->index(1, 4), initBounds.getLowerBound()[0], Qt::EditRole);
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(boundsPage->errorMessageLabel_->text().isEmpty(), "Label must be empty");

    // change variable check state
    wizard.boundsPage_->tableModel_->setData(tableModel->index(3, 0), Qt::Checked, Qt::CheckStateRole);
    QVERIFY2(boundsPage->tableModel_->getAnalysis().getVariableInputs() == model.getInputNames(), "wrong variable inputs number");

    wizard.boundsPage_->tableModel_->setData(tableModel->index(0, 0), Qt::Unchecked, Qt::CheckStateRole);
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!boundsPage->errorMessageLabel_->text().isEmpty(), "Label must be not empty");
    wizard.boundsPage_->tableModel_->setData(tableModel->index(1, 0), Qt::Checked, Qt::CheckStateRole);
    wizard.boundsPage_->tableModel_->setData(tableModel->index(2, 0), Qt::Checked, Qt::CheckStateRole);
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(boundsPage->errorMessageLabel_->text().isEmpty(), "Label must be empty");

    // change bounds check state
    wizard.boundsPage_->tableModel_->setData(tableModel->index(1, 4), Qt::Unchecked, Qt::CheckStateRole);
    QVERIFY2(tableModel->data(tableModel->index(1, 4)).toString() == "-∞", "wrong lower bound value");
    wizard.boundsPage_->tableModel_->setData(tableModel->index(1, 5), Qt::Unchecked, Qt::CheckStateRole);
    QVERIFY2(tableModel->data(tableModel->index(1, 5)).toString() == "+∞", "wrong upper bound value");

    wizard.boundsPage_->tableModel_->setData(tableModel->index(1, 4), Qt::Checked, Qt::CheckStateRole);
    wizard.boundsPage_->tableModel_->setData(tableModel->index(1, 5), Qt::Checked, Qt::CheckStateRole);

    QVERIFY2(wizard.boundsPage_->validatePage(), "Page must be valid");
    QVERIFY2(wizard.algoPage_->validatePage(), "Page must be valid");

    ConstraintsPage * cstrPage = wizard.cstrPage_;
    ConstraintsTableModel * cstrTableModel = cstrPage->getTableModel();

    QVERIFY2(cstrTableModel->rowCount() == 0, "Table must be empty");
    cstrTableModel->addLine();
    QVERIFY2(cstrTableModel->data(tableModel->index(0, 0)).toString() == "Ep", "wrong left part");
    QVERIFY2(cstrTableModel->data(tableModel->index(0, 1)).toString() == ">", "wrong operator");
    QVERIFY2(cstrTableModel->data(tableModel->index(0, 2)).toString() == "0", "wrong right part");
    cstrTableModel->removeLine(tableModel->index(0, 0));
    QVERIFY2(wizard.cstrPage_->validatePage(), "Page must be valid");

    bool analysisEquality = wizard.getAnalysis().getParameters()==analysis.getParameters();
    QVERIFY2(analysisEquality, "The two OptimizationAnalysis must be equal");


  }

  void TestOptimisation()
  {
    // create the analysis
    OptimizationAnalysis analysis("analysis", model);

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

    // - fourth page
    wizard.next();
    QVERIFY2(wizard.currentId() == 3, "Current page ID must be 3");

    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters()==analysis.getParameters();
    QVERIFY2(analysisEquality, "The two OptimizationAnalysis must be equal");
  }
};
}

QTEST_MAIN(PERSALYS::TestOptimizationWizard)
#include "t_OptimizationWizard_std.moc"
