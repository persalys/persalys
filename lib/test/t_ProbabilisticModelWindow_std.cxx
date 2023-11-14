#include "persalys/ProbabilisticModelWindow.hxx"
#include "persalys/SymbolicPhysicalModel.hxx"
#include "persalys/MarginalsWidget.hxx"
#include "persalys/StudyTreeViewModel.hxx"
#include "persalys/CheckableHeaderView.hxx"

#include <openturns/OTDistribution.hxx>
#include <openturns/OTtestcode.hxx>

#include <QtTest/QtTest>
#include <QTableView>

namespace PERSALYS
{
class TestProbabilisticModelWindow : public QObject
{
  Q_OBJECT

private:
  ProbabilisticModelItem * probaItem;
  SymbolicPhysicalModel * model;

private slots:
  void initTestCase()
  {
    Input Q("Q", 10200, OT::Normal(10200, 100), "Primary energy");
    Input E("E", 3000, OT::Normal(3000, 15), "Produced electric energy");
    Input C("C", 4000, OT::Normal(4000, 60), "Valued thermal energy");
    Input anInput("anInput", 2);
    Output Ep("Ep", "Primary energy savings");
    InputCollection inputCollection(4);
    inputCollection[0] = Q;
    inputCollection[1] = E;
    inputCollection[2] = C;
    inputCollection[3] = anInput;
    OutputCollection outputCollection(1, Ep);

    OT::Description formula(1, "1-(Q/((E/((1-0.05)*0.54))+(C/0.8)))");
    model = new SymbolicPhysicalModel("model", inputCollection, outputCollection, formula);

    Study aStudy;
    aStudy.add(model);

    StudyTreeViewModel * treeModel = new StudyTreeViewModel;
    treeModel->appendItem(aStudy);

    // Find the ProbabilisticModelItem
    QModelIndexList listIndexes = treeModel->match(treeModel->index(0, 0), Qt::DisplayRole, "Probabilistic model", 1, Qt::MatchRecursive);
    probaItem = dynamic_cast<ProbabilisticModelItem*>(treeModel->itemFromIndex(listIndexes[0]));
  }


  void TestCreationFromFullModel()
  {
    // create the window
    ProbabilisticModelWindow * window = new ProbabilisticModelWindow(probaItem);
    window->show();

    // get widgets
    MarginalsWidget * mainWidget = window->findChild<MarginalsWidget*>();
    QTableView * table = mainWidget->findChild<QTableView*>();
    TemporaryLabel * errorLabel = mainWidget->findChild<TemporaryLabel*>();

    // check
    QVERIFY2(errorLabel->text().isEmpty() == true, "Label must be empty");
    QVERIFY2(table->model()->rowCount() == (int)model->getInputDimension(), "wrong number of rows");
    for (int i = 0; i < table->model()->rowCount(); ++i)
    {
      QVERIFY2(table->model()->data(table->model()->index(i, 0)).toString() == model->getInputNames()[i].c_str(), "wrong name in Table");
      if (model->getInputs()[i].isStochastic())
      {
        QVERIFY2(table->model()->data(table->model()->index(i, 0), Qt::CheckStateRole).toInt() == Qt::Checked, "wrong check state in Table");
        QString distributionName(model->getInputs()[i].getDistribution().getImplementation()->getClassName().c_str());
        QVERIFY2(table->model()->data(table->model()->index(i, 1)).toString() == distributionName, "wrong distribution in Table");
      }
      else
      {
        QVERIFY2(table->model()->data(table->model()->index(i, 0), Qt::CheckStateRole).toInt() == Qt::Unchecked, "wrong check state in Table");
        QVERIFY2(table->model()->data(table->model()->index(i, 1)).toString() == "", "wrong value in Table");
      }
    }
  }


  void TestModificationFromWidgets()
  {
    // create the window
    ProbabilisticModelWindow window(probaItem);
    window.show();

    // get widgets
    MarginalsWidget * mainWidget = window.findChild<MarginalsWidget*>();
    QTableView * inTable = mainWidget->findChild<QTableView*>();
    CheckableHeaderView * headerView = dynamic_cast<CheckableHeaderView*>(inTable->horizontalHeader());
    QList<QCheckBox*> checkBoxes = mainWidget->findChildren<QCheckBox*>();
    ValueLineEdit * lowerBoundEdit = mainWidget->findChild<ValueLineEdit*>("lowerBound");
    ValueLineEdit * upperBoundEdit = mainWidget->findChild<ValueLineEdit*>("upperBound");
    QComboBox * selectParamTypeCombo = mainWidget->findChild<QComboBox*>("paramTypeCombo");
    TemporaryLabel * errorLabel = mainWidget->findChild<TemporaryLabel*>();

    // check

    // change input status (deterministic or probabilistic)
    QVERIFY2(headerView->isChecked() == false, "wrong header check state");
    OT::Distribution inDist(model->getInputs()[0].getDistribution());
    inTable->model()->setData(inTable->model()->index(0, 0), Qt::Unchecked, Qt::CheckStateRole);
    QVERIFY2(model->getInputs()[0].isStochastic() == false, "wrong input status");
    QVERIFY2(model->getInputs()[0].getDistribution() == inDist, "wrong input distribution");

    inTable->model()->setData(inTable->model()->index(0, 0), Qt::Checked, Qt::CheckStateRole);
    QVERIFY2(model->getInputs()[0].getDistribution() == inDist, "wrong input distribution");

    inTable->model()->setData(inTable->model()->index(3, 0), Qt::Checked, Qt::CheckStateRole);
    QVERIFY2(headerView->isChecked() == true, "wrong header check state");
    QVERIFY2(model->getInputs()[3].isStochastic() == true, "wrong input status");
    QVERIFY2(model->getInputs()[3].getDistribution() == OT::Normal(2, 0.2), "wrong input distribution");

    QTest::mousePress(headerView->viewport(), Qt::LeftButton, Qt::NoModifier, headerView->viewport()->rect().bottomLeft()+QPoint(5,2));
    QVERIFY2(model->hasStochasticInputs() == false, "no stochastic inputs");
    QTest::mousePress(headerView->viewport(), Qt::LeftButton, Qt::NoModifier, headerView->viewport()->rect().bottomLeft()+QPoint(5,2));
    QVERIFY2(model->hasStochasticInputs() == true, "must have stochastic inputs");

    // change input distribution
    inTable->model()->setData(inTable->model()->index(3, 1), "Gamma");
    OT::Test::assert_almost_equal(model->getInputs()[3].getDistribution(), OT::Gamma(100.0, 50.0), 1e-5, 1e-8, "wrong input distribution");

    // change distribution configuration type
    QRect rect = inTable->visualRect(inTable->model()->index(3, 0));
    QTest::mouseClick(inTable->viewport(), Qt::LeftButton, Qt::NoModifier, rect.center()); // select line 3
    selectParamTypeCombo->setCurrentIndex(1);
    ValueLineEdit * paramValueEdit = mainWidget->findChild<ValueLineEdit*>("paramValueEdit_0");
    QVERIFY2(paramValueEdit->value() == 2, "wrong mu value");
    paramValueEdit = mainWidget->findChild<ValueLineEdit*>("paramValueEdit_1");
    QVERIFY2(paramValueEdit->value() == 0.2, "wrong sigma value");
    paramValueEdit = mainWidget->findChild<ValueLineEdit*>("paramValueEdit_2");
    QVERIFY2(std::abs(paramValueEdit->value()) < 1e-8, "wrong gamma value");

    selectParamTypeCombo->setCurrentIndex(0);

    // change truncation
    QVERIFY2(checkBoxes[0]->isChecked() == false, "wrong check state");
    QVERIFY2(checkBoxes[1]->isChecked() == false, "wrong check state");
    checkBoxes[0]->setChecked(true);
    checkBoxes[1]->setChecked(true);
    QVERIFY2(lowerBoundEdit->value() == 1.8, "wrong lower bound");
    QVERIFY2(upperBoundEdit->value() == 2.2, "wrong lower bound");
    OT::Test::assert_almost_equal(model->getInputs()[3].getDistribution(), OT::TruncatedDistribution(OT::Gamma(100.0, 50.0), 1.8, 2.2), 1e-5, 1e-8, "wrong input distribution");

    selectParamTypeCombo->setCurrentIndex(1);
    QVERIFY2(model->getInputs()[3].getDistributionParametersType() == 1, "wrong input distribution");

    // change bounds
    lowerBoundEdit->setValue(1.7);
    lowerBoundEdit->setFocus();
    upperBoundEdit->setValue(2.4);
    upperBoundEdit->setFocus();
    OT::Test::assert_almost_equal(model->getInputs()[3].getDistribution(), OT::TruncatedDistribution(OT::Gamma(100.0, 50.0), 1.7, 2.4), 1e-5, 1e-8, "wrong input distribution");

    checkBoxes[0]->setChecked(false);
    checkBoxes[1]->setChecked(false);
    OT::Test::assert_almost_equal(model->getInputs()[3].getDistribution(), OT::Gamma(100.0, 50.0), 1e-5, 1e-8, "wrong input distribution");

    // change distribution param values
    QTest::mouseClick(inTable->viewport(), Qt::LeftButton, Qt::NoModifier, rect.center()); // select line 3
    selectParamTypeCombo->setCurrentIndex(0);
    // - wrong value
    paramValueEdit = mainWidget->findChild<ValueLineEdit*>("paramValueEdit_1");
    paramValueEdit->setValue(-2);
    QTest::keyClick(paramValueEdit, Qt::Key_Enter);
    QVERIFY2(errorLabel->text().isEmpty() == false, "Label must be not empty");
    OT::Test::assert_almost_equal(model->getInputs()[3].getDistribution(), OT::Gamma(100.0, 50.0), 1e-5, 1e-8, "wrong input distribution");

    // - right value
    paramValueEdit->setValue(52);
    QTest::keyClick(paramValueEdit, Qt::Key_Enter);
    OT::Test::assert_almost_equal(model->getInputs()[3].getDistribution(), OT::Gamma(100.0, 52.0), 1e-5, 1e-8, "wrong input distribution");
    QVERIFY2(errorLabel->text().isEmpty() == true, "Label must be empty");
  }


  void TestModificationFromModel()
  {
    // create the window
    ProbabilisticModelWindow window(probaItem);
    window.show();

    // get widgets
    MarginalsWidget * mainWidget = window.findChild<MarginalsWidget*>();
    QTableView * inTable = mainWidget->findChild<QTableView*>();
    ValueLineEdit * valueEdit = mainWidget->findChild<ValueLineEdit*>("valueDeterministicVar");
    QList<QCheckBox*> checkBoxes = mainWidget->findChildren<QCheckBox*>();
    ValueLineEdit * lowerBoundEdit = mainWidget->findChild<ValueLineEdit*>("lowerBound");
    ValueLineEdit * upperBoundEdit = mainWidget->findChild<ValueLineEdit*>("upperBound");

    // checks

    QVERIFY2(valueEdit->isEnabled() == false, "Value editor must be disabled");

    // change inputs param

    // set name
    model->setInputName("anInput", "aNewInput");
    QVERIFY2(inTable->model()->data(inTable->model()->index(3, 0)).toString() == "aNewInput", "wrong name");

    // set state
    model->setInputStochastic("aNewInput", false);
    QVERIFY2(inTable->model()->data(inTable->model()->index(3, 0), Qt::CheckStateRole).toInt() == Qt::Unchecked, "line must be unchecked");

    // set Value
    model->setInputValue("aNewInput", 15000.);
    QRect rect = inTable->visualRect(inTable->model()->index(3, 0));
    QTest::mouseClick(inTable->viewport(), Qt::LeftButton, Qt::NoModifier, rect.center()); // select line 3
    QVERIFY2(valueEdit->text() == "15000", "wrong value");

    // set distribution
    model->setDistribution("aNewInput", OT::Beta(4, 8, -3, 3));
    QVERIFY2(inTable->model()->data(inTable->model()->index(3, 1)).toString() == "Beta", "wrong distribution name");
    QTest::mouseClick(inTable->viewport(), Qt::LeftButton, Qt::NoModifier, rect.center()); // select line 3
    ValueLineEdit * paramValueEdit = mainWidget->findChild<ValueLineEdit*>("paramValueEdit_0");
    QVERIFY2(paramValueEdit->value() == 4, "wrong r param value");
    paramValueEdit = mainWidget->findChild<ValueLineEdit*>("paramValueEdit_1");
    QVERIFY2(paramValueEdit->value() == 8, "wrong t param value");
    paramValueEdit = mainWidget->findChild<ValueLineEdit*>("paramValueEdit_2");
    QVERIFY2(paramValueEdit->value() == -3, "wrong a param value");
    paramValueEdit = mainWidget->findChild<ValueLineEdit*>("paramValueEdit_3");
    QVERIFY2(paramValueEdit->value() == 3, "wrong b param value");

    // set truncated distribution
    model->setDistribution("aNewInput", OT::TruncatedDistribution(OT::LogNormal(3, 1, 2), 4, 78));
    QVERIFY2(inTable->model()->data(inTable->model()->index(3, 1)).toString() == "LogNormal", "wrong distribution name");
    QTest::mouseClick(inTable->viewport(), Qt::LeftButton, Qt::NoModifier, rect.center()); // select line 3
    paramValueEdit = mainWidget->findChild<ValueLineEdit*>("paramValueEdit_0");
    QVERIFY2(paramValueEdit->value() == 3, "wrong mulog param value");
    paramValueEdit = mainWidget->findChild<ValueLineEdit*>("paramValueEdit_1");
    QVERIFY2(paramValueEdit->value() == 1, "wrong sigmalog param value");
    paramValueEdit = mainWidget->findChild<ValueLineEdit*>("paramValueEdit_2");
    QVERIFY2(paramValueEdit->value() == 2, "wrong gamma param value");

    QVERIFY2(checkBoxes[0]->isChecked() == true, "wrong check state");
    QVERIFY2(checkBoxes[1]->isChecked() == true, "wrong check state");
    QVERIFY2(lowerBoundEdit->value() == 4, "wrong lower bound");
    QVERIFY2(upperBoundEdit->value() == 78, "wrong lower bound");

    // remove input
    model->removeInput("aNewInput");
    QVERIFY2(inTable->model()->rowCount() == 3, "wrong number of rows");
    QVERIFY2(inTable->currentIndex() == inTable->model()->index(0, 0), "wrong table line selection");

    // add input
    model->addInput(Input("A", 100.5, "a description"));
    QVERIFY2(inTable->model()->rowCount() == 4, "Table must have 4 lines");
    QVERIFY2(inTable->model()->data(inTable->model()->index(3, 0)).toString() == "A", "wrong name");
    QTest::mouseClick(inTable->viewport(), Qt::LeftButton, Qt::NoModifier, rect.center()); // select line 3
    QVERIFY2(valueEdit->text() == "100.5", "wrong value");
  }
};
}

QTEST_MAIN(PERSALYS::TestProbabilisticModelWindow)
#include "t_ProbabilisticModelWindow_std.moc"
