#include "persalys/SymbolicPhysicalModelWindow.hxx"
#include "persalys/SymbolicPhysicalModel.hxx"
#include "persalys/PhysicalModelWindowWidget.hxx"
#include "persalys/CheckableHeaderView.hxx"
#include "persalys/DifferentiationTableModel.hxx"
#include "persalys/PhysicalModelDefinitionItem.hxx"

#include <openturns/OTtypes.hxx>
#include <openturns/Normal.hxx>

#include <QtTest/QtTest>
#include <QPushButton>

namespace PERSALYS
{
class TestSymbolicPhysicalModelWindow : public QObject
{
  Q_OBJECT

private slots:
  void TestModificationFromWidgets()
  {
    // create the item
    PhysicalModelDefinitionItem * item = new PhysicalModelDefinitionItem(SymbolicPhysicalModel());

    // create the window
    SymbolicPhysicalModelWindow window(item);
    window.show();

    // get widgets
    PhysicalModelWindowWidget * mainWidget = window.findChild<PhysicalModelWindowWidget*>();
    QList<CopyableTableView*> listTables = mainWidget->findChildren<CopyableTableView*>();
    TemporaryLabel * errorLabel = mainWidget->findChild<TemporaryLabel*>();
    QList<QPushButton*> pushButtons = mainWidget->findChildren<QPushButton*>();
    CopyableTableView * inTable = 0;
    CopyableTableView * outTable = 0;
    CopyableTableView * diffTable = 0;
    for (int i = 0; i < listTables.size(); ++i)
    {
      if (listTables[i]->model()->columnCount() == 2)
        diffTable = listTables[i];
      else if (dynamic_cast<CheckableHeaderView*>(listTables[i]->horizontalHeader()))
        outTable = listTables[i];
      else
        inTable = listTables[i];
    }
    DifferentiationTableModel * diffTableModel = dynamic_cast<DifferentiationTableModel*>(diffTable->model());
    QPushButton * evaluateButton = 0;
    for (int i = 0; i < pushButtons.size(); ++i)
    {
      if (pushButtons[i]->text() == "Check model")
      {
        evaluateButton = pushButtons[i];
      }
    }

    // check

    // add 2 inputs and 2 outputs
    for (int i = 0; i < pushButtons.size(); ++i)
    {
      if (pushButtons[i]->text() == "Add")
      {
        pushButtons[i]->click();
        pushButtons[i]->click();
      }
    }
    for (int i = 0; i < listTables.size(); ++i)
    {
      QVERIFY2(listTables[i]->model()->rowCount() == 2, "Each table must have 2 lines");
    }
    QVERIFY2(item->getPhysicalModel().getInputs()[0] == Input("X0"), "wrong input");
    QVERIFY2(item->getPhysicalModel().getOutputs()[0] == Output("Y0"), "wrong output");
    QVERIFY2(diffTable->model()->data(diffTable->model()->index(0, 0)).toString() == "X0", "wrong name in diff Table");
    QVERIFY2(diffTable->model()->data(diffTable->model()->index(0, 1)).toDouble() == 1e-7, "wrong step in diff Table");

    // edit input param
    inTable->model()->setData(inTable->model()->index(0, 0), "Input0");
    inTable->model()->setData(inTable->model()->index(0, 1), "first input");
    inTable->model()->setData(inTable->model()->index(0, 2), 3.);
    QVERIFY2(item->getPhysicalModel().getInputs()[0] == Input("Input0", 3., "first input"), "wrong input");
    QVERIFY2(diffTable->model()->data(diffTable->model()->index(0, 0)).toString() == "Input0", "wrong name in diff Table");

    // edit output param
    outTable->model()->setData(outTable->model()->index(0, 0), "Output0");
    outTable->model()->setData(outTable->model()->index(0, 1), "first output");
    outTable->model()->setData(outTable->model()->index(0, 2), "Input0 * 2");
    QVERIFY2(item->getPhysicalModel().getOutputs()[0] == Output("Output0", "first output"), "wrong output");
    QVERIFY2(outTable->model()->data(outTable->model()->index(0, 3)).toString() == "?", "wrong output value");

    outTable->model()->setData(outTable->model()->index(1, 2), "Input0 + 2");

    // uncheck Y1 + evaluate
    outTable->model()->setData(outTable->model()->index(1, 0), Qt::Unchecked, Qt::CheckStateRole);
    evaluateButton->click();
    QVERIFY2(outTable->model()->data(outTable->model()->index(0, 3)).toString() == "6", "wrong output value");
    QVERIFY2(outTable->model()->data(outTable->model()->index(1, 3)).toString() == "?", "wrong output value");

    // check Y1
    outTable->model()->setData(outTable->model()->index(1, 0), Qt::Checked, Qt::CheckStateRole);
    QVERIFY2(dynamic_cast<CheckableHeaderView*>(outTable->horizontalHeader())->isChecked(), "header must be checked");

    // evaluate
    evaluateButton->click();
    QVERIFY2(outTable->model()->data(outTable->model()->index(1, 3)).toString() == "5", "wrong output value");

    // change an input value
    inTable->model()->setData(inTable->model()->index(0, 2), 7.);
    QVERIFY2(outTable->model()->data(outTable->model()->index(0, 3)).toString() == "?", "wrong output value");
    QVERIFY2(outTable->model()->data(outTable->model()->index(1, 3)).toString() == "?", "wrong output value");

    // uncheck all outputs
    outTable->model()->setData(outTable->model()->index(0, 0), Qt::Unchecked, Qt::CheckStateRole);
    outTable->model()->setData(outTable->model()->index(1, 0), Qt::Unchecked, Qt::CheckStateRole);
    QVERIFY2(!dynamic_cast<CheckableHeaderView*>(outTable->horizontalHeader())->isChecked(), "header must be unchecked");

    // evaluate
    evaluateButton->click();
    QVERIFY2(outTable->model()->data(outTable->model()->index(0, 3)).toString() == "?", "wrong output value");
    QVERIFY2(outTable->model()->data(outTable->model()->index(1, 3)).toString() == "?", "wrong output value");

    // check all outputs + set wrong Y1 formula + evaluate
    outTable->model()->setData(outTable->model()->index(0, 0), Qt::Checked, Qt::CheckStateRole);
    outTable->model()->setData(outTable->model()->index(1, 0), Qt::Checked, Qt::CheckStateRole);
    outTable->model()->setData(outTable->model()->index(1, 2), "Input0 + ");
    evaluateButton->click();
    QVERIFY2(outTable->model()->data(outTable->model()->index(0, 3)).toString() == "?", "wrong output value");
    QVERIFY2(outTable->model()->data(outTable->model()->index(1, 3)).toString() == "?", "wrong output value");
    QVERIFY2(errorLabel->text().isEmpty() == false, "label not empty");

    // uncheck Y1 + evaluate
    outTable->model()->setData(outTable->model()->index(1, 0), Qt::Unchecked, Qt::CheckStateRole);
    evaluateButton->click();
    QVERIFY2(errorLabel->text().isEmpty() == true, "label must be empty");

    // change finite difference step
    diffTable->model()->setData(diffTable->model()->index(0, 1), 1e-3);
    QVERIFY2(item->getPhysicalModel().getInputs()[0].getFiniteDifferenceStep() == 1e-3, "wrong input finite diff step");
    diffTableModel->applyValueToAll(1e-3);
    QVERIFY2(item->getPhysicalModel().getInputs()[1].getFiniteDifferenceStep() == 1e-3, "wrong input finite diff step");

    // remove variables
    for (int i = 0; i < pushButtons.size(); ++i)
    {
      if (pushButtons[i]->text() == "Remove")
      {
        inTable->selectRow(0);
        outTable->selectRow(0);
        pushButtons[i]->click();
        inTable->selectRow(0);
        outTable->selectRow(0);
        pushButtons[i]->click();
      }
    }
    for (int i = 0; i < listTables.size(); ++i)
    {
      QVERIFY2(listTables[i]->model()->rowCount() == 0, "Each table must have 0 line");
    }
  }


  void TestModificationFromModel()
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
    SymbolicPhysicalModel * model = new SymbolicPhysicalModel("model", inputCollection, outputCollection, formula);

    // create the item
    PhysicalModelDefinitionItem * item = new PhysicalModelDefinitionItem(PhysicalModel(model));

    // create the window
    SymbolicPhysicalModelWindow window(item);
    window.show();

    // get widgets
    PhysicalModelWindowWidget * mainWidget = window.findChild<PhysicalModelWindowWidget*>();
    QList<CopyableTableView*> listTables = mainWidget->findChildren<CopyableTableView*>();
    CopyableTableView * inTable = 0;
    CopyableTableView * outTable = 0;
    CopyableTableView * diffTable = 0;
    for (int i = 0; i < listTables.size(); ++i)
    {
      if (listTables[i]->model()->columnCount() == 2)
        diffTable = listTables[i];
      else if (dynamic_cast<CheckableHeaderView*>(listTables[i]->horizontalHeader()))
        outTable = listTables[i];
      else
        inTable = listTables[i];
    }

    // checks
    QVERIFY2(inTable->model()->rowCount() == 3, "Input table must have 3 lines");
    QVERIFY2(diffTable->model()->rowCount() == 3, "Finite difference table must have 3 lines");
    QVERIFY2(outTable->model()->rowCount() == 1, "Output table must have 1 line");

    // change inputs param
    model->setInputName("Q", "newQ");
    QVERIFY2(inTable->model()->data(inTable->model()->index(0, 0)).toString() == "newQ", "wrong name");
    QVERIFY2(diffTable->model()->data(diffTable->model()->index(0, 0)).toString() == "newQ", "wrong name");
    model->setInputDescription("newQ", "new Primary energy");
    QVERIFY2(inTable->model()->data(inTable->model()->index(0, 1)).toString() == "new Primary energy", "wrong description");
    model->setInputValue("newQ", 15000.);
    QVERIFY2(inTable->model()->data(inTable->model()->index(0, 2)).toString() == "15000", "wrong value");
    model->setFiniteDifferenceStep("newQ", 0.001);
    QVERIFY2(diffTable->model()->data(diffTable->model()->index(0, 1)).toString() == "0.001", "wrong step value");

    model->addInput(Input("A", 0.5, "a description"));
    QVERIFY2(inTable->model()->rowCount() == 4, "Input table must have 4 lines");
    QVERIFY2(diffTable->model()->rowCount() == 4, "Finite difference table must have 4 lines");
    QVERIFY2(inTable->model()->data(inTable->model()->index(3, 0)).toString() == "A", "wrong name");
    QVERIFY2(diffTable->model()->data(diffTable->model()->index(3, 0)).toString() == "A", "wrong name");
    QVERIFY2(inTable->model()->data(inTable->model()->index(3, 1)).toString() == "a description", "wrong description");
    QVERIFY2(inTable->model()->data(inTable->model()->index(3, 2)).toString() == "0.5", "wrong value");
    model->removeInput("A");
    QVERIFY2(inTable->model()->rowCount() == 3, "Input table must have 3 lines");

    // change outputs param
    model->setOutputName("Ep", "newEp");
    QVERIFY2(outTable->model()->data(outTable->model()->index(0, 0)).toString() == "newEp", "wrong name");
    model->setOutputDescription("newEp", "new Primary energy savings");
    QVERIFY2(outTable->model()->data(outTable->model()->index(0, 1)).toString() == "new Primary energy savings", "wrong description");
    model->setFormula("newEp", "1-(Q/((E/((1-0.05)*0.54))+(C/0.8))) + 2.");
    QVERIFY2(outTable->model()->data(outTable->model()->index(0, 2)).toString() == "1-(Q/((E/((1-0.05)*0.54))+(C/0.8))) + 2.", "wrong formula");
    model->selectOutput("newEp", false);
    QVERIFY2(outTable->model()->data(outTable->model()->index(0, 0), Qt::CheckStateRole).toInt() == Qt::Unchecked, "wrong check state");

    model->addOutput(Output("B", "a description"));
    QVERIFY2(outTable->model()->rowCount() == 2, "Input table must have 2 lines");
    QVERIFY2(outTable->model()->data(outTable->model()->index(1, 0)).toString() == "B", "wrong name");
    QVERIFY2(outTable->model()->data(outTable->model()->index(1, 1)).toString() == "a description", "wrong description");
    QVERIFY2(outTable->model()->data(outTable->model()->index(1, 2)).toString() == "", "wrong value");
    model->removeOutput("B");
    QVERIFY2(outTable->model()->rowCount() == 1, "Input table must have 1 line");
  }
};
}

QTEST_MAIN(PERSALYS::TestSymbolicPhysicalModelWindow)
#include "t_SymbolicPhysicalModelWindow_std.moc"
